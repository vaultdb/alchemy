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

	// 0 is LSB
   static Integer prepKey(Integer src) {

	   int srcLength = src.size();
	   Integer dst = Integer(src);
	   dst.resize(srcLength + 1);

	   //dst = dst >> 1;
	   //dst.bits[srcLength - 1] = src.bits[0];

	   return dst;


   }

  static void cmpSwapSql(Integer* tuples, int i, int j, Bit acc, int keyPos, int keyLength) {
		   Integer lhs = Integer(keyLength, tuples[i].bits + keyPos);
		   Integer rhs = Integer(keyLength, tuples[j].bits + keyPos);

		   lhs = lhs.resize(keyLength + 1); // otherwise dummyTag gets interpreted as sign bit
		   rhs = rhs.resize(keyLength + 1);


		   Bit toSwap = ((lhs > rhs) == acc);

		   cout << "Comparing idx: " << i << ": " << tuples[i].bits[0].reveal(PUBLIC) << ", " << Integer(32, tuples[i].bits + 1).reveal<int32_t>(PUBLIC) << ", "
				   << " to idx: " << j << ": " << tuples[j].bits[0].reveal(PUBLIC) << ", " << Integer(32, tuples[j].bits + 1).reveal<int32_t>(PUBLIC) << " swapping? " << toSwap.reveal(PUBLIC) << endl;
		   cout << "Keys: " << lhs.reveal<int64_t>(PUBLIC) << ", " << rhs.reveal<int64_t>(PUBLIC) << endl;

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



};
