#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/project.h>
#include <data/CsvReader.h>


using namespace emp;
using namespace vaultdb::types;



class CsvReaderTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};

TEST_F(CsvReaderTest, lineitemTest) {

 // generated input file by running:
 // psql --csv  -c "SELECT * FROM lineitem ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > lineitem.csv

    std::string inputFile = "test/support/csv/lineitem.csv";
    std::string query = "SELECT * FROM lineitem ORDER BY (1), (2)  LIMIT 50";

    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> expected = dataProvider.getQueryTable("tpch_unioned", query);

    std::unique_ptr<QueryTable> observed = CsvReader::readCsv(inputFile, expected->getSchema());

    ASSERT_EQ(expected, observed);


}


TEST_F(CsvReaderTest, customerTest) {

    // generated input file by running:
    // psql --csv  -c "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > customer.csv

    std::string inputFile = "test/support/csv/customer.csv";
    std::string query = "SELECT * FROM customer ORDER BY (1), (2)  LIMIT 50";

    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> expected = dataProvider.getQueryTable("tpch_unioned", query);

    std::unique_ptr<QueryTable> observed = CsvReader::readCsv(inputFile, expected->getSchema());

    ASSERT_EQ(expected, observed);


}


TEST_F(CsvReaderTest, ordersTest) {

    // generated input file by running:
    // psql --csv  -c "SELECT * FROM orders ORDER BY (1), (2)  LIMIT 50" tpch_unioned  > orders.csv

    std::string inputFile = "test/support/csv/orders.csv";
    std::string query = "SELECT * FROM orders ORDER BY (1), (2)  LIMIT 50";

    PsqlDataProvider dataProvider;
    std::unique_ptr<QueryTable> expected = dataProvider.getQueryTable("tpch_unioned", query);

    std::unique_ptr<QueryTable> observed = CsvReader::readCsv(inputFile, expected->getSchema());

    ASSERT_EQ(expected, observed);


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}