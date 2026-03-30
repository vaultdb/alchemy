#include <emp-sh2pc/emp-sh2pc.h>
#include <map>
#include <string>

#include "EmpUtilities.h"


// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;




namespace CountIcd9sCodegen {

class CountIcd9sCodegenClass {

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



    int rowLength = 33;


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
    EmpUtilities::bitonicMergeSql(res, 0, aliceSize + bobSize, Bit(true), 0, 33);

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
Data * Distinct2(Data *data) {
	// sort step needs to always put dummies last and then sort by payload
	
	
	

	int tupleLen = data->tuples[0].size();
	int dummyIdx = tupleLen - 1;
	int payloadBits = tupleLen - 1;
	cout << " Running Distinct2 with " << data->publicSize << " inputs, tupleLen=" << tupleLen << endl;


    Integer lastPayload = Integer(payloadBits, data->tuples[0].bits);
    Integer payload;
    int cursor = 0;



	cout << "Distinct outputting (" << data->tuples[cursor][dummyIdx].reveal(PUBLIC) << ", " << lastPayload.reveal<uint32_t>(PUBLIC) << ")" <<  endl;
    
    while ( cursor < data->publicSize - 1) {


        cursor++;
	
        payload =  Integer(payloadBits, data->tuples[cursor].bits);

        Bit isDummy = data->tuples[cursor][dummyIdx]; // easier to perform logic checks
	    Bit payloadsEqual = (payload == lastPayload);
	  
 
         // if it is real and != to predecessor
	    Bit setDummy = isDummy | payloadsEqual; // set to dummy if it equals its predecessor
	    //cout << "Payload == lastPayload: " << payloadsEqual.reveal(PUBLIC) << endl;
	    //std::cout << "Comparison " << payload.reveal<int32_t>(PUBLIC) << " vs " << lastPayload.reveal<int32_t>(PUBLIC) << " dummy tag: " << isDummy.reveal(PUBLIC) << " setting dummy? " << setDummy.reveal(PUBLIC) << std::endl;
	     
        data->tuples[cursor][dummyIdx] = setDummy; 
		cout << "Distinct outputting (" << data->tuples[cursor][dummyIdx].reveal(PUBLIC) << ", " << payload.reveal<uint32_t>(PUBLIC) << ")" <<  endl;
        lastPayload = payload;
    }




    return data;
}

Data * Aggregate3(Data *data) {


    Data *result = new Data;
    result->tuples = new Integer[1];
    result->publicSize = 1; 

    int dummyIdx = 33 - 1;
    Integer *output = new Integer(65, 0, PUBLIC);


    cout << "Running Aggregate3 " << endl;

   Integer agg1= Integer(64,0,PUBLIC);
Integer tupleArg1 = Integer(64,0, PUBLIC);


  for(int cursor = 0; cursor < data->publicSize; cursor++){
        Integer tuple = data->tuples[cursor];
        Bit dummyCheck = tuple[dummyIdx];

	   EmpUtilities::writeToInteger( &tupleArg1, &tuple, 0, 0, 64);
agg1 = If(dummyCheck, agg1 + Integer(64, 1, PUBLIC), agg1);


    }

    EmpUtilities::writeToInteger(output, &agg1, 0, 0, 64);


    result->tuples = output;

    return result;
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

    Data * Distinct2Output = Distinct2(SeqScan0UnionOutput);

    Data * Aggregate3Output = Aggregate3(Distinct2Output);


    
    Data * results = Aggregate3Output;

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
 