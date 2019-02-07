#include <emp-sh2pc/emp-sh2pc.h>
#include <pqxx/pqxx>
#include <utils/row.h>

// header of an ExecutionSegment in emp
// put in front of first generated MPC operator


using namespace emp;
using namespace std;
using namespace pqxx;

#define LENGTH_INT 64

// Connection strings, encapsulates db name, db user, port, host
string aliceConnectionString = "dbname=smcql_testdb_site1 user=smcql host=localhost port=5432";
string bobConnectionString = "dbname=smcql_testdb_site2 user=smcql host=localhost port=5432";
string aliceHost = "localhost";
string bobHost = "localhost";




std::vector<String> queries;
// Plaintext queries
// $srcSQL

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

void cmp_swap_sql(Integer*key, int i, int j, Bit acc, int key_pos, int key_length) {
    Integer keyi = Integer(key_length, key[i].bits+key_pos);
    Integer keyj = Integer(key_length, key[j].bits+key_pos);
    Bit to_swap = ((keyi > keyj) == acc);
    swap(to_swap, key[i], key[j]);
}

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

bool *concat(std::vector<Row> rows, int row_size) {
    bool *result = new bool[row_size*rows.size()];
    int num_rows = rows.size();

    for (int i=0; i<num_rows; i++) {
        memcpy(result + i*row_size, rows[i].to_bool(), row_size);   
    }
    return result;
}

int sum_vals(vector<int> vec) {
    int sum = 0;
    for (int i : vec)
        sum += i;
    return sum;
}

// DB connection functions
std::vector<Row> execute_sql(string sql, int party) {
    std::vector<Row> res;
    try {
        string config= (party == ALICE) ? aliceConnectionString : bobConnectionString;
        connection C(config);

        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
            work w(C);
            result r = w.exec(sql);
            w.commit();

            for (auto row : r) {
            	const int num_cols = row.size();
            	vector<int>lengths;
            	string bin_str = "";
            	for (int j=0; j<num_cols; j++) {
            	    const pqxx::field field = row[j];
            	    int oid = field.type();
            	   /* if (oid == OID_STRING) { // TODO: TODO: what happens if we have two string fields in the same query? 
            	        lengths.push_back(LENGTH_STRING);  //   implement this using pqxx::binary_string size methods
            	                                          // May need to parameterize schema, e.g., define column icd9 as varchar(7) 
            	        bin_str += str_to_binary(row[j].as<string>(), LENGTH_STRING);
            	    } else*/
            	   if (oid == OID_INT) {
            	        lengths.push_back(LENGTH_INT);
            	        bin_str += int64_to_binstr(row[j].as<int64_t>());
            	    } else {
            	        throw "Unsupported data type in column";
            	    }
            	}
            	res.push_back(Row(bin_str, lengths));
            }
        } else {
            cout << "Can't open database" << endl;
        }
        C.disconnect();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }

    return res;
}

// Operator functions
class Data {
public:
    Integer * data;
    int public_size;
    Integer real_size;
};

Integer from_bool(bool* b, int size, int party) {
    Integer res;
    res.bits = new Bit[size];
    init(res.bits, b, size, party);
    return res;
}


// ordered union, multiset op, has multiset semantics
// inputs must be sorted by sort key from query plan in source DBs
// right now this presumes that the sort key is always the first column.
// need to make this more general by tweaking the parameters 


Data* Distinct2Merge(int party, NetIO * io) {
	string sql = "SELECT DISTINCT icd9 FROM diagnoses ORDER BY icd9";
    std::vector<Row> in = execute_sql(sql, party);
    int bit_length = 256; // TODO: automatically derive size from Row metadata?
    // flatten out local data
    bool *local_data = concat(in, bit_length);

    int alice_size, bob_size;
    alice_size = bob_size = in.size();

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
    
    for (int i = 0; i < alice_size*bit_length; ++i) {
        	   // set up bit array, if alice, secret share a local bit, 
        	   // otherwise bob collects his part of the secret share and inputs 0 as a placeholder
        	   alice_batcher.add<Bit>((ALICE==party) ? local_data[i]:0);
    }

	alice_batcher.make_semi_honest(ALICE);
		
	for(int i = 0; i < alice.size*bit_length; ++i) {
		*tmpPtr = batcher.next<Bit>();
		++tmpPtr;
	}
	
    for (int i = 0; i < bob_size*bit_length; ++i)
        bob_batcher.add<Bit>((BOB==party) ? local_data[i]:0);
    bob_batcher.make_semi_honest(BOB);

	// append all of bob's bits to tmp
    for (int i = 0; i < bob_size*bit_length; ++i) {
        *tmpPtr = batcher.next<Bit>();
        ++tmpPtr;
	}
	
	tmpPtr = tmp;
	// create a 2D array of secret-shared bits
	// each index is a tuple
    for(int i = 0; i < alice_size + bob_size; ++i) {
        res[i] = Integer(bit_length, tmpPtr);
        tmpPtr += bitLength;
     }


    bitonic_merge_sql(res, 0, alice_size + bob_size, Bit(true), 0, col_length0);
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
  	    data->real_size = If(eq, data->real_size, data->real_size - 1);
        memcpy(data->data[i],id1, tupleLen);       
    }
    
    return data;
}

Integer Aggregate3(Data *data) {
    return data->real_size;
}

// suffix for our emp ExecutionStep
// TODO: generalize this
// we fill in functions as we go along
// need to fill in NetIO setup based on contents of ConnectionManager
// expects as arguments party (1 = alice, 2 = bob) plus the port it will run the protocols over

int main(int argc, char** argv) {
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? aliceHost : bobHost, port);

    setup_semi_honest(io, party);
    
     Data *Distinct2MergeOutput = Distinct2Merge(party, io);

    Data * Distinct2Output = Distinct2(Distinct2MergeOutput);

    Data * Aggregate3Output = Aggregate3(Distinct2Output);


    
    // TODO: decrypt Aggregate3Output at honest broker
   
    io->flush();
    for (int i=0; i<res_0->public_size; i++) {
        if (i==0 && party == BOB)
            cout << "\nOutput:" << endl;

        string val = reveal_bin(res_0->data[i], initial_row_size, PUBLIC);
        string col0 = val.substr(0, col_length0);
        reverse(col0.begin(), col0.end());
        string col1 = val.substr(col_length0, col_length0 + col_length1);
        
        vector<int> lengths{col_length0, col_length1};
        Row row = Row(col0 + col1, lengths);
        
        if (party == ALICE && i < limit)
            cout << row.to_string() << endl;    
    }
    delete io;
}