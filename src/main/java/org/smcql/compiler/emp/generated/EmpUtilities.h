#include <emp-sh2pc/emp-sh2pc.h>
#include <map>
#include <string>

using namespace emp;
using namespace std;


class EmpUtilities {


	// Helper functions
public:
	static string revealBinary(Integer &input, int length, int output_party) {
		bool * b = new bool[length];
		ProtocolExecution::prot_exec->reveal(b, output_party, (block *)input.bits,  length);
		char *bin = new char[length];

		for (int i=0; i<length; i++)
			bin[i] = (b[i] ? '1':'0');

		delete [] b;
		return string(bin);
		//return bin;
	}



	// dummy tag is at most significant bit (at the end)
		// this means that the comparator mistakens it for a sign bit
		// producing incorrect results.
		// so we pad it with another bit to make the sign bit a neutral


  static void cmpSwapSql(Integer* tuples, int i, int j, Bit acc, int keyPos, int keyLength) {
		   Integer lhs = Integer(keyLength, tuples[i].bits + keyPos);
		   Integer rhs = Integer(keyLength, tuples[j].bits + keyPos);

		   lhs = lhs.resize(keyLength + 1, 0); // otherwise dummyTag gets interpreted as sign bit
		   rhs = rhs.resize(keyLength + 1, 0);


		   Bit toSwap = ((lhs > rhs) == acc);

		   swap(toSwap, tuples[i], tuples[j]);
	}


// TODO: extend this to multiple columns as a list of keyPos and keyLength
	static void bitonicMergeSql(Integer* tuples, int lo, int n, Bit acc, int keyPos, int keyLength) {
		if (n > 1) {
			int m = greatestPowerOfTwoLessThan(n);
			for (int i = lo; i < lo + n - m; i++)
				cmpSwapSql(tuples, i, i + m, acc, keyPos, keyLength);

			bitonicMergeSql(tuples, lo, m, acc, keyPos, keyLength);
			bitonicMergeSql(tuples, lo + m, n - m, acc, keyPos, keyLength);
		}
	}

	static void bitonicSortSql(Integer * key, int lo, int n, Bit acc,  int keyPos, int keyLength) {
		if (n > 1) {
			int m = n / 2;
			bitonicSortSql(key, lo, m, !acc, keyPos, keyLength);
			bitonicSortSql(key, lo + m, n - m, acc, keyPos, keyLength);
			bitonicMergeSql(key, lo, n, acc, keyPos, keyLength);
		}
	}



	static bool * outputBits(Integer &input, int length, int output_party) {
		bool * b = new bool[length];
		ProtocolExecution::prot_exec->reveal(b, output_party, (block *)input.bits,  length);


		return b;
	}



	static bool * toBool(string src) {
		long length = src.length();
		bool *output = new bool[length];

		for(int i = 0; i < length; ++i)
			output[i] = (src[i] == '1') ? true : false;

		return output;
	}

	static void writeToInteger(Integer *dst, Integer *src, int writeOffset, int readOffset, int size) {
		Bit *writePtr = dst->bits + writeOffset;
		Bit *srcPtr = src->bits + readOffset;

		memcpy(writePtr, srcPtr, sizeof(Bit)*size);

	}


	static Bit getDummyTag(Integer tuple) {
		int dummyIdx = tuple.size() - 1;
		return tuple[dummyIdx];
	}


	static Integer readFromInteger(Integer src, int readOffset, int size) {
		return Integer(size, src.bits + readOffset);
	}


};
