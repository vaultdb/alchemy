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
#include <operators/project.h>

using namespace emp;
using namespace vaultdb::types;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class SortTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};

    std::shared_ptr<Sort> getSort(const std::string & srcSql, const SortDefinition & sortDefinition);

    const std::string dbName = "tpch_alice";

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
    sortDefinition.push_back(aColumnSort);

    std::shared_ptr<Operator> sort = getSort(sql, sortDefinition);
    std::shared_ptr<QueryTable> observed = sort->run();
    ASSERT_EQ(expectedResult, observed->toString());


}

TEST_F(SortTest, tpchQ1Sort) {
    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10) SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);

    std::shared_ptr<Operator> sort = getSort(sql, sortDefinition);
    std::shared_ptr<QueryTable> observed = sort->run();

    // no projection needed here
    ASSERT_EQ(*expected, *observed);

}

TEST_F(SortTest, tpchQ3Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate")  + " FROM input ORDER BY revenue DESC, o_orderdate";
    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);

    std::shared_ptr<Sort> sort = getSort(sql, sortDefinition);


    // project it down to $1, $3
    Project *projectOp = new Project(sort->getPtr());
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(1, 0);
    projectOp->addColumnMapping(3, 1);

    std::shared_ptr<QueryTable> observed = project->run();

    ASSERT_EQ(*expected, *observed);

}


TEST_F(SortTest, tpchQ5Sort) {

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

    std::shared_ptr<Sort> sort = getSort(sql, sortDefinition);


    // project it down to $1
    Project *projectOp = new Project(sort->getPtr());
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(1, 0);

    std::shared_ptr<QueryTable> observed = project->run();

    ASSERT_EQ(*expected, *observed);

}



TEST_F(SortTest, tpchQ8Sort) {

    std::string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);


    std::shared_ptr<Sort> sort = getSort(sql, sortDefinition);
    std::shared_ptr<QueryTable> observed = sort->run();

    ASSERT_EQ(*expected, *observed);
}




TEST_F(SortTest, tpchQ9Sort) {

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT n_name, o_orderyear FROM input ORDER BY n_name, o_orderyear DESC";

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);


    std::shared_ptr<Sort> sort = getSort(sql, sortDefinition);

    // project it down to $0
    Project *projectOp = new Project(sort->getPtr());
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(2, 0);
    projectOp->addColumnMapping(0, 1);

    std::shared_ptr<QueryTable> observed = project->run();

    ASSERT_EQ(*expected, *observed);


}



// 18
TEST_F(SortTest, tpchQ18Sort) {

    std::string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_comment, o_custkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    std:string expectedResultSql = "WITH input AS (" + sql + ") SELECT o_totalprice, " + DataUtilities::queryDatetime("o_orderdate") + "  FROM input ORDER BY o_totalprice DESC, o_orderdate";


    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);


    std::shared_ptr<Sort> sort = getSort(sql, sortDefinition);

    // project it down to $2, $1
    Project *projectOp = new Project(sort->getPtr());
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(2, 0);
    projectOp->addColumnMapping(1, 1);

    std::shared_ptr<QueryTable> observed = project->run();

    ASSERT_EQ(*expected, *observed);


}






std::shared_ptr<Sort> SortTest::getSort(const string &srcSql, const SortDefinition &sortDefinition) {
    std::shared_ptr<Operator> input(new SqlInput(dbName, srcSql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it


    return std::dynamic_pointer_cast<Sort> (sortOp->getPtr());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

