#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/CsvReader.h>
#include <unistd.h>
#include <limits.h>


using namespace emp;
using namespace vaultdb::types;
using namespace vaultdb;


class CsvReaderTest : public ::testing::Test {


protected:
    void SetUp() override {

        currentWorkingDirectory = DataUtilities::getCurrentWorkingDirectory();

    };
    void TearDown() override{};
    std::string currentWorkingDirectory;

    QueryFieldDesc convertDateField(const QueryFieldDesc & srcField) {
        return QueryFieldDesc(srcField, types::TypeId::DATE);
    }
};

TEST_F(CsvReaderTest, lineitemTest) {

 // generated input file by running:
 // psql  -t --csv  -c "SELECT * FROM lineitem ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > lineitem.csv

    std::string inputFile =  currentWorkingDirectory + "/test/support/csv/lineitem.csv";
    std::string query = "SELECT * FROM lineitem ORDER BY (1), (2)  LIMIT 50";

    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> expected = dataProvider.getQueryTable("tpch_unioned", query);
    QuerySchema csvSchema = expected->getSchema();
    // substitute longs with dates in the appropriate cols, fields 10, 11, 12
    csvSchema.putField(convertDateField(csvSchema.getField(10)));
    csvSchema.putField(convertDateField(csvSchema.getField(11)));
    csvSchema.putField(convertDateField(csvSchema.getField(12)));


    std::unique_ptr<QueryTable> observed = CsvReader::readCsv(inputFile, csvSchema);
    observed->setSchema(expected->getSchema()); // switch back from date schema

    ASSERT_EQ(*expected, *observed);


}


TEST_F(CsvReaderTest, quotedStringTest) {
    std::string testStr = "16,Customer#000000016,\"cYiaeMLZSMAOQ2 d0W,\",10,20-781-609-3107,4681.03,FURNITURE ,kly silent courts. thinly regular theodolites sleep fluffily after ";

    // grab customer table for schema:
    std::string query = "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 50";
    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> expected = dataProvider.getQueryTable("tpch_unioned", query);

    QueryTuple parsedTuple = CsvReader::parseTuple(testStr, expected->getSchema());
    QueryTuple expectedTuple = expected->getTuple(15);

    assert(parsedTuple == expectedTuple);

    // won't compile, don't know why yet.
    //ASSERT_EQ(parsedTuple, expectedTuple);
}

TEST_F(CsvReaderTest, customerTest) {

    // generated input file by running:
    // psql --csv  -t -c "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > customer.csv

    std::string inputFile =  currentWorkingDirectory +  "/test/support/csv/customer.csv";
    std::string query = "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 50";

    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> expected = dataProvider.getQueryTable("tpch_unioned", query);

    std::unique_ptr<QueryTable> observed = CsvReader::readCsv(inputFile, expected->getSchema());

    ASSERT_EQ(*expected, *observed);


}


TEST_F(CsvReaderTest, ordersTest) {

    // generated input file by running:
    // psql --csv -t  -c "SELECT * FROM orders ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > orders.csv

    std::string inputFile = currentWorkingDirectory +  "/test/support/csv/orders.csv";
    std::string query = "SELECT * FROM orders ORDER BY (1), (2)  LIMIT 50";

    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> expected = dataProvider.getQueryTable("tpch_unioned", query);

    QuerySchema csvSchema = expected->getSchema();
    // o_orderdate(4) set schema to date
    csvSchema.putField(convertDateField(csvSchema.getField(4)));

    std::unique_ptr<QueryTable> observed = CsvReader::readCsv(inputFile, csvSchema);
    observed->setSchema( expected->getSchema());

    ASSERT_EQ(*expected, *observed);


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}