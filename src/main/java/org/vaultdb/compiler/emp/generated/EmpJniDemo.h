#include <map>
#include <string>
#include <sstream>
#include <iostream>


using namespace std;




namespace EmpJniDemo {



class EmpJniDemoClass {


  string aliceHost = "127.0.0.1";
  string output;
  map<string, string> inputs; // maps opName --> bitString of input tuples

public:

	void run(int party, int port) {
	}


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

	int main(int argc, char** argv) {
		int party=0, port=0;
		run(party, port);
		return 0;
	}

}; // end class
} // end namespace
 
