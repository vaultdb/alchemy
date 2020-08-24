//
// Created by Salome Kariuki on 4/29/20.
//
#include "sort.h"

#include <util/emp_manager.h>
#include <operators/secure_sql_input.h>

#include <gflags/gflags.h>


#include "emp-tool/emp-tool.h"
#include "util/data_utilities.h"
#include <gtest/gtest.h>
#include <data/PsqlDataProvider.h>



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");



class SecureSortTest :  public  ::testing::Test  {
protected:
    void SetUp() override {};

    void TearDown() override {

        std::string leadDb =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";
        pqxx::connection c("dbname=" + leadDb);
        pqxx::work w(c);

        w.exec("DROP TABLE tmp");


    };
    void runSortTest(const SortDefinition & sortDefinition, const std::string & sql, const std::string & expectedResult);


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

    std::cout << "Expected output: \n" << expectedResult;

    std::cout << "Observed result: \n" << *revealed << std::endl;
    ASSERT_EQ(revealed->toString(), expectedResult);

    empManager->close();
}


/*
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



} */


// try the logic of Q1 in plaintext for warmup
TEST_F(SecureSortTest, tpchQ1SortClear) {

    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols
    std::string orderBy = "l_returnflag, l_linestatus";
    std::string expectedResult = DataUtilities::getExpectedResults(sql, orderBy, FLAGS_party, "tpch_alice", "tpch_bob")->toString();
    std::cout << "Expected result:\n " << expectedResult << std::endl;

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);


    SqlInput *sqlInputPtr = new SqlInput("tpch_alice", "tpch_bob", sql);
    std::shared_ptr<Operator> input(sqlInputPtr);

    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();
    std::shared_ptr<QueryTable> result = sort->run();

    std::cout << "Observed result: "  << *result << std::endl;
    ASSERT_EQ(result->toString(), expectedResult);

}

TEST_F(SecureSortTest, tpchQ1Sort) {
    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols
    std::string orderBy = "l_returnflag, l_linestatus";
    std::string expectedResult = DataUtilities::getExpectedResults(sql, orderBy, FLAGS_party, "tpch_alice", "tpch_bob")->toString();


    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}

/* TEST_F(SecureSortTest, tpchQ3Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, EXTRACT(epoch FROM o.o_orderdate) o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY l_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols
    std::string orderBy = "revenue DESC, o_orderdate";
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY " + orderBy;
    std::string expectedResult = getExpectedResult(expectedResultSql, false, orderBy)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.columnOrders.emplace_back(3, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}


TEST_F(SecureSortTest, tpchQ5Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY l_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols
    std::string orderBy = " revenue DESC";
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY " + orderBy;
    std::string expectedResult = getExpectedResult(expectedResultSql, false, orderBy)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(1, SortDirection::DESCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}



TEST_F(SecureSortTest, tpchQ8Sort) {

    std::string sql = "SELECT extract(year from o.o_orderdate) as o_year, o_orderkey FROM orders o  ORDER BY o_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols
    std::string orderBy = " o_year, o_orderkey DESC";
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY " + orderBy;  // orderkey DESC needed to align with psql's layout
    std::string expectedResult = getExpectedResult(expectedResultSql, false, orderBy)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}




TEST_F(SecureSortTest, tpchQ9Sort) {

    std::string sql = "SELECT extract(year from o.o_orderdate) as o_year, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY o_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols
                      std::string orderBy = "  n_name ASC, o_year ASC,  o_orderkey";
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY " + orderBy;  // o_orderkey might be needed to make this reproducible

    std::string expectedResult = getExpectedResult(expectedResultSql, false, orderBy)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.columnOrders.emplace_back(0, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}



// 18
TEST_F(SecureSortTest, tpchQ18Sort) {

    std::string sql = "SELECT o_orderkey, EXTRACT(epoch FROM o_orderdate) o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols
                      std::string orderBy = " o_totalprice DESC, o_orderdate";
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY " + orderBy;
    std::string expectedResult = getExpectedResult(expectedResultSql, false, orderBy)->toString();

    SortDefinition sortDefinition;
    sortDefinition.columnOrders.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.columnOrders.emplace_back(1, SortDirection::ASCENDING);


    runSortTest(sortDefinition, sql, expectedResult);

}




*/




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





