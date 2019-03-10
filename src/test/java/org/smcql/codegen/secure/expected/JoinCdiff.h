#include <emp-sh2pc/emp-sh2pc.h>
#include <pqxx/pqxx>
#include "row.h"

// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;
using namespace pqxx;




namespace JoinCdiff {

class JoinCdiffClass {
// Connection strings, encapsulates db name, db user, port, host
string aliceConnectionString = "dbname=smcql_testdb_site1 user=smcql host=127.0.0.1 port=5432";
string bobConnectionString = "dbname=smcql_testdb_site2 user=smcql host=127.0.0.1 port=5432";

string aliceHost = "127.0.0.1";
int LENGTH_INT = 64;
string output;
map<string, string> inputs; // maps opName --> bitString of input tuples


// Helper functions
string reveal_bin(Integer &input, int length, int output_party) {
    bool * b = new bool[length];
    ProtocolExecution::prot_exec->reveal(b, output_party, (block *)input.bits,  length);
    string bin="";
    
    for (int i=0; i<length; i++)
        bin += (b[i] ? '1':'0');
    
    delete [] b;
    return bin;
}

	bool * outputBits(Integer &input, int length, int output_party) {
		bool * b = new bool[length];
		ProtocolExecution::prot_exec->reveal(b, output_party, (block *)input.bits,  length);
		string bin="";

		return b;
	}


void cmp_swap_sql(Integer*key, int i, int j, Bit acc, int key_pos, int key_length) {
    Integer keyi = Integer(key_length, key[i].bits+key_pos);
    Integer keyj = Integer(key_length, key[j].bits+key_pos);
    Bit to_swap = ((keyi > keyj) == acc);
    swap(to_swap, key[i], key[j]);
}

// TODO: extend this to multiple columns as a list of key_pos and key_length
void bitonic_merge_sql(Integer* key, int lo, int n, Bit acc, int key_pos, int key_length) {
    if (n > 1) {
        int m = greatestPowerOfTwoLessThan(n);
        for (int i = lo; i < lo + n - m; i++)
            cmp_swap_sql(key, i, i + m, acc, key_pos, key_length);
        bitonic_merge_sql(key, lo, m, acc, key_pos, key_length);
        bitonic_merge_sql(key, lo + m, n - m, acc, key_pos, key_length);
    }
}

void bitonic_sort_sql(Integer * key, int lo, int n, Bit acc,  int key_pos, int key_length) {
    if (n > 1) {
        int m = n / 2;
        bitonic_sort_sql(key, lo, m, !acc, key_pos, key_length);
        bitonic_sort_sql(key, lo + m, n - m, acc, key_pos, key_length);
        bitonic_merge_sql(key, lo, n, acc, key_pos, key_length);
    }
}

bool *concat(std::vector<Row> rows, int row_size) {
    bool *result = new bool[row_size*rows.size()];
    int num_rows = rows.size();

    for (int i=0; i<num_rows; i++) {
        memcpy(result + i*row_size, rows[i].to_bool(), row_size);   
    }
    return result;
}

int sum_vals(vector<int> vec) {
    int sum = 0;
    for (int i : vec)
        sum += i;
    return sum;
}

// DB connection functions
std::vector<Row> execute_sql(string sql, int party) {
    std::vector<Row> res;
    try {
        string config= (party == ALICE) ? aliceConnectionString : bobConnectionString;
        connection C(config);

        if (C.is_open()) {
            cout << "Running " << sql << endl;

            cout << "Opened database successfully: " << C.dbname() << endl;
            work w(C);
            result r = w.exec(sql);
            w.commit();
			cout << "Received query results!" << endl;
			
            for (auto row : r) {
            	const int num_cols = row.size();
            	vector<int>lengths;
            	string bin_str = "";
            	for (int j=0; j<num_cols; j++) {
            	    int val;
                    string str;
                    try {
                        row[j].to(val);
                        lengths.push_back(LENGTH_INT);
                        bin_str += int64_to_binstr(val);
                    } catch (const std::exception& ex) {
                        try {
                            row[j].to(str);
                            lengths.push_back(str.length());
                            bin_str += str_to_binary(str, str.length());
                        } catch (const std::exception& ex) {
                            throw runtime_error("Unsupported data type in column");
                        }
                    }
            	}
            	res.push_back(Row(bin_str, lengths));
            }
        } else {
            cout << "Can't open database" << endl;
        }
        C.disconnect();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }

    return res;
}

// Operator functions
class Data {
public:
    Integer * data;
    int publicSize;
    Integer realSize;
    Integer dummyTags;
};

Integer from_bool(bool* b, int size, int party) {
    Integer res;
    res.bits = new Bit[size];
    init(res.bits, b, size, party);
    return res;
}


Data* SeqScan0Merge(int party, NetIO * io) {
    cout << "Running SeqScan0Merge!" << endl;
    string sql = "SELECT patient_id, icd9 = '008.45' FROM (SELECT patient_id, icd9 FROM diagnoses) AS t ORDER BY patient_id";
    std::vector<Row> in = execute_sql(sql, party);
    
    cout << "Received query results" << endl;
    int rowLength = 65; 
    bool *localData = concat(in, rowLength);


    int aliceSize = in.size();
    int bobSize = in.size();

    if (party == ALICE) {
        io->send_data(&aliceSize, 4);
        io->flush();
        io->recv_data(&bobSize, 4);
        io->flush();
    } else if (party == BOB) {
        io->recv_data(&aliceSize, 4);
        io->flush();
        io->send_data(&bobSize, 4);
        io->flush();
    }

    Integer * res = new Integer[aliceSize + bobSize];  // enough space for all inputs
   
    Bit * tmp = new Bit[rowLength * (aliceSize + bobSize)]; //  bit array of inputs
    Bit *tmpPtr = tmp;

    Batcher aliceBatcher, bobBatcher;
   
    int bobBits = bobSize * rowLength;
    int aliceBits = aliceSize * rowLength;

    for (int i = 0; i < bobBits; ++i) {
    	bobBatcher.add<Bit>((BOB==party) ? localData[i]:0);
     }
     
    bobBatcher.make_semi_honest(BOB);



    // append all of bob's bits to tmp
    for (int i = 0; i < bobBits; ++i) {
        *tmpPtr = bobBatcher.next<Bit>();
        ++tmpPtr;
	}
   

    for (int i = 0; i < aliceBits; ++i) {
    	aliceBatcher.add<Bit>((ALICE==party) ? localData[i]:0);
    }

    aliceBatcher.make_semi_honest(ALICE);


    for(int i = 0; i < aliceBits; ++i) {
    	    *tmpPtr = aliceBatcher.next<Bit>();
	    	    ++tmpPtr;
		    }

   		    // resetting cursor
		    tmpPtr = tmp;
		    
		    // create a 2D array of secret-shared bits
		    // each index is a tuple
    for(int i = 0; i < aliceSize + bobSize; ++i) {
        res[i] = Integer(rowLength, tmpPtr);
        tmpPtr += rowLength;
     }

    // TODO: make sort more robust.  Handle sort keys that are not adjacent or in the same order in the table
    bitonic_merge_sql(res, 0, aliceSize + bobSize, Bit(true), 0, 64);

    Data * d = new Data;
    d->data = res;
    d->publicSize = aliceSize + bobSize;
    d->realSize = Integer(64, d->publicSize, PUBLIC);
    cout << "Done union!" << endl;   
    return d;
}
Data* SeqScan4Merge(int party, NetIO * io) {
    cout << "Running SeqScan4Merge!" << endl;
    string sql = "SELECT patient_id FROM medications ORDER BY patient_id";
    std::vector<Row> in = execute_sql(sql, party);
    
    cout << "Received query results" << endl;
    int rowLength = 64; 
    bool *localData = concat(in, rowLength);


    int aliceSize = in.size();
    int bobSize = in.size();

    if (party == ALICE) {
        io->send_data(&aliceSize, 4);
        io->flush();
        io->recv_data(&bobSize, 4);
        io->flush();
    } else if (party == BOB) {
        io->recv_data(&aliceSize, 4);
        io->flush();
        io->send_data(&bobSize, 4);
        io->flush();
    }

    Integer * res = new Integer[aliceSize + bobSize];  // enough space for all inputs
   
    Bit * tmp = new Bit[rowLength * (aliceSize + bobSize)]; //  bit array of inputs
    Bit *tmpPtr = tmp;

    Batcher aliceBatcher, bobBatcher;
   
    int bobBits = bobSize * rowLength;
    int aliceBits = aliceSize * rowLength;

    for (int i = 0; i < bobBits; ++i) {
    	bobBatcher.add<Bit>((BOB==party) ? localData[i]:0);
     }
     
    bobBatcher.make_semi_honest(BOB);



    // append all of bob's bits to tmp
    for (int i = 0; i < bobBits; ++i) {
        *tmpPtr = bobBatcher.next<Bit>();
        ++tmpPtr;
	}
   

    for (int i = 0; i < aliceBits; ++i) {
    	aliceBatcher.add<Bit>((ALICE==party) ? localData[i]:0);
    }

    aliceBatcher.make_semi_honest(ALICE);


    for(int i = 0; i < aliceBits; ++i) {
    	    *tmpPtr = aliceBatcher.next<Bit>();
	    	    ++tmpPtr;
		    }

   		    // resetting cursor
		    tmpPtr = tmp;
		    
		    // create a 2D array of secret-shared bits
		    // each index is a tuple
    for(int i = 0; i < aliceSize + bobSize; ++i) {
        res[i] = Integer(rowLength, tmpPtr);
        tmpPtr += rowLength;
     }

    // TODO: make sort more robust.  Handle sort keys that are not adjacent or in the same order in the table
    bitonic_merge_sql(res, 0, aliceSize + bobSize, Bit(true), 0, 64);

    Data * d = new Data;
    d->data = res;
    d->publicSize = aliceSize + bobSize;
    d->realSize = Integer(64, d->publicSize, PUBLIC);
    cout << "Done union!" << endl;   
    return d;
}
// TODO: generalize for arbitrary selection criteria
// see ObliVM generator for example of this
// this example is based on aspirin expected code
Data * Join6(Data *left, Data *right) {
    Integer *output = new Integer[left->publicSize * right->publicSize];
    int writeIdx = 0;
    Integer dstTuple, srcTuple, lTuple, rTuple;
    int jointSchemaSize = 64 + 64;
    srcTuple = Integer();
    srcTuple.resize(jointSchemaSize);
    
    dstTuple = Integer(64, 0, PUBLIC); // indicates dummy
    
    for (int i=0; i<left->publicSize; i++) {
        for (int j=0; j<right->publicSize; j++) {
        	lTuple = left->data[i];
        	rTuple = right->data[j];
        	// concatenate the two inputs into srcTuple, the standard join output schema
        	memcpy(srcTuple.bits, lTuple.bits, 64);
        	memcpy(srcTuple.bits + 64, rTuple.bits, 64);
        	
        	Bit cmp = (Integer(64, lTuple.bits) == Integer(64, rTuple.bits)) & ((lTuple.bits[64] == Bit(1, PUBLIC)));
        	memcpy(dstTuple.bits , Integer(64, srcTuple.bits).bits, 64);
;
        	dstTuple = If(cmp, dstTuple, Integer(64, 0, PUBLIC)); 
	        output[writeIdx] = dstTuple;
            writeIdx++;
            
        }
    }
    Data *result = new Data;
    result->data = output;
    // output tuple count
    result->publicSize = left->publicSize * right->publicSize;
	// real size needs to be maintained by counting the times the join criteria is met
	// this variable should probably not be public
    result->realSize = Integer(64, left->publicSize, PUBLIC);
    return result;
}


// expects as arguments party (1 = alice, 2 = bob) plus the port it will run the protocols over

public:
void setGeneratorHost(string host) {
     aliceHost = host;
}


// placeholder for maintaining a map of inputs from JDBC
void addInput(const std::string& opName, const std::string& bitString) {

     inputs[opName] = bitString;

     }


const std::string& getOutput() {
      return output;
}

