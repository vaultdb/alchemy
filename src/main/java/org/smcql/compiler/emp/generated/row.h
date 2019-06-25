#ifndef ROW_H__
#define ROW_H__

using namespace emp;
using namespace std;

string bool_to_binstr(bool *b, int size) {
        string result = "";
        for (int i=0; i<size; i++)
                result = result + ((b[i]) ? "1" : "0");
        return result;
}

string binstr_to_str(string bin) {
        string result = "";
        int charCount = bin.size() / 8;
        int readIdx = 0;
        std::bitset<8> bits;

        for(int i = 0; i < charCount; ++i) {
        	String bits = charCount.substr(readIdx, 8);
        	// TODO: finish this
            sstream >> bits;
            char c = char(bits.to_ulong());
            result += c;

        }
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

string int64_to_binstr(int64_t val) {
	string result = std::bitset<64>(val).to_string();
        std::reverse(result.begin(), result.end());
	return result;
}

string str_to_binary(string str, unsigned int num_bits) {
        string binary = "";
        for (unsigned int i=0; i<num_bits/8; i++) {
                string next = (i < str.length()) ? bitset<8>(str.c_str()[i]).to_string() : "00000000";
                binary += next;
        }

        return binary;
}

class Row {
	public:
        string data;
	vector<int> lengths;

	Row(string d, vector<int> l) {
		data = d;
		lengths = l;
	}

       	#include "row.hpp"
};

#endif
