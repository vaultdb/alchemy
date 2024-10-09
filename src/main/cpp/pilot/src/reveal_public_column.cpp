#include <util/data_utilities.h>
#include "common/union_hybrid_data.h"
#include "emp-sh2pc/semihonest.h"
#include<cstring>

using namespace std;
using namespace emp;
using namespace vaultdb;

// usage: ./reveal_public_column <party 1 || 2> <host> <port> <col ordinal> <secret shares file>
// e.g.,
// ./reveal_public_column 1 127.0.01. 4444 0 pilot/results/phame/aggregate_only/phame_cohort_counts.alice
// ./reveal_public_column 2 127.0.01. 4444 0 pilot/results/phame/aggregate_only/phame_cohort_counts.bob

int main(int argc, char **argv) {
    int port, party;
    if(argc < 5) {
        cout << "Usage: ./reveal_public_column <party 1 || 2> <host> <port> <col ordinal> <secret shares file>\n";
        return 1;
    }

    party = atoi(argv[1]);
    string host = argv[2];
    port = 4445; //atoi(argv[3]);
    int col_ordinal = atoi(argv[4]);
    string secret_shares_file(argv[5]);

    cout << "Setup: party=" << party << " host=" << host << " port=" << port << " col_idx=" << col_ordinal << " input_shares=" << secret_shares_file << '\n';
    NetIO *io = new NetIO(party==ALICE ? nullptr : host.c_str(), port);
    setup_semi_honest(io, party);


    // get schema:
    int suffix_start = secret_shares_file.find_last_of(".");
    cout << "Secret shares file: " << secret_shares_file << '\n';
    string schema_file = secret_shares_file.substr(0, suffix_start) + ".schema";
    cout << "Schema file: " << schema_file << '\n';

    auto schema_spec = DataUtilities::readTextFileToString(schema_file);
    auto schema = QuerySchema(schema_spec);
    auto public_schema = QuerySchema::toPlain(schema);

    auto secret_shared = UnionHybridData::readSecretSharedInput(secret_shares_file, public_schema);
    cout << "Revealing column " << col_ordinal << "'s  " << secret_shared->tuple_cnt_ << " rows.\n";
    QueryFieldDesc src_schema = schema.getField(col_ordinal);

    for(auto i = 0; i < secret_shared->tuple_cnt_; i++) {
        auto field = secret_shared->getField(i, col_ordinal);
        auto revealed = field.reveal(src_schema, PUBLIC);
        cout << revealed.toString() << '\n';
    }

    finalize_semi_honest();

}
