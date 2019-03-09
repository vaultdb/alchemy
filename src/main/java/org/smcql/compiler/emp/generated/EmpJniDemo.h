#include <emp-sh2pc/emp-sh2pc.h>
#include <map>

using namespace emp;
using namespace std;




namespace EmpJniDemo {



class EmpJniDemoClass {


  string aliceHost = "127.0.0.1";
  string output;
  map<string, string> inputs; // maps opName --> bitString of input tuples



  // Operator functions
  class Data {
  public:
      Integer * data;
      int publicSize;
      Integer realSize;
      Integer dummyTag;
  };

  Integer fromBool(bool* b, int size, int party) {
      Integer res;
      res.bits = new Bit[size];
      init(res.bits, b, size, party);
      return res;
  }

bool * outputBits(Integer &input, int length, int output_party) {
	bool * b = new bool[length];
	block *bits =  (block *) input.bits;

	ProtocolExecution::prot_exec->reveal(b, output_party, bits,  length);
	return b;
}



string reveal_bin(Integer &input, int length, int output_party) {
	bool * b = new bool[length];
	ProtocolExecution::prot_exec->reveal(b, output_party, (block *)input.bits,  length);
	string bin="";

	for (int i=0; i<length; i++)
		bin += (b[i] ? '1':'0');

	delete [] b;
	return bin;
}



string binstr_to_str(string bin) {
        string result = "";

        std::stringstream sstream(bin);
        while(sstream.good())
        {
                std::bitset<8> bits;
                sstream >> bits;
                char c = char(bits.to_ulong());
                result += c;
        }

        return result;
}







bool *toBool(string str) {
    string boolStr = str_to_binary(str, str.length() * 8); // to bits

	int l = boolStr.length();
	bool *result = new bool[l];
	const char *tmp = boolStr.c_str();

	for (int i=0; i<l; i++)
		result[i] = (tmp[i] == '0') ? false : true;

	return result;
}

bool * setUpLocalData() {
	int rowLength = 24; // 3 chars
	bool *localData = new bool[2*rowLength];

	// input same for both alice and bob
	string v1 = "008";
	bool* b1 = toBool(v1);
	memcpy(localData, b1, rowLength);

	string v2 = "414";
	bool* b2 = toBool(v2);
	memcpy(localData + rowLength, b2, rowLength);
	return localData;
}


Data* unionOp(int party, NetIO * io) {

    int rowLength = 24; // 3 chars = 24 bits
    bool * localData = setUpLocalData();

    int aliceSize, bobSize;
    aliceSize = bobSize = 2;

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

    	for(int i = 0; i < aliceSize + bobSize; ++i) {
    		string decrypted = reveal_bin(res[0], rowLength, PUBLIC);
    		if(party == ALICE) {
    			// reverse for endian-ness
    			std::reverse(decrypted.begin(), decrypted.end());
    			string plaintext = binstr_to_str(decrypted);
    		}

    }

    Data * d = new Data;
    d->data = res;
    d->publicSize = aliceSize + bobSize;
    d->realSize = Integer(64, d->publicSize, PUBLIC);
    return d;

}

string str_to_binary(string str, unsigned int num_bits) {
        string binary = "";
        for (unsigned int i=0; i<num_bits/8; i++) {
                string next = (i < str.length()) ? bitset<8>(str.c_str()[i]).to_string() : "00000000";
                binary += next;
        }

        return binary;
}


// expects as arguments party (1 = alice, 2 = bob) plus the port it will run the protocols over

public:

	void run(int party, int port) {
	
	std::cout << "starting run in emp! party=" << party << " port=" << port <<  " alice host=" << aliceHost <<  std::endl;
	NetIO * io = new NetIO((party==ALICE ? nullptr : aliceHost.c_str()), port);
    
    setup_semi_honest(io, party);

    cout << "Calling union op!" << endl;
     Data *results = unionOp(party, io);


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

     cout << "Completed emp program!" << endl;

		
}

// placeholder for maintaining a map of inputs from JDBC
void addInput(const std::string& opName, const std::string& bitString) {

	inputs[opName] = bitString;

	}


const std::string& getOutput() {
	return output;
}

int main(int argc, char** argv) { 
	int party, port;
	parse_party_and_port(argv, 2, &party, &port);
	run(party, port);
	return 0;
}
}; // end class
} // end namespace
 
