#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/CsvReader.h>

using namespace emp;
using namespace vaultdb;
using namespace std;


class SerializationTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");
    };

    void TearDown() override{
        finalize_plain_prot();
    };



    const std::string dbName = "tpch_unioned";


    QuerySchema getInputSchema();
};


TEST_F(SerializationTest, typesTest) {
    // selecting one of each type:
    // int
    // varchar
    // fixed char
    // numeric
    // date

   std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, l_comment, "
                            "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "
                            "FROM lineitem "
                            "ORDER BY l_shipdate "
                            "LIMIT 10";

    std::shared_ptr<PlainTable> inputTable = DataUtilities::getQueryResults(dbName, inputQuery, false);
    vector<int8_t> tableData = inputTable->serialize();
    uint32_t expectedSize = inputTable->getSchema().size()/8 * 10;
    ASSERT_EQ(tableData.size(), expectedSize);
    std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(inputTable->getSchema(), tableData);

    ASSERT_EQ(*inputTable, *deserialized);


}



QuerySchema SerializationTest::getInputSchema() {
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
    QuerySchema targetSchema(9);
    targetSchema.putField(QueryFieldDesc(0, "patid", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "zip_marker", "patient", FieldType::STRING, 3));
    targetSchema.putField(QueryFieldDesc(2, "age_days", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(3, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(5, "race", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(6, "numerator", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(7, "denom_excl", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(8, "site_id", "patient", FieldType::INT));

    return targetSchema;
}


TEST_F(SerializationTest, capricornTest) {

    QuerySchema targetSchema = getInputSchema();

    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string srcCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-patient.csv";

    std::unique_ptr<PlainTable> inputTable = CsvReader::readCsv(srcCsvFile, targetSchema);

    // temporarily truncate this to first 5 tuples
    inputTable->resize(5);
    std::cout << "Input table: " << *inputTable << std::endl;

    vector<int8_t> serialized = inputTable->serialize();

    std::cout << "Deserializing" << std::endl;
    std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(targetSchema, serialized);

    std::cout << "Comparing values" << std::endl;
    ASSERT_EQ(*inputTable, *deserialized);

    std::cout << "Cleanup." << std::endl;


}


TEST_F(SerializationTest, xored_serialization_test) {

    QuerySchema targetSchema = getInputSchema();

    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string srcCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-patient.csv";

    std::unique_ptr<PlainTable> inputTable = CsvReader::readCsv(srcCsvFile, targetSchema);
    vector<int8_t> serialized = inputTable->serialize();

    // alice contains random values... ssh!
    string aliceFile = currentWorkingDirectory + "/pilot/test/output/chi-patient.alice";
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

    QuerySchema targetSchema = getInputSchema();
    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string aliceFile = currentWorkingDirectory + "/pilot/test/output/chi-patient.alice";
    string bobFile = currentWorkingDirectory + "/pilot/test/output/chi-patient.bob";

    vector<int8_t> aliceBits = DataUtilities::readFile(aliceFile);
    vector<int8_t> bobBits = DataUtilities::readFile(bobFile);

    //  bob has "xored bits"
    vector<int8_t> serialized = aliceBits;

    vector<int8_t>::iterator writePos = serialized.begin();
    vector<int8_t>::iterator readPos = bobBits.begin();

    while(writePos != serialized.end()) {
        *writePos ^= *readPos;
        ++writePos;
        ++readPos;
    }


    std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(targetSchema, serialized);
    /* Expected:

     First plaintext bytes: 2,0,0,0,48,50,56,-60,49,0,0
    First xored bytes: 44,-42,78,-12,-69,51,-4,67,88,114,-69

     Observed:
     First xored bits: 45,-42,78,-12,-69,48,-9,-12,58,114,-69
    First plaintext bytes: 3,0,0,0,48,49,51,115,83,0,0

     Deserialized tuple: (2, '028', 12740, 'F', false, 2, 0, 0, 3)
Deserialized tuple: (11, '036', 18528, 'M', true, 5, 0, 1, 3)
Deserialized tuple: (12, '030', 22768, 'M', true, 4, 0, 1, 3)
Deserialized tuple: (13, '029', 36285, 'M', true, 2, 0, 0, 3)
     */

    string expectedCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-patient.csv";
    std::unique_ptr<PlainTable> expected = CsvReader::readCsv(expectedCsvFile, targetSchema);

    ASSERT_EQ(*expected, *deserialized);


}


