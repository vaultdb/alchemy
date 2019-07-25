#include <emp-sh2pc/emp-sh2pc.h>
#include <map>
#include <string>

#include "EmpUtilities.h"


// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;




namespace JoinCdiff {

class JoinCdiffClass {

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



	  cout << "Outputting " << res[i][rowLength - 1].reveal(PUBLIC) << ", " << value.reveal<int32_t>(PUBLIC) << endl;	  	  
    }
    return d;
}
Data* SeqScan4Union(int party, NetIO * io) {



    int rowLength = 33;


    std::cout << "The rowlength in bits is " << rowLength << std::endl;

    string localBitstring = inputs["SeqScan4Union"];
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

    cout << "SeqScan4Union took as input " << aliceSize + bobSize << " tuples from alice and bob." << endl;
    cout << "Tuple length "  << res[0].size() << endl;

    // TODO: make sort more robust.  Handle sort keys that are not adjacent or in the same order in the table
    EmpUtilities::bitonicMergeSql(res, 0, aliceSize + bobSize, Bit(true), 0, 33);

    Data * d = new Data;
    d->tuples = res;
    d->publicSize = aliceSize + bobSize;


    for(int i = 0; i < aliceSize + bobSize; ++i) { 
	   Integer value = Integer(res[i]);
	   value.bits[rowLength-1] = Bit(0, PUBLIC); // zero out the dummy tag



	  cout << "Outputting " << res[i][rowLength - 1].reveal(PUBLIC) << ", " << value.reveal<int32_t>(PUBLIC) << endl;	  	  
    }
    return d;
}
// TODO: generalize for arbitrary selection criteria
// see ObliVM generator for example of this
// this example is based on aspirin expected code
Data * Join6(Data *left, Data *right) {
 
    int writeIdx = 0;
    Integer dstTuple, srcTuple, lTuple, rTuple;
    int jointSchemaSize = 32 + 32 - 1; // don't double-count dummy tags from two contributing tuples
    int outputTupleCount = left->publicSize * right->publicSize;
	int lhsPayloadSize = 32 - 1; // sans dummy tag
	int rhsPayloadSize = 32 - 1; // sans dummy tag
	
    

	Data *result = new Data;
    
	srcTuple = Integer(jointSchemaSize, 0, PUBLIC);
    

    // output tuple count
    result->tuples = new Integer[outputTupleCount];
    result->publicSize = outputTupleCount;
    
    dstTuple = Integer(32, 0, PUBLIC);
	
    for (int i=0; i < left->publicSize; i++) {
        for (int j=0; j < right->publicSize; j++) {
        	lTuple = left->tuples[i];
        	rTuple = right->tuples[j];
	        	
        	// concatenate the two inputs into srcTuple, the standard join output schema
        	memcpy(srcTuple.bits, lTuple.bits, lhsPayloadSize);
        	memcpy(srcTuple.bits + 32, rTuple.bits, rhsPayloadSize);
        	
        	// TODO: rewire filter flattener to skip the memcopies above
        	Bit cmp = Integer(32, srcTuple.bits ) == Integer(32, srcTuple.bits  + 32);
        	
        	// populate dstTuple with any projections
        	memcpy(dstTuple.bits , Integer(32, srcTuple.bits ).bits, 32);
;
        	
        	dstTuple[dstTuple.size() - 1] = cmp & !EmpUtilities::getDummyTag(lTuple) & !EmpUtilities::getDummyTag(rTuple);
	       
        	
	        result->tuples[writeIdx] = dstTuple;
            writeIdx++;
            
        }
    }
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

     Data *SeqScan4UnionOutput = SeqScan4Union(party, io);

    Data * Join6Output = Join6(SeqScan0UnionOutput, SeqScan4UnionOutput);


    
    Data * results = Join6Output;

    int tupleWidth = results->tuples[0].size();

    // Debugging assistance
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
 