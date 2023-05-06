#include <iostream>
#include <query_table/query_schema.h>
#include <pilot/src/common/shared_schema.h>
#include <util/data_utilities.h>
#include <sstream>

using namespace std;
using namespace vaultdb;


std::string src_path = Utilities::getCurrentWorkingDirectory() + "/";
std::string dst_path = src_path;

// output schema consists of rollup field and counts, e.g.:
// result (#0 int32 .age_strata, #1 int64 .numerator, #2 int64 .denominator, #3 int64 .numerator_multisite, #4 int64 .denominator_multisite)
QuerySchema getSchema(const std::string & rollupName) {
    QuerySchema schema = SharedSchema::getInputSchema();
    QuerySchema outputSchema;
    QueryFieldDesc rollupField = schema.getField(rollupName);
    rollupField.setOrdinal(0);

    QueryFieldDesc numerator(1, "numerator", "", FieldType::LONG, 0);
    QueryFieldDesc denominator(2, "denominator", "", FieldType::LONG, 0);
    QueryFieldDesc numerator_multisite(3, "numerator_multisite", "", FieldType::LONG, 0);
    QueryFieldDesc denominator_multisite(4, "denominator_multisite", "", FieldType::LONG, 0);



    outputSchema.putField(rollupField);
    outputSchema.putField(numerator);
    outputSchema.putField(denominator);
    outputSchema.putField(numerator_multisite);
    outputSchema.putField(denominator_multisite);

    return outputSchema;
}

void revealRollup(const std::string & rollupName) {

    std::string aliceFile = src_path + "/" + rollupName + ".alice";
    std::string bobFile = src_path + "/" + rollupName + ".bob";

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


    QuerySchema rollupSchema = getSchema(rollupName); // rollupName != "age_strata" ? getSchema(rollupName) : getSchema("age_days");
    std::shared_ptr<PlainTable> result = PlainTable::deserialize(rollupSchema, revealed);

    std::stringstream schema_str;
    schema_str << *result->getSchema() << std::endl;
    std::string csv = schema_str.str();

    for(size_t i = 0; i < result->getTupleCount(); ++i) {
        PlainTuple tuple = result->getTuple(i);
        if(!tuple.getDummyTag())
            csv += (*result)[i].toString() + "\n";
    }

    std::cout << "Revealing " << csv << std::endl;
    std::string out_file = dst_path + "/" + rollupName + ".csv";

    DataUtilities::writeFile(out_file, csv);
}


// input generated with:
// ./bin/run_data_partner ...
// for each rollup (e.g., ethnicity), take in Alice and Bob's inputs (e.g., ethnicity.alice, ethnicity.bob) and write out a csv with their results (e.g., ethnicity.csv)
int main(int argc, char **argv) {
    // paths are relative to $VAULTDB_ROOT/src/main/cpp
    // e.g., ./bin/assemble_secret_shares pilot/secret_shares/xor  pilot/secret_shares/revealed

    if (argc < 3) {
        cout << "usage: assemble_secret_shares <src path> <dst path>" << endl;
        exit(-1);
    }

    src_path +=  argv[1];
    dst_path += argv[2];

    Utilities::mkdir(dst_path);

    vector<string> rollups{"age_strata", "sex", "ethnicity", "race"};

    for(std::string rollup : rollups) {
        revealRollup(rollup);
    }

}

