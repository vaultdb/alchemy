#include <util/data_utilities.h>
#include "util/emp_manager/sh2pc_manager.h"
#include "query_table/column_table.h"
#include<cstring>

using namespace std;
using namespace emp;
using namespace vaultdb;



// usage: ./reveal_public_column <party 1 || 2> <host> <port>  <secret shares file> <optional max row count>
// e.g.,
// ./bin/reveal_xor_shared_table 1 127.0.0.1 4444 pilot/results/phame/aggregate_only/phame_rollup.alice <max row count>
// ./bin/reveal_xor_shared_table 2 127.0.0.1 4444 pilot/results/phame/aggregate_only/phame_rollup.bob <max row count>

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
    int limit = -1;
    if(argc > 5) {
        limit = atoi(argv[5]);
    }

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

    auto secret_shared = DataUtilities::readSecretSharedInput(secret_shares_file, plain_schema, limit);
    auto plain = secret_shared->reveal();

    cout << "writing revealed data to " << dst_file << '\n';
    DataUtilities::writeFile(dst_file, plain->toString());

    emp_manager->flush();
    delete emp_manager;
    s.emp_manager_ = nullptr;

}
