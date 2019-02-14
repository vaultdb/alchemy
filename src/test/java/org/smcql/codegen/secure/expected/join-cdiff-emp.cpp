#include <emp-sh2pc/emp-sh2pc.h>
#include <pqxx/pqxx>
#include <utils/row.h>

// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;
using namespace pqxx;

#define LENGTH_INT 64

#define OID_INT 20

// Connection strings, encapsulates db name, db user, port, host
string aliceConnectionString = "dbname=smcql_testdb_site1 user=smcql host=localhost port=5432";
string bobConnectionString = "dbname=smcql_testdb_site2 user=smcql host=localhost port=5432";
string aliceHost = "localhost";
string bobHost = "localhost";





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
            cout << "Opened database successfully: " << C.dbname() << endl;
            work w(C);
            result r = w.exec(sql);
            w.commit();

            for (auto row : r) {
            	const int num_cols = row.size();
            	vector<int>lengths;
            	string bin_str = "";
            	for (int j=0; j<num_cols; j++) {
            	    const pqxx::field field = row[j];
            	    int oid = field.type();
            	   /* if (oid == OID_STRING) { // TODO: TODO: what happens if we have two string fields in the same query? 
            	        lengths.push_back(LENGTH_STRING);  //   implement this using pqxx::binary_string size methods
            	                                          // May need to parameterize schema, e.g., define column icd9 as varchar(7) 
            	        bin_str += str_to_binary(row[j].as<string>(), LENGTH_STRING);
            	    } else*/
            	   if (oid == OID_INT) {
            	        lengths.push_back(LENGTH_INT);
            	        bin_str += int64_to_binstr(row[j].as<int64_t>());
            	    } else {
            	        throw "Unsupported data type in column";
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
    int public_size;
    Integer real_size;
    Integer dummyTags;
};

Integer from_bool(bool* b, int size, int party) {
    Integer res;
    res.bits = new Bit[size];
    init(res.bits, b, size, party);
    return res;
}


// ordered union, multiset op, has multiset semantics
// inputs must be sorted by sort key from query plan in source DBs
// right now this presumes that the sort key is always the first column.
// need to make this more general by tweaking the parameters 


Data* SeqScan0Merge(int party, NetIO * io) {
	string sql = "SELECT patient_id, icd9 = '008.45' FROM (SELECT patient_id, icd9 FROM diagnoses) AS t ORDER BY patient_id";
    std::vector<Row> in = execute_sql(sql, party);
    int bit_length = 65; // TODO: automatically derive size from Row metadata?
    // flatten out local data
    bool *local_data = concat(in, bit_length);

    int alice_size, bob_size;
    alice_size = bob_size = in.size();

    if (party == ALICE) {
        io->send_data(&alice_size, 4);
        io->flush();
        io->recv_data(&bob_size, 4);
        io->flush();
    } else if (party == BOB) {
        io->recv_data(&alice_size, 4);
        io->flush();
        io->send_data(&bob_size, 4);
        io->flush();
    }

	Integer * res = new Integer[alice_size + bob_size];  // enough space for all inputs
   
    Bit * tmp = new Bit[bit_length * (alice_size + bob_size)]; //  bit array of inputs
    Bit *tmpPtr = tmp;
    Batcher alice_batcher, bob_batcher;
    
    for (int i = 0; i < alice_size*bit_length; ++i) {
        	   // set up bit array, if alice, secret share a local bit, 
        	   // otherwise bob collects his part of the secret share and inputs 0 as a placeholder
        	   alice_batcher.add<Bit>((ALICE==party) ? local_data[i]:0);
    }

	alice_batcher.make_semi_honest(ALICE);
		
	for(int i = 0; i < alice_size*bit_length; ++i) {
		*tmpPtr = alice_batcher.next<Bit>();
		++tmpPtr;
	}
	
    for (int i = 0; i < bob_size*bit_length; ++i)
        bob_batcher.add<Bit>((BOB==party) ? local_data[i]:0);
    	bob_batcher.make_semi_honest(BOB);

	// append all of bob's bits to tmp
    for (int i = 0; i < bob_size*bit_length; ++i) {
        *tmpPtr = bob_batcher.next<Bit>();
        ++tmpPtr;
	}
	
	tmpPtr = tmp;
	
	// create a 2D array of secret-shared bits
	// each index is a tuple
    for(int i = 0; i < alice_size + bob_size; ++i) {
        res[i] = Integer(bit_length, tmpPtr);
        tmpPtr += bit_length;
     }


    // TODO: sort if needed, not specific to col_length0
    bitonic_merge_sql(res, 0, alice_size + bob_size, Bit(true), 0, 64);
    Data * d = new Data;
    d->data = res;
    d->public_size = alice_size + bob_size;
    d->real_size = Integer(64, d->public_size, PUBLIC);
        
    return d;
}
// ordered union, multiset op, has multiset semantics
// inputs must be sorted by sort key from query plan in source DBs
// right now this presumes that the sort key is always the first column.
// need to make this more general by tweaking the parameters 


Data* SeqScan4Merge(int party, NetIO * io) {
	string sql = "SELECT patient_id FROM medications ORDER BY patient_id";
    std::vector<Row> in = execute_sql(sql, party);
    int bit_length = 64; // TODO: automatically derive size from Row metadata?
    // flatten out local data
    bool *local_data = concat(in, bit_length);

    int alice_size, bob_size;
    alice_size = bob_size = in.size();

    if (party == ALICE) {
        io->send_data(&alice_size, 4);
        io->flush();
        io->recv_data(&bob_size, 4);
        io->flush();
    } else if (party == BOB) {
        io->recv_data(&alice_size, 4);
        io->flush();
        io->send_data(&bob_size, 4);
        io->flush();
    }

	Integer * res = new Integer[alice_size + bob_size];  // enough space for all inputs
   
    Bit * tmp = new Bit[bit_length * (alice_size + bob_size)]; //  bit array of inputs
    Bit *tmpPtr = tmp;
    Batcher alice_batcher, bob_batcher;
    
    for (int i = 0; i < alice_size*bit_length; ++i) {
        	   // set up bit array, if alice, secret share a local bit, 
        	   // otherwise bob collects his part of the secret share and inputs 0 as a placeholder
        	   alice_batcher.add<Bit>((ALICE==party) ? local_data[i]:0);
    }

	alice_batcher.make_semi_honest(ALICE);
		
	for(int i = 0; i < alice_size*bit_length; ++i) {
		*tmpPtr = alice_batcher.next<Bit>();
		++tmpPtr;
	}
	
    for (int i = 0; i < bob_size*bit_length; ++i)
        bob_batcher.add<Bit>((BOB==party) ? local_data[i]:0);
    	bob_batcher.make_semi_honest(BOB);

	// append all of bob's bits to tmp
    for (int i = 0; i < bob_size*bit_length; ++i) {
        *tmpPtr = bob_batcher.next<Bit>();
        ++tmpPtr;
	}
	
	tmpPtr = tmp;
	
	// create a 2D array of secret-shared bits
	// each index is a tuple
    for(int i = 0; i < alice_size + bob_size; ++i) {
        res[i] = Integer(bit_length, tmpPtr);
        tmpPtr += bit_length;
     }


    // TODO: sort if needed, not specific to col_length0
    bitonic_merge_sql(res, 0, alice_size + bob_size, Bit(true), 0, 64);
    Data * d = new Data;
    d->data = res;
    d->public_size = alice_size + bob_size;
    d->real_size = Integer(64, d->public_size, PUBLIC);
        
    return d;
}
// TODO: generalize for arbitrary selection criteria
// see ObliVM generator for example of this
// this example is based on aspirin expected code
Data * Join6(Data *left, Data *right) {
    Integer *output = new Integer[left->public_size * right->public_size];
    int writeIdx = 0;
    Integer dstTuple, srcTuple, lTuple, rTuple;
    int jointSchemaSize = 64 + 64;
    srcTuple = Integer();
    srcTuple.resize(jointSchemaSize);
    
    dstTuple = Integer(64, 0, PUBLIC); // indicates dummy
    
    for (int i=0; i<left->public_size; i++) {
        for (int j=0; j<right->public_size; j++) {
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
    result->public_size = left->public_size * right->public_size;
	// real size needs to be maintained by counting the times the join criteria is met
	// this variable should probably not be public
    result->real_size = Integer(64, left->public_size, PUBLIC);
    return result;
}
// suffix for our emp ExecutionStep
// TODO: generalize this
// we fill in functions as we go along
// need to fill in NetIO setup based on contents of ConnectionManager
// expects as arguments party (1 = alice, 2 = bob) plus the port it will run the protocols over

int main(int argc, char** argv) {

    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO((party==ALICE ? aliceHost.c_str() : bobHost.c_str()), port);
    
    setup_semi_honest(io, party);
    
     Data *SeqScan0MergeOutput = SeqScan0Merge(party, io);

     Data *SeqScan4MergeOutput = SeqScan4Merge(party, io);

    Data * Join6Output = Join6(SeqScan0MergeOutput, SeqScan4MergeOutput);


    
    // TODO: decrypt Join6Output at honest broker
   
    io->flush();
    delete io;
}