#include <util/data_utilities.h>
#include "util/emp_manager/sh2pc_manager.h"
#include "query_table/column_table.h"
#include<cstring>
#include <data/psql_data_provider.h>

using namespace std;
using namespace emp;
using namespace vaultdb;



// usage: ./reveal_public_column <src root> <dst root>
// e.g.,
// ./bin/reveal_local_xor_shared_table  ~/phame_results/observed/phame_rollup
// directory contains phame_rollup.alice phame_rollup.bob and phame_rollup.schema
// ./bin/reveal_local_xor_shared_table pilot/results/zodiac/broadband pilot/results/zodiac/observed/broadband zodiac
int main(int argc, char **argv) {
    int port, party;
    if(argc < 3) {
        cout << "Usage: ./reveal_local_xor_shared_table <src file root> <dst file root> <optional: db to insert table> \n";
        return 1;
    }

    string src_file_root(argv[1]);
    string dst_file_root(argv[2]);

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
    PlainTable *result = QueryTable<bool>::deserialize(rollup_schema, revealed.data(), revealed.size());

    std::string csv;
    for(size_t i = 0; i < result->tuple_cnt_; ++i) {
        if(!result->getDummyTag(i)) {
            PlainTuple tuple = result->getPlainTuple(i);
            auto tmp = tuple.toString();
            // chop off parentheses
            tmp = tmp.substr(1, tmp.size() - 2);
            csv += tmp + "\n";
        }
    }

    std::string out_file = dst_file_root + ".csv";
    string out_schema = dst_file_root +  ".schema";

    DataUtilities::writeFile(out_file, csv);

    QuerySchema out_schema_desc = result->getSchema();
    out_schema_desc.toFile(out_schema);

    if (argc > 3) {
        string db_name = argv[3];
        std::string table_name = src_file_root.substr(src_file_root.find_last_of("/") + 1);
        if(src_file_root.find_last_of("/") == string::npos) {
            table_name = src_file_root;
        }

        // create table in postgres
        PsqlDataProvider psql;
        psql.runQuery(db_name, "DROP TABLE IF EXISTS " + table_name );

        psql.createTable(db_name, table_name, out_schema_desc);
        if (out_file[0] != '/') {
            // get absolute path
            out_file = Utilities::getCurrentWorkingDirectory() + "/" + out_file;
        }

        string cp_cmd = "COPY " + table_name + " FROM '" + out_file + "' WITH delimiter ','";
        psql.runQuery(db_name, cp_cmd);
    }
    delete result;
}
