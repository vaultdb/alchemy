#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>
#include <util/data_utilities.h>

using namespace vaultdb;
using namespace std;
// paths are relative to $VAULTDB_ROOT/src/main/cpp
// e.g., ./bin/secret_share_from_query enrich_htn_prod  my_query.sql pilot/secret_shares/output/example
// writes to  pilot/secret_shares/output/example.alice and  pilot/secret_shares/output/example.bob
int main(int argc, char **argv) {

    if (argc < 4) {
        cout << "usage: secret_share_from_query <target_db> <file with sql query> <destination root>" << endl;
        exit(-1);
    }

    setup_plain_prot(false, "");
    string db_name = argv[1];
    vector<string> query_lines = DataUtilities::readTextFile(argv[2]);
    string query = "";
    for(string line : query_lines) {
        query += line + " ";
    }
    string dst_root = argv[3];



    PilotUtilities::secretShareFromQuery(db_name, query, dst_root);

    finalize_plain_prot();

}


