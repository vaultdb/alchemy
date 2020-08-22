//
// Created by Jennie Rogers on 8/20/20.
//

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/sort.h>
#include <data/PsqlDataProvider.h>

using namespace emp;
using namespace vaultdb::types;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class SortTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};

    void runSortTest(const SortDefinition & sortDefinition, const std::string & sql, const std::string & expectedResult);
    std::unique_ptr<QueryTable> getExpectedResult(std::string sql, bool dummyTag);
};





TEST_F(SortTest, testSingleIntColumn) {

    std::string sql = "SELECT c_custkey FROM customer ORDER BY c_address LIMIT 10";  // c_address "randomizes" the order
    std::string expectedResult = "(#0 int32 customer.c_custkey) isEncrypted? 0\n"
                                 "(1523)\n"
                                 "(2310)\n"
                                 "(3932)\n"
                                 "(5345)\n"
                                 "(6512)\n"
                                 "(6578)\n"
                                 "(7934)\n"
                                 "(8702)\n"
                                 "(11702)\n"
                                 "(12431)\n";

    SortDefinition sortDefinition;
    ColumnSort aColumnSort(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.push_back(aColumnSort);

    runSortTest(sortDefinition, sql, expectedResult);


}

TEST_F(SortTest, tpchQ1Sort) {
    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10) SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}

TEST_F(SortTest, tpchQ3Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, EXTRACT(epoch FROM o.o_orderdate) o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY revenue DESC, o_orderdate";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.columnOrders.emplace_back(3, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}


TEST_F(SortTest, tpchQ5Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY revenue DESC";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::DESCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}



TEST_F(SortTest, tpchQ8Sort) {

    std::string sql = "SELECT extract(year from o.o_orderdate) as o_year, o_orderkey FROM orders o  ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_year, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}




TEST_F(SortTest, tpchQ9Sort) {

    std::string sql = "SELECT extract(year from o.o_orderdate) as o_year, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY n_name, o_year";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}



// 18
TEST_F(SortTest, tpchQ18Sort) {

    std::string sql = "SELECT o_orderkey, EXTRACT(epoch FROM o_orderdate) o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_totalprice DESC, o_orderdate";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}






void SortTest::runSortTest(const SortDefinition & sortDefinition, const std::string & sql, const std::string & expectedResult) {
    std::string dbName =  "tpch_alice";


    std::shared_ptr<Operator> input(new SqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();

    std::shared_ptr<QueryTable> result = sort->run();

    std::cout << *result << std::endl;

    ASSERT_EQ(result->toString(), expectedResult);

}


std::unique_ptr<QueryTable> SortTest::getExpectedResult(std::string sql, bool dummyTag) {

    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable("tpch_alice", sql, dummyTag);

}