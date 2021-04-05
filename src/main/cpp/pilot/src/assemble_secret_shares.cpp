#include <iostream>
#include <query_table/query_schema.h>
#include <pilot/src/common/shared_schema.h>
#include <util/data_utilities.h>

using namespace std;
using namespace vaultdb;


// output schema consists of rollup field and a count (int64_t)
QuerySchema getSchema(const std::string & rollupName) {
    QuerySchema schema = SharedSchema::getInputSchema();
    QuerySchema outputSchema(2);
    const QueryFieldDesc rollupField = schema.getField(rollupName);
    QueryFieldDesc countField(1, "count", "", FieldType::LONG, 0);

    outputSchema.putField(rollupField);
    outputSchema.putField(countField);

    return outputSchema;
}

void revealRollup(const std::string & rollupName) {

    std::string aliceFile = rollupName + ".alice";
    std::string bobFile = rollupName + ".bob";

    vector<int8_t> aliceBits = DataUtilities::readFile(aliceFile);
    vector<int8_t> bobBits = DataUtilities::readFile(bobFile);
    vector<int8_t> revealed;

    assert(aliceBits.size() == bobBits.size());
    revealed.resize(aliceBits.size());

    vector<int8_t>::iterator  alicePos = aliceBits.begin();
    vector<int8_t>::iterator  bobPos = bobBits.begin();
    vector<int8_t>::iterator  revealedPos = revealed.begin();

    while(alicePos != aliceBits.end()) {
        *revealedPos = *alicePos ^ *bobPos;
        ++alicePos;
        ++bobPos;
        ++revealedPos;
    }

    QuerySchema rollupSchema = getSchema(rollupName);
    std::shared_ptr<PlainTable> result = PlainTable::deserialize(rollupSchema, revealed);

    std::string csv;
    for(size_t i = 0; i < result->getTupleCount(); ++i)
        csv += (*result)[i].toString() + "\n";

    std::string outputFileName = rollupName + ".csv";
    DataUtilities::writeFile(outputFileName, csv);
}


// input generated with:
// ./bin/run_data_partner ...
// for each rollup (e.g., ethnicity), take in Alice and Bob's inputs (e.g., ethnicity.alice, ethnicity.bob) and write out a csv with their results (e.g., ethnicity.csv)
int main(int argc, char **argv) {
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/secret_share_csv pilot/test/input/chi-patient.csv pilot/test/output/chi-patient
    // writes to pilot/test/output/chi-patient.alice pilot/secret_shares/output/chi-patient.bob

    if (argc < 3) {
        cout << "usage: secret_share_csv <src file> <destination root>" << endl;
        exit(-1);
    }

    vector<string> rollups{"zip_marker", "age_strata", "sex", "ethnicity", "race"};

    for(std::string rollup : rollups) {
        revealRollup(rollup);
    }

}

