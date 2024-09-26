#include "util/data_utilities.h"
#include "query_table/query_table.h"



using namespace std;
using namespace vaultdb;

int main(int argc, char **argv) {

    // paths are relative to local $VAULTDB_ROOT, the src/main/cpp within your vaultdb directory
    // usage: ./secret_share_ompc_table_from_query <db name> "sql statement" <destination root> <party count>
    // e.g., ./secret_share_ompc_table_from_query tpch_unioned_600 "select * from lineitem ORDER BY l_orderkey, l_linenumber"  shares/tpch_unioned_600/lineitem 4
    if (argc < 5) {
        std::cout << "usage: ./secret_share_mpc_table_from_query <db name> \"sql statement\" <destination root> <party count>" << std::endl;
        exit(-1);
    }

    string db_name = argv[1];
    string query = argv[2];
    string dst_root = Utilities::getCurrentWorkingDirectory() + "/" + argv[3];
    int party_count = atoi(argv[4]);

    string dst_dir = dst_root.substr(0, dst_root.find_last_of("/"));
    Utilities::mkdir(dst_dir);

    PlainTable *table = DataUtilities::getQueryResults(db_name, query, false);

    vector<vector<int8_t> > shares = table->generateSecretShares(party_count);

    for(int i = 0; i < party_count; i++) {
        string share_filename = dst_root + "."  + to_string(i);
        DataUtilities::writeFile(share_filename, shares[i]);
    }

    string schema_filename = dst_root + ".schema";
    DataUtilities::writeFile(schema_filename, table->getSchema().prettyPrint());

    delete table;
    cout << "Wrote " << party_count << " shares to " << dst_root << ".[0-" << party_count - 1 << "]" << endl;
}