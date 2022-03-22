 #include "plain/plain_base_test.h"
#include <util/type_utilities.h>
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include "pilot/src/common/shared_schema.h"

using namespace std;


class SerializationTest : public PlainBaseTest {
protected:
  QuerySchema getCapricornSchema();

};


TEST_F(SerializationTest, typesTest) {
    // selecting one of each type:
    // int
    // varchar(44)
    // fixed char(1)
    // numeric
    // date

   std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                            "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "
                            "FROM lineitem "
                            "ORDER BY l_shipdate "
                            "LIMIT 10";

    std::shared_ptr<PlainTable> inputTable = DataUtilities::getQueryResults(dbName, inputQuery, false);

    vector<int8_t> tableData = inputTable->serialize();
    uint32_t expectedSize = inputTable->getSchema()->size() / 8 * 10;
    ASSERT_EQ(tableData.size(), expectedSize);

    std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(*inputTable->getSchema(), tableData);
    ASSERT_EQ(*inputTable, *deserialized);


}


QuerySchema SerializationTest::getCapricornSchema() {
    QuerySchema targetSchema(7);
    // 12,5,U,U,4,f,f
    targetSchema.putField(QueryFieldDesc(0, "pat_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "age_strata", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(2, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(3, "ethnicity", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "race", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(5, "numerator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(6, "denom_excl", "patient", FieldType::BOOL));

    return targetSchema;
}



TEST_F(SerializationTest, capricorn_test) {


  QuerySchema targetSchema = getCapricornSchema();
    

    
    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string srcCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-multisite-patient.csv";

    std::unique_ptr<PlainTable> inputTable = CsvReader::readCsv(srcCsvFile, targetSchema);

     vector<int8_t> serialized = inputTable->serialize();

     std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(targetSchema, serialized);

    ASSERT_EQ(*inputTable, *deserialized);



}


TEST_F(SerializationTest, xored_serialization_test) {

    QuerySchema targetSchema = getCapricornSchema();


    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string srcCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-multisite-patient.csv";

    std::unique_ptr<PlainTable> inputTable = CsvReader::readCsv(srcCsvFile, targetSchema);
    vector<int8_t> serialized = inputTable->serialize();

    // alice contains random values... ssh!
    string aliceFile = currentWorkingDirectory + "/pilot/test/output/chi-patient-multisite.alice";
    vector<int8_t> randInts =  DataUtilities::readFile(aliceFile);

    vector<int8_t>::iterator writePos = serialized.begin();
    vector<int8_t>::iterator readPos = randInts.begin();

    while(writePos != serialized.end()) {
        *writePos ^= *readPos;
        ++writePos;
        ++readPos;
    }


    // repeat the process again to decrypt it

    writePos = serialized.begin();
    readPos = randInts.begin();

    while(writePos != serialized.end()) {
        *writePos ^= *readPos;
        ++writePos;
        ++readPos;
    }


    std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(targetSchema, serialized);

    ASSERT_EQ(*inputTable, *deserialized);


}



TEST_F(SerializationTest, capricorn_deserialization) {

  
  QuerySchema targetSchema = SharedSchema::getInputSchema();
    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string aliceFile = currentWorkingDirectory + "/pilot/test/output/chi-patient-multisite.alice";
    string bobFile = currentWorkingDirectory + "/pilot/test/output/chi-patient-multisite.bob";

    vector<int8_t> aliceBits = DataUtilities::readFile(aliceFile);
    vector<int8_t> bobBits = DataUtilities::readFile(bobFile);

    vector<int8_t> serialized = aliceBits;

    vector<int8_t>::iterator writePos = serialized.begin();
    vector<int8_t>::iterator readPos = bobBits.begin();

    while(writePos != serialized.end()) {
        *writePos ^= *readPos;
        ++writePos;
        ++readPos;
    }

    std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(targetSchema, serialized);

    std::string expected_query = "SELECT  study_year, pat_id, age_strata, sex, ethnicity, race, numerator, denominator, denom_excl FROM patient WHERE site_id=3 AND multisite ORDER BY study_year, pat_id";
    std::string db_name = "enrich_htn_unioned_3pc";

    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(db_name, expected_query, false);

    ASSERT_EQ(*expected, *deserialized);


}


