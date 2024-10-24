#include <util/data_utilities.h>
#include "util/emp_manager/sh2pc_manager.h"
#include "query_table/column_table.h"
#include<cstring>

using namespace std;
using namespace emp;
using namespace vaultdb;



// usage: ./reveal_public_column <party 1 || 2> <host> <port>  <secret shares file> <optional max row count>
// e.g.,
// ./bin/reveal_local_xor_shared_table  ~/phame_results/observed/phame_rollup
// directory contains phame_rollup.alice phame_rollup.bob and phame_rollup.schema

int main(int argc, char **argv) {
    int port, party;
    if(argc < 3) {
        cout << "Usage: ./reveal_local_xor_shared_table <src file root> <dst file root> \n";
        return 1;
    }

    string src_file_root(argv[1]);
    string dst_file_root(argv[2]);
    // get schema:
    string schema_file = src_file_root + ".schema";



    std::string alice_file = src_file_root+ ".alice";
    std::string bob_file = src_file_root + ".bob";
    std::string schema_file = src_file_root + ".schema";

    vector<int8_t> alice_bits = DataUtilities::readFile(alice_file);
    vector<int8_t> bob_bits = DataUtilities::readFile(bob_file);

    vector<int8_t> revealed;

    assert(alice_bits.size() == bob_bits.size());
    revealed.resize(alice_bits.size());

    auto alice_pos = alice_bits.begin();
    auto bob_pos = bob_bits.begin();
    auto revealed_pos = revealed.begin();

    while(alice_pos != alice_bits.end()) {
        *revealed_pos = *alice_pos ^ *bob_pos;
        ++alice_pos;
        ++bob_pos;
        ++revealed_pos;
    }


    QuerySchema rollup_schema = QuerySchema::fromFile(schema_file);
    rollup_schema = QuerySchema::toPlain(rollup_schema);
    PlainTable *result = QueryTable<bool>::deserialize(rollup_schema, revealed);

    std::string csv;
    for(size_t i = 0; i < result->tuple_cnt_; ++i) {
        if(!result->getDummyTag(i)) {
            PlainTuple tuple = result->getPlainTuple(i);
            csv += tuple.toString() + "\n";
        }
    }

    std::string out_file = dst_file_root + ".csv";
    string out_schema = dst_file_root +  ".schema";

    DataUtilities::writeFile(out_file, csv);

    QuerySchema out_schema_desc = result->getSchema();
    out_schema_desc.toFile(out_schema);

    delete result;
}
