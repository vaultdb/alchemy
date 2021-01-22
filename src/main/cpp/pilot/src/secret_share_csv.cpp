#include <data/CsvReader.h>
#include <pilot/src/common/shared_schema.h>
#include "secret_share_csv.h"


void SecretShareCsv::writeFile(const std::string &  fileName, const std::vector<int8_t> & contents) {
    std::ofstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);
    if(!outFile.is_open()) {
        throw "Could not write output file " + fileName;
    }
    outFile.write((char * ) contents.data(), contents.size());
    outFile.close();
}

void SecretShareCsv::SecretShareTable(const string &srcCsvFile, const QuerySchema &srcSchema, const string &dstRoot) {


    std::unique_ptr<QueryTable> inputTable = CsvReader::readCsv(srcCsvFile, srcSchema);
    SecretShares shares = inputTable->generateSecretShares();

    writeFile(dstRoot + ".alice", shares.first);
    writeFile(dstRoot + ".bob", shares.second);



}


// input generated with:
// ./bin/generate_enrich_data_three_parties pilot/secret_shares/input/ 100
// TODO: integrate host ID later for deduplication
int main(int argc, char **argv) {
    // usage: secret_share_csv <src file> <destination root>
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/secret_share_csv pilot/secret_shares/input/chi-patient.csv pilot/secret_shares/output/chi-patient
    // writes to pilot/secret_shares/output/chi-patient.a pilot/secret_shares/output/chi-patient.b

    if (argc < 3) {
        cout << "usage: secret_share_csv <src file> <destination root>" << endl;
        exit(-1);
    }

    QuerySchema targetSchema = SharedSchema::getInputSchema();


    SecretShareCsv::SecretShareTable(string(argv[1]), targetSchema, string(argv[2]));


}

