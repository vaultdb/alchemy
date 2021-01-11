#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/sort.h>
#include <operators/project.h>

using namespace emp;
using namespace vaultdb::types;
using namespace vaultdb;


class SerializationTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};


    const std::string dbName = "tpch_unioned";

};


TEST_F(SerializationTest, typesTest) {
    // selecting one of each type:
    // int
    // varchar
    // fixed char
    // numeric
    // date

   std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                            "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "
                            "FROM lineitem "
                            "ORDER BY l_shipdate "
                            "LIMIT 10";

    std::shared_ptr<QueryTable> inputTable = DataUtilities::getQueryResults(dbName, inputQuery, false);
    vector<int8_t> tableData = inputTable->serialize();
    ASSERT_EQ(tableData.size(), 620);
    std::shared_ptr<QueryTable> deserialized = QueryTable::deserialize(inputTable->getSchema(), tableData);

    ASSERT_EQ(*inputTable, *deserialized);


}

