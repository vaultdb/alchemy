 #include "plain/plain_base_test.h"
#include <util/type_utilities.h>
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include "pilot/src/common/shared_schema.h"

using namespace std;


class SerializationTest : public PlainBaseTest {


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




TEST_F(SerializationTest, capricorn_test) {

    QuerySchema targetSchema = SharedSchema::getInputSchema();

    string currentWorkingDirectory = Utilities::getCurrentWorkingDirectory();
    string srcCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-patient.csv";

    std::unique_ptr<PlainTable> inputTable = CsvReader::readCsv(srcCsvFile, targetSchema);

     vector<int8_t> serialized = inputTable->serialize();

     std::shared_ptr<PlainTable> deserialized = PlainTable::deserialize(targetSchema, serialized);

    ASSERT_EQ(*inputTable, *deserialized);



}


TEST_F(SerializationTest, xored_serialization_test) {

    QuerySchema targetSchema = SharedSchema::getInputSchema();

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

    QuerySchema targetSchema = SharedSchema::getInputSchema();
    // JMR return here
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


    string expectedCsvFile = currentWorkingDirectory + "/pilot/test/input/chi-patient.csv";
    std::unique_ptr<PlainTable> expected = CsvReader::readCsv(expectedCsvFile, targetSchema);

    ASSERT_EQ(*expected, *deserialized);


}


