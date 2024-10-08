#include "emp-sh2pc/semihonest.h"
#include "util/data_utilities.h"
#include<cstring>

using namespace std;
using namespace emp;

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
    port = atoi(argv[3]);
    int col_ordinal = atoi(argv[4]);
    string secret_shares_file = argv[5];
    // get schema:
    int suffix_start = secret_shares_file.find_last_of(".");
    string schema_file = secret_shares_file.substr(0, suffix_start) + ".schema";
    cout << "Schema file: " << schema_file << '\n';

    vector<int8_t> share_bytes = DataUtilities::readFile(secret_shares_file);

    auto schema = DataUtilities::readTextFileToString(schema_file);

    finalize_semi_honest();

}