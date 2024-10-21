#include <util/data_utilities.h>
#include "util/emp_manager/sh2pc_manager.h"
#include "query_table/column_table.h"
#include<cstring>

using namespace std;
using namespace emp;
using namespace vaultdb;


// plaintext input schema, setup for XOR-shared data
// from: union_hybrid_data.cpp
SecureTable *readSecretSharedInput(const string &secret_shares_file, const QuerySchema &plain_schema) {
    QuerySchema secure_schema = QuerySchema::toSecure(plain_schema);
    // read in binary and then xor it with other side to secret share it.
    std::vector<int8_t> src_data = DataUtilities::readFile(secret_shares_file);
    size_t src_byte_cnt = src_data.size();
    size_t src_bit_cnt = src_byte_cnt * 8;
    size_t tuple_cnt = src_bit_cnt / plain_schema.size();

    // convert serialized representation from byte-aligned to bit-by-bit
    size_t dst_bit_cnt = tuple_cnt * secure_schema.size();
//    size_t dst_bit_alloc = dst_bit_cnt + (dst_bit_cnt % 128);  // pad it to 128-bit increments for emp
    bool *dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
    bool *dst_cursor = dst_bools;
    int8_t *src_cursor = src_data.data();

    assert(src_bit_cnt % plain_schema.size() == 0);
    for(int i = 0; i < plain_schema.getFieldCount(); ++i) {
        auto plain_field = plain_schema.getField(i);
        auto secure_field = secure_schema.getField(i);

        if(plain_field.size() == secure_field.size()) { // 1:1, just serialize it
            int write_size = secure_schema.getField(i).size() * tuple_cnt;
            emp::to_bool<int8_t>(dst_cursor, src_cursor, write_size, false);
            dst_cursor += write_size;
            src_cursor += write_size/8;
        }
        else {
            assert(plain_field.size() == (secure_field.size() + 7)); // only for bools
            for(int j = 0; j < tuple_cnt; ++j) {
                *dst_cursor =   ((*src_cursor & 1) != 0); // (bool) *src_cursor;
                ++dst_cursor;
                ++src_cursor;
            }
        } // end for each tuple
    } // end for all fields

    // copy dummy tag
    for(int i = 0; i < tuple_cnt; ++i) {
        *dst_cursor = ((*src_cursor & 1) != 0);
        ++dst_cursor;
        ++src_cursor;
    }


    Integer alice(dst_bit_cnt, 0L, emp::PUBLIC);
    Integer bob(dst_bit_cnt, 0L, emp::PUBLIC);
    int party = SystemConfiguration::getInstance().party_;
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;

    if(party == ALICE) {
        // feed through Alice's data, then wait for Bob's
        manager->feed(alice.bits.data(), ALICE, dst_bools, dst_bit_cnt);
        manager->feed(bob.bits.data(), BOB, nullptr, dst_bit_cnt);
    }
    else {
        manager->feed(alice.bits.data(), ALICE, nullptr, dst_bit_cnt);
        manager->feed(bob.bits.data(), BOB, dst_bools, dst_bit_cnt);

    }

    Integer shared_data = alice ^ bob;
    // remove padding
    //    shared_data.bits.resize(dst_bit_cnt);

    SecureTable *shared_table = ColumnTable<Bit>::deserialize(secure_schema, shared_data.bits);

    delete [] dst_bools;
    return shared_table;

}
// usage: ./reveal_public_column <party 1 || 2> <host> <port>  <secret shares file>
// e.g.,
// ./bin/reveal_xor_shared_table 1 127.0.0.1 4444 0 pilot/results/phame/aggregate_only/phame_cohort_counts.alice
// ./bin/reveal_xor_shared_table 2 127.0.0.1 4444 0 pilot/results/phame/aggregate_only/phame_cohort_counts.bob

int main(int argc, char **argv) {
    int port, party;
    if(argc < 4) {
        cout << "Usage: ./reveal_public_column <party 1 || 2> <host> <port> <secret shares file>\n";
        return 1;
    }

    party = atoi(argv[1]);
    string host = argv[2];
    port = atoi(argv[3]);
    string secret_shares_file(argv[4]);

    cout << "Setup: party=" << party << " host=" << host << " port=" << port << " input_shares=" << secret_shares_file << '\n';
    auto emp_manager = new SH2PCManager(host, party, port);
    SystemConfiguration & s = SystemConfiguration::getInstance();
    s.emp_mode_ = EmpMode::SH2PC;
    s.emp_manager_ = emp_manager;
    BitPackingMetadata md; // empty set
    s.initialize("", md, StorageModel::COLUMN_STORE);


    // get schema:
    int suffix_start = secret_shares_file.find_last_of(".");
    string schema_file = secret_shares_file.substr(0, suffix_start) + ".schema";
    string dst_file = secret_shares_file.substr(0, suffix_start) + ".csv";
    auto schema_spec = DataUtilities::readTextFileToString(schema_file);
    auto schema = QuerySchema(schema_spec);
    auto plain_schema = QuerySchema::toPlain(schema);

    auto secret_shared = readSecretSharedInput(secret_shares_file, plain_schema);
    auto plain = secret_shared->reveal();

    cout << "writing revealed data to " << dst_file << '\n';
    DataUtilities::writeFile(dst_file, plain->toString());

    emp_manager->flush();
    delete emp_manager;
    s.emp_manager_ = nullptr;

}
