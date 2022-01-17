#include <data/csv_reader.h>
#include <pilot/src/common/shared_schema.h>
#include <util/data_utilities.h>


using namespace std;
using namespace vaultdb;

// input generated with:
// ./bin/generate_enrich_data_three_parties pilot/secret_shares/input/ 100
int main(int argc, char **argv) {
    // usage: secret_share_csv <src file> <destination root>
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/secret_share_csv pilot/test/input/chi-patient.csv pilot/test/output/chi-patient
    // writes to pilot/test/output/chi-patient.alice pilot/secret_shares/output/chi-patient.bob

    if (argc < 3) {
        cout << "usage: secret_share_csv <src file> <destination root>" << endl;
        exit(-1);
    }

    setup_plain_prot(false, "");
    QuerySchema target_schema = SharedSchema::getInputSchema();
    string src_csv = argv[1];
    string dst_root = argv[2];

    std::unique_ptr<PlainTable> inputTable = CsvReader::readCsv(src_csv, target_schema);
    SecretShares shares = inputTable->generateSecretShares();

    DataUtilities::writeFile(dst_root + ".alice", shares.first);
    DataUtilities::writeFile(dst_root + ".bob", shares.second);


    finalize_plain_prot();

    std::cout << "Success." << std::endl;

}

