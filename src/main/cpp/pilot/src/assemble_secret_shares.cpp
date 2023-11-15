#include <iostream>
#include <query_table/query_schema.h>
#include <util/data_utilities.h>
#include <sstream>
#include <pilot/src/common/pilot_utilities.h>
#include "query_table/query_table.h"

using namespace std;
using namespace vaultdb;


std::string src_path = Utilities::getCurrentWorkingDirectory() + "/";
std::string dst_path = src_path;

void revealRollup(const std::string & rollup_name) {

    std::string alice_file = src_path + "/" + rollup_name + ".alice";
    std::string bob_file = src_path + "/" + rollup_name + ".bob";
    std::string schema_file = src_path + "/" + rollup_name + ".schema";

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

    std::cout << "Revealing " << csv << std::endl;
    std::string out_file = dst_path + "/" + rollup_name + ".csv";
    string out_schema = dst_path + "/" + rollup_name + ".schema";

    DataUtilities::writeFile(out_file, csv);
    QuerySchema out_schema_desc = result->getSchema();
    out_schema_desc.toFile(out_schema);

    delete result;
}


// input generated with:
// ./bin/run_data_partner ...
// for each rollup (e.g., ethnicity), take in Alice and Bob's inputs (e.g., ethnicity.alice, ethnicity.bob) and write out a csv with their results (e.g., ethnicity.csv)
int main(int argc, char **argv) {
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/assemble_secret_shares pilot/secret_shares/xor  pilot/secret_shares/revealed

    if (argc < 3) {
        cout << "usage: assemble_secret_shares <src path> <dst path>" << endl;
        exit(-1);
    }

    src_path +=  argv[1];
    dst_path += argv[2];
    PilotUtilities::setupSystemConfiguration();

    Utilities::mkdir(dst_path);

    vector<string> rollups{"age_strata", "sex", "ethnicity", "race"};

    for(std::string rollup : rollups) {
        revealRollup(rollup);
    }

}

