#include <data/csv_reader.h>
#include <pilot/src/common/shared_schema.h>
#include <pilot/src/common/pilot_utilities.h>


using namespace std;
using namespace vaultdb;

// input generated with:
// ./bin/generate_enrich_data_three_parties pilot/secret_shares/input/ 100
int main(int argc, char **argv) {
    // usage: secret_share_csv <src file> <destination root>
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/secret_share_csv pilot/test/input/chi-patient.csv pilot/test/input/chi-patient.schema  pilot/test/output/chi-patient
    // writes to pilot/test/output/chi-patient.alice pilot/secret_shares/output/chi-patient.bob

    if (argc < 4) {
        cout << "usage: secret_share_csv <src file> <schema file> <destination root>" << endl;
        exit(-1);
    }

    PilotUtilities::setupSystemConfiguration();
    string src_csv = argv[1];
    string schema_file = argv[2];
    string dst_root = argv[3];

    QuerySchema target_schema = QuerySchema::fromFile(schema_file);
    PilotUtilities::secretShareFromCsv(src_csv, target_schema, dst_root);


    std::cout << "Success." << std::endl;

}

