#include <data/CsvReader.h>
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
int main(int argc, char **argv) {
    // usage: secret_share_csv <src file> <destination root>
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/secret_share_csv pilot/secret_shares/input/chi-patient.csv pilot/secret_shares/output/chi-patient
    // writes to pilot/secret_shares/output/enrich.a pilot/secret_shares/output/enrich.b

    if (argc < 3) {
        cout << "usage: secret_share_csv <src file> <destination root>" << endl;
        exit(-1);
    }

    // CREATE TABLE patient (
//    patid int,
//    zip_marker varchar(3),
//    age_days integer,
//    sex varchar(1),
//    ethnicity bool,
//    race int,
//    numerator int default 0, -- denotes 0 = false, 1 = true
//    denom_excl int default 0 -- denotes 0 = false, 1 = true
//);
    QuerySchema targetSchema(8);
    targetSchema.putField(QueryFieldDesc(0, "patid", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(1, "zip_marker", "patient", TypeId::VARCHAR, 3));
    targetSchema.putField(QueryFieldDesc(2, "age_days", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(3, "sex", "patient", TypeId::VARCHAR, 1));
    targetSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", TypeId::BOOLEAN));
    targetSchema.putField(QueryFieldDesc(5, "race", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(6, "numerator", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(7, "denom_excl", "patient", TypeId::INTEGER32));

    SecretShareCsv::SecretShareTable(string(argv[1]), targetSchema, string(argv[2]));


}

