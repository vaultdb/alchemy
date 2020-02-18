#include <emp-sh2pc/emp-sh2pc.h>
#include "query_table/query_table.h"
#include <map>

using namespace emp;
using namespace std;




namespace EmpJniDemo {



class EmpJniDemoClass {


  string aliceHost = "127.0.0.1";
  void* output;
  map<string, Querytable> inputs; // maps opName --> C++ query table representation



  // Operator functions
  class Data {
  public:
      Integer * tuples;
      int publicSize;
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




unique_ptr<QueryTable> unionOp(int party, NetIO * io) {



    string firstKey = inputs.begin()->first;
    QueryTable localData = inputs[firstKey];
    int rowLength = localData.; // 3 chars = 24 bits


    ShareDef def;
    int aliceSize, bobSize;

    aliceSize = bobSize = localData.GetNumTuples();

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


    EmpParty my_party =
         party == emp::ALICE ? EmpParty::ALICE : EmpParty::BOB;
     ShareDef def;
     ShareCount ca = {.party = EmpParty::ALICE};
     ShareCount cb = {.party = EmpParty::BOB};
     ca.num_tuples = aliceSize;
     cb.num_tuples = bobSize;

     def.share_map[EmpParty::ALICE] = ca;
     def.share_map[EmpParty::BOB] = cb;

     auto s_table =
           ShareData(localData->GetSchema(), my_party, localData.get(), def);
     return s_table;
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
     unique_ptr<QueryTable> results = unionOp(party, io);

     QueryTable xorRevealed = results.revealXOR();
     void *output = toBytes(xorRevealed);
     io->flush();
     delete io;

     cout << "Completed emp program!" << endl;


}

void setGeneratorHost(string host) {
	aliceHost = host;
}


// placeholder for maintaining a map of inputs from JDBC
void addInput(const std::string& opName, const void & bitString) {

	inputs[opName] = ProtoToQueryTable(bitString);

	}


const void & getOutput() {
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


