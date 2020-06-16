#include <emp-sh2pc/emp-sh2pc.h>
#include <map>
#include <string>

#include "EmpUtilities.h"


// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;




namespace TpcHQuery1 {

class TpcHQuery1Class {

string aliceHost = "127.0.0.1";

string output;
map<string, string> inputs;    // maps opName --> bitString of input tuples



	class Data {
	public:
		Integer * tuples;
		int publicSize;

	};

public:
	// maintains a map of inputs from JDBC
	void addInput(const std::string& opName, const std::string& bitString) {

     inputs[opName] = bitString;

     }


	const std::string& getOutput() {
	      return output;
	}

Data* SeqScan0Union(int party, NetIO * io) {



    int rowLength = 273;


    std::cout << "The rowlength in bits is " << rowLength << std::endl;

    string localBitstring = inputs["SeqScan0Union"];
    bool *localData = EmpUtilities::toBool(localBitstring);

    int aliceSize = localBitstring.length() / rowLength;
    int bobSize = aliceSize;

    // communicate with other party to correct sizes ( number of tuples )
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

    // allocating secret share for union data & dummies
    Integer * res = new Integer[aliceSize + bobSize];  // enough space for all inputs
    Bit * tmp = new Bit[rowLength * (aliceSize + bobSize)]; //  bit representation of res



    bool *readPos = localData;

    int writePos = 0;
    int writeTuple = aliceSize; // last tuple

    // reverse Alice's order for correct Bitonic Merge ( Increasing -> Max -> Decreasing )
    for (int i = 0;  i < aliceSize; ++i) {
    	--writeTuple;
    	writePos = writeTuple * rowLength;


        for (int j = 0; j < rowLength; ++j) {
            // updating tmp pointer with alice's
            tmp[writeTuple*rowLength + j] = Bit((ALICE==party) ? *readPos : 0, ALICE);

            ++readPos;
            ++writePos;
         }
	}

	// Initialize Bob's Batcher
    Batcher batcher;



    for (int i = 0; i < bobSize*rowLength; ++i)
        batcher.add<Bit>((BOB==party) ? localData[i]:0);

    batcher.make_semi_honest(BOB); // Secret Share


    // add Bob's encrypted tuples
    for (int i = 0; i < bobSize*rowLength; ++i)
        tmp[i+aliceSize*rowLength] = batcher.next<Bit>();

    // create full Integer representation with both Bob and Alice's information
    for(int i = 0; i < aliceSize + bobSize; ++i) {
           
	   res[i] = Integer(rowLength, tmp + rowLength*i);
	   
	
	}

    cout << "SeqScan0Union took as input " << aliceSize + bobSize << " tuples from alice and bob." << endl;
    cout << "Tuple length "  << res[0].size() << endl;

    // TODO: make sort more robust.  Handle sort keys that are not adjacent or in the same order in the table
    EmpUtilities::bitonicMergeSql(res, 0, aliceSize + bobSize, Bit(true), 0, 273);

    Data * d = new Data;
    d->tuples = res;
    d->publicSize = aliceSize + bobSize;


    for(int i = 0; i < aliceSize + bobSize; ++i) { 
	   Integer value = Integer(res[i]);
	   value.bits[rowLength-1] = Bit(0, PUBLIC); // zero out the dummy tag



	  //cout << "Outputting " << res[i][rowLength - 1].reveal(PUBLIC) << ", " << value.reveal<int32_t>(PUBLIC) << endl;	  	  
    }
    return d;
}
Data * Aggregate4(QueryTable *t) {
    AggregateDef d;

    ScalarAggregateDef def1 = {2, vaultdb::AggregateId::SUM};
    ScalarAggregateDef def2 = {3, vaultdb::AggregateId::SUM};
    ScalarAggregateDef def3 = {4, vaultdb::AggregateId::SUM};
    ScalarAggregateDef def4 = {5, vaultdb::AggregateId::SUM};
    ScalarAggregateDef def5 = {6, vaultdb::AggregateId::AVG};
    ScalarAggregateDef def6 = {7, vaultdb::AggregateId::AVG};
    ScalarAggregateDef def7 = {8, vaultdb::AggregateId::AVG};
    ScalarAggregateDef def8 = {9, vaultdb::AggregateId::COUNT};
    d.defs.push_back(def1);
    d.defs.push_back(def2);
    d.defs.push_back(def3);
    d.defs.push_back(def4);
    d.defs.push_back(def5);
    d.defs.push_back(def6);
    d.defs.push_back(def7);
    d.defs.push_back(def8);
    d.gb_ordinals.push_back(0);
    d.gb_ordinals.push_back(1);


    return Aggregate(t, d);
}


// expects as arguments party (1 = alice, 2 = bob) plus the port it will run the protocols over

public:
void setGeneratorHost(string host) {
     aliceHost = host;
}


void run(int party, int port) {
	
	std::cout << "starting run in emp! party=" << party << " port=" << port <<  std::endl;
	NetIO * io = new NetIO((party==ALICE ? nullptr : aliceHost.c_str()), port);
    
    setup_semi_honest(io, party);
    
         Data *SeqScan0UnionOutput = SeqScan0Union(party, io);

    Data * Aggregate4Output = Aggregate4(SeqScan0UnionOutput);


    
    Data * results = Aggregate4Output;

    int tupleWidth = results->tuples[0].size();

    std::cout << "Final output tuple width is " << tupleWidth << std::endl;


    long outputSize = results->publicSize * tupleWidth;
    output.reserve(outputSize);
    bool *tuple;



    for(int i = 0; i < results->publicSize; ++i) {
         tuple = EmpUtilities::outputBits(results->tuples[i], tupleWidth, XOR);
         for(int j = 0; j < tupleWidth; ++j) {
                 output += (tuple[j] ? '1' : '0');
             }
    }


	io->flush();
	delete io;

	cout << "Returning " << outputSize  << " bits." << endl;
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
 