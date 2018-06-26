#include <emp-sh2pc/emp-sh2pc.h>
#include <pqxx/pqxx>
#include <utils/row.h>

using namespace emp;
using namespace std;
using namespace pqxx;

// Constants
#define OID_STRING 1043
#define LENGTH_STRING 256
#define OID_INT 20
#define LENGTH_INT 64

// Connection variables
string alice_db = "$db_name_1";
string bob_db = "$db_name_2";
string user_db = "$user";
string host_db = "$db_host_addr";
string port_db = "$db_port";

// Plaintext Query
$src_sql


int alice_size = 159;   //run time
int bob_size = 591; //run time

// Generated variables
int initial_row_size = $row_size;     //compile time
int limit = $limit;     //compile time
int col_length0 = $col_length_0;  //compile time
int col_length1 = $col_length_1;   //compile time

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

// DB connection functions
std::vector<Row> execute_sql(string sql, int party) {
    std::vector<Row> res;
    try {
        string db = (party == ALICE) ? alice_db : bob_db;
        string config= "dbname = " + db + " user = " + user_db + " host = " + host_db + " port = " + port_db;
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
            		if (oid == OID_STRING) {
            			lengths.push_back(LENGTH_STRING);
            			bin_str += str_to_binary(row[j].as<string>(), LENGTH_STRING);
            		} else if (oid == OID_INT) {
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
class Data {public:
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

Data* op_merge(string sql, int bit_length, int alice_size, int bob_size, int party) {
	std::vector<Row> in = execute_sql(sql, party);
	std::sort(in.begin(), in.end());
	bool *local_data = concat(in, initial_row_size);

    Integer * res = new Integer[alice_size + bob_size];
    Bit * tmp = new Bit[bit_length * (alice_size + bob_size)];
    for (int i = 0; i < alice_size; ++i)
        for (int j = 0; j < bit_length; ++j)
            tmp[i*bit_length + j] = Bit((ALICE==party) ? 
                local_data[(alice_size-i-1)*bit_length + j]:0, ALICE);

    Batcher batcher;
    for (int i = 0; i < bob_size*bit_length; ++i)
        batcher.add<Bit>((BOB==party) ? local_data[i]:0);
    batcher.make_semi_honest(BOB);

    for (int i = 0; i < bob_size*bit_length; ++i)
        tmp[i+alice_size*bit_length] = batcher.next<Bit>();

    for(int i = 0; i < alice_size + bob_size; ++i)
        res[i] = Integer(bit_length, tmp+bit_length*i);


    bitonic_merge_sql(res, 0, alice_size + bob_size, Bit(true), 0, col_length0);
    Data * d = new Data;
    d->data = res;
    d->public_size = alice_size + bob_size;
    d->real_size = Integer(64, d->public_size, PUBLIC);
        
    return d;
}
void op_aggregate (Data * data) {
    for (int i = 0; i < data->public_size - 1; ++i) {
        Integer id1(col_length0, data->data[i].bits);
        Integer cnt1(col_length1, data->data[i].bits + col_length0);
        Integer id2(col_length0, data->data[i+1].bits);
        Integer cnt2(col_length1, data->data[i+1].bits + col_length0);
        Bit eq = id1 == id2;
        cnt1 = If(eq, cnt1+cnt2, cnt1);
        cnt2 = If(eq, Integer(col_length1, 0, PUBLIC), cnt2);
        memcpy(data->data[i].bits+col_length0,cnt1.bits, col_length1*sizeof(Bit));
        memcpy(data->data[i+1].bits+col_length0,cnt2.bits, col_length1*sizeof(Bit));
    }
}

void op_sort(Data * data) {
    bitonic_sort_sql(data->data, 0, data->public_size, Bit(false), col_length0, col_length1);
}

void op_distinct(Data *data) {

}

void op_join(Data *left, Data *right) {

}

void op_window_aggregate(Data *data) {

}

// Main execution
int main(int argc, char** argv) {
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    setup_semi_honest(io, party);
    $functions
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
