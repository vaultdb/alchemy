#include <emp-sh2pc/emp-sh2pc.h>
#include <map>
#include <string>

// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;

#define LENGTH_INT 64

#define OID_INT 20



namespace Count {

class CountClass {

	string output;
	map<string, string> inputs; // maps opName --> bitString of input tuples


	// Connection strings, encapsulates db name, db user, port, host
	string aliceConnectionString = "dbname=smcql_testdb_site1 user=smcql host=localhost port=5432";
	string bobConnectionString = "dbname=smcql_testdb_site2 user=smcql host=localhost port=5432";
	string aliceHost = "127.0.0.1";

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

	bool * outputBits(Integer &input, int length, int output_party) {
		bool * b = new bool[length];
		ProtocolExecution::prot_exec->reveal(b, output_party, (block *)input.bits,  length);
		string bin="";

		return b;
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


	int sum_vals(vector<int> vec) {
		int sum = 0;
		for (int i : vec)
			sum += i;
		return sum;
	}


	// Operator functions
	class Data {
	public:
		Integer * data;
		int public_size;
		Integer real_size;
		Integer dummyTags;
	};

	Integer from_bool(bool* b, int size, int party) {
		Integer res;
		res.bits = new Bit[size];
		init(res.bits, b, size, party);
		return res;
	}







	bool * toBool(string src) {
		long length = src.length();
		bool *output = new bool[length];

		for(int i = 0; i < length; ++i)
			output[i] = (src[i] == '1') ? true : false;

		return output;
	}


// ordered union, multiset op, has multiset semantics
// inputs must be sorted by sort key from query plan in source DBs
// right now this presumes that the sort key is always the first column.
// need to make this more general by tweaking the parameters


	Data* Distinct2Merge(int party, NetIO * io) {
		string inputStr = inputs["Distinct2Merge"];
		bool *local_data = toBool(inputStr);
		int bit_length = 32;

		int alice_size, bob_size;
		alice_size = bob_size = inputStr.length() / bit_length;

		if (party == ALICE) {
			io->send_data(&alice_size, 4);
			io->flush();
			io->recv_data(&bob_size, 4);
			io->flush();
		} else if (party == BOB) {
			io->recv_data(&alice_size, 4);
			io->flush();
			io->send_data(&bob_size, 4);
			io->flush();
		}

		Integer * res = new Integer[alice_size + bob_size];  // enough space for all inputs

		Bit * tmp = new Bit[bit_length * (alice_size + bob_size)]; //  bit array of inputs
		Bit *tmpPtr = tmp;
		Batcher alice_batcher, bob_batcher;

		for (int i = 0; i < alice_size*bit_length; ++i) {
				   // set up bit array, if alice, secret share a local bit,
				   // otherwise bob collects his part of the secret share and inputs 0 as a placeholder
				   alice_batcher.add<Bit>((ALICE==party) ? local_data[i]:0);
		}

		alice_batcher.make_semi_honest(ALICE);

		for(int i = 0; i < alice_size*bit_length; ++i) {
			*tmpPtr = alice_batcher.next<Bit>();
			++tmpPtr;
		}

		for (int i = 0; i < bob_size*bit_length; ++i)
			bob_batcher.add<Bit>((BOB==party) ? local_data[i]:0);
			bob_batcher.make_semi_honest(BOB);

		// append all of bob's bits to tmp
		for (int i = 0; i < bob_size*bit_length; ++i) {
			*tmpPtr = bob_batcher.next<Bit>();
			++tmpPtr;
		}

		tmpPtr = tmp;

		// create a 2D array of secret-shared bits
		// each index is a tuple
		for(int i = 0; i < alice_size + bob_size; ++i) {
			res[i] = Integer(bit_length, tmpPtr);
			tmpPtr += bit_length;
		 }


		// TODO: sort if needed, not specific to col_length0
		bitonic_merge_sql(res, 0, alice_size + bob_size, Bit(true), 0, 256);
		Data * d = new Data;
		d->data = res;
		d->public_size = alice_size + bob_size;
		d->real_size = Integer(64, d->public_size, PUBLIC);

		return d;
	}

	Data * Distinct2(Data *data) {


		int tupleLen = data->data[0].size() * sizeof(Bit);

		for (int i=0; i< data->public_size - 1; i++) {
			Integer id1 = data->data[i];
			Integer id2 = data->data[i+1];
			Bit eq = (id1 == id2);
			id1 = If(eq, Integer(tupleLen, 0, PUBLIC), id1);
			//maintain real size
			data->real_size = If(eq, data->real_size - Integer(LENGTH_INT, 1, PUBLIC),  data->real_size);
			memcpy(data->data[i].bits, id1.bits, tupleLen);
		}

		return data;
	}

	Data * Aggregate3(Data *data) {
		data->public_size = 1;
		data->data = new Integer[1];
		data->data[0] = data->real_size;
		data->real_size = Integer(LENGTH_INT, 1, PUBLIC);
		return data;
	}

// suffix for our emp ExecutionStep
// TODO: generalize this
// we fill in functions as we go along
// need to fill in NetIO setup based on contents of ConnectionManager
// expects as arguments party (1 = alice, 2 = bob) plus the port it will run the protocols over

public:

	 void setGeneratorHost(string& generator) {
		aliceHost = generator;
	}

	const std::string& getOutput() {
		      return output;
		}


	void run(int party, int port) {

		NetIO * io = new NetIO((party==ALICE ? nullptr : aliceHost.c_str()), port);

		setup_semi_honest(io, party);

		 Data *Distinct2MergeOutput = Distinct2Merge(party, io);

		Data * Distinct2Output = Distinct2(Distinct2MergeOutput);

		Data * Aggregate3Output = Aggregate3(Distinct2Output);
		io->flush();
		delete io;

		Data * results = Aggregate3Output;

		int tupleLen = results->data[0].size();

		long outputLength = results->public_size * tupleLen;
		output.reserve(outputLength);

		bool *tuple;
		for(int i = 0; i < results->public_size; ++i) {
			tuple = outputBits(results->data[i], tupleLen, XOR);
			for(int j = 0; j < tupleLen; ++j) {
				output += (tuple[j] == true) ? '1' : '0';
			}
		}

		
		

	}

	void addInput(const std::string& opName, const std::string& bitString) {

	     inputs[opName] = bitString;

	     }



};
}