	void run(int party, int port) {
	
	std::cout << "starting run in emp! party=" << party << " port=" << port <<  std::endl;
	NetIO * io = new NetIO((party==ALICE ? nullptr : aliceHost.c_str()), port);
    
    setup_semi_honest(io, party);
    
     Data *SeqScan0MergeOutput = SeqScan0Merge(party, io);

     Data *SeqScan4MergeOutput = SeqScan4Merge(party, io);

    Data * Join6Output = Join6(SeqScan0MergeOutput, SeqScan4MergeOutput);


    
    	Data * results = Join6Output;

     int tupleWidth = results->data[0].size();
     long outputSize = results->publicSize * tupleWidth;
     output.reserve(outputSize);
     bool *tuple;


     for(int i = 0; i < results->publicSize; ++i) {
     	     tuple = outputBits(results->data[i], tupleWidth, XOR);
     	     for(int j = 0; j < tupleWidth; ++j) {
     	     	     output += (tuple[j] == true) ? '1' : '0';
     		     }
     }

     io->flush();
     delete io;

		cout << "Finished query!" << endl;
		
	}








int main(int argc, char** argv) { 
	int party, port;
	parse_party_and_port(argv, 2, &party, &port);
	run(party, port);
	return 0;
}
}; // end class
} // end namespace
 