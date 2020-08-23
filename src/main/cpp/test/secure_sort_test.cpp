//
// Created by Salome Kariuki on 4/29/20.
//
#include "sort.h"
#include <util/emp_manager.h>
#include <operators/secure_sql_input.h>

#include <gflags/gflags.h>


#include "emp-tool/emp-tool.h"
#include <gtest/gtest.h>
#include <data/PsqlDataProvider.h>

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");

using namespace emp;
using namespace std;




class SecureSortTest : public ::testing::Test {
protected:
    void SetUp() override {};

    void TearDown() override {};
    void runSortTest(const SortDefinition & sortDefinition, const std::string & sql, const std::string & expectedResult);

    std::unique_ptr<QueryTable> getExpectedResult(std::string sql, bool dummyTag);

};

void
SecureSortTest::runSortTest(const SortDefinition &sortDefinition, const string &sql, const string &expectedResult) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();
    std::shared_ptr<QueryTable> result = sort->run();
    std::unique_ptr<QueryTable> revealed = result->reveal(emp::PUBLIC);


    ASSERT_EQ(revealed->toString(), expectedResult);

    empManager->close();
}

/*    // void testSingleIntColumn();
TEST_F(SecureSortTest,  testSingleIntColumn) {
    std::string sql = "SELECT c_custkey FROM customer ORDER BY c_address LIMIT 5";  // c_address "randomizes" the order
    std::string expectedResult = "(#0 int32 customer.c_custkey) isEncrypted? 0\n"
                                 "(768)\n"
                                 "(1523)\n"
                                 "(2310)\n"
                                 "(2702)\n"
                                 "(3932)\n"
                                 "(6214)\n"
                                 "(6386)\n"
                                 "(6578)\n"
                                 "(7283)\n"
                                 "(11702)\n";

    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    SortDefinition sortDefinition;
    ColumnSort aColumnSort(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.push_back(aColumnSort);

    runSortTest(sortDefinition, sql, expectedResult);


} */


TEST_F(SecureSortTest,  testTwoIntColumns) {
    std::string sql = "SELECT o_orderkey, o_custkey FROM orders ORDER BY o_comment LIMIT 5";  // o_comment "randomizes" the order

    std::string expectedResult = "(#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey) isEncrypted? 0\n"
                                 "(195488, 10849)\n"
                                 "(206023, 11525)\n"
                                 "(222786, 985)\n"
                                 "(265414, 11266)\n"
                                 "(317728, 8138)\n"
                                 "(416930, 3689)\n"
                                 "(466049, 1877)\n"
                                 "(479106, 2077)\n"
                                 "(527522, 1235)\n"
                                 "(545762, 6976)\n";



    SortDefinition sortDefinition;
    sortDefinition.columnOrders.push_back(ColumnSort(0, SortDirection::ASCENDING));
    sortDefinition.columnOrders.push_back(ColumnSort(1, SortDirection::ASCENDING));


    runSortTest(sortDefinition, sql, expectedResult);



}

TEST_F(SecureSortTest, tpchQ1Sort) {
    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10) SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}

TEST_F(SecureSortTest, tpchQ3Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, EXTRACT(epoch FROM o.o_orderdate) o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY revenue DESC, o_orderdate";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.columnOrders.emplace_back(3, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}


TEST_F(SecureSortTest, tpchQ5Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY revenue DESC";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::DESCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}



TEST_F(SecureSortTest, tpchQ8Sort) {

    std::string sql = "SELECT extract(year from o.o_orderdate) as o_year, o_orderkey FROM orders o  ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_year, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}




TEST_F(SecureSortTest, tpchQ9Sort) {

    std::string sql = "SELECT extract(year from o.o_orderdate) as o_year, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY n_name ASC, o_year ASC,  o_orderkey";  // o_orderkey might be needed to make this reproducible

    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}



// 18
TEST_F(SecureSortTest, tpchQ18Sort) {

    std::string sql = "SELECT o_orderkey, EXTRACT(epoch FROM o_orderdate) o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_totalprice DESC, o_orderdate";
    std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}







std::unique_ptr<QueryTable> SecureSortTest::getExpectedResult(std::string sql, bool dummyTag) {

    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable("tpch_alice", sql, dummyTag);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


