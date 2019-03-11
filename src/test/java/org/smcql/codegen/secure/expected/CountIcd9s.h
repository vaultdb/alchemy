#include <emp-sh2pc/emp-sh2pc.h>
#include <map>
#include <string>


// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;




namespace CountIcd9s {

class CountIcd9sClass {
// Connection strings, encapsulates db name, db user, port, host
string aliceConnectionString = "dbname=smcql_testdb_site1 user=smcql host=localhost port=5432";
string bobConnectionString = "dbname=smcql_testdb_site2 user=smcql host=localhost port=5432";

string aliceHost = "127.0.0.1";
int INT_LENGTH = 64;
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



bool * outputBits(Integer &input, int length, int output_party) {
		bool * b = new bool[length];
		ProtocolExecution::prot_exec->reveal(b, output_party, (block *)input.bits,  length);
		string bin="";

		return b;
	}


bool * toBool(string src) {
	long length = src.length();
	bool *output = new bool[length];
	
	for(int i = 0; i < length; ++i) 
		output[i] = (src[i] == '1') ? true : false;
	
	return output;
}

// Operator functions
class Data {
public:
    Integer * data;
    int publicSize;
    Integer realSize;
    Integer dummyTags; // TODO
};



Data* SeqScan0Merge(int party, NetIO * io) {
    int rowLength = 524288; 
	string localBitstring = inputs["SeqScan0Merge"];
    bool *localData = toBool(localBitstring);


    int aliceSize = localBitstring.length() / rowLength;
    int bobSize = aliceSize;

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

    cout << "SeqScan0Merge took as input " << aliceSize + bobSize << " tuples." << endl;

    // TODO: make sort more robust.  Handle sort keys that are not adjacent or in the same order in the table
    bitonic_merge_sql(res, 0, aliceSize + bobSize, Bit(true), 0, 524288);

    Data * d = new Data;
    d->data = res;
    d->publicSize = aliceSize + bobSize;
    d->realSize = Integer(64, d->publicSize, PUBLIC);
    cout << "Done union!" << endl;   
    return d;
}
Data * Distinct2(Data *data) {
	
	
	cout << " Running Distinct2 with " << data->publicSize << " inputs." << endl;
	int tupleLen = data->data[0].size() * sizeof(Bit);
	
    for (int i=0; i< data->publicSize - 1; i++) {
        Integer id1 = data->data[i];
        Integer id2 = data->data[i+1];
        Bit eq = (id1 == id2);
        id1 = If(eq, Integer(tupleLen, 0, PUBLIC), id1);
        
        //cout << "Equality check for " << i << " is " << eq.reveal(PUBLIC) << " for " << id1.reveal<int64_t>(PUBLIC) << " and " << id2.reveal<int64_t>(PUBLIC) << endl;

        //maintain real size
        Integer decremented = data->realSize - Integer(INT_LENGTH, 1, PUBLIC);
  	    data->realSize = If(eq, decremented,  data->realSize);
         
        if(eq.reveal(PUBLIC)) {
        	cout << "Decrementing!" << endl;
        }
        
  	    memcpy(data->data[i].bits, id1.bits, tupleLen);       
    }
    
    return data;
}

Data * Aggregate3(Data *data) {

	cout << "Running Aggregate3 " << endl;
	data->publicSize = 1;
	data->data = new Integer[1];
	data->data[0] = data->realSize;
	data->realSize = Integer(INT_LENGTH, 1, PUBLIC);
    return data;
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

    Data * Distinct2Output = Distinct2(SeqScan0MergeOutput);

    Data * Aggregate3Output = Aggregate3(Distinct2Output);


    
    	Data * results = Aggregate3Output;

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
 