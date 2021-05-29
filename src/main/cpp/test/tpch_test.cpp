#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/CsvReader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"




using namespace emp;
using namespace vaultdb;

// DIAGNOSE = 1 --> all tests produce non-empty output
#define DIAGNOSE 1

class TpcHTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");

    };
    void TearDown() override{
        finalize_plain_prot();
    };
    // depends on truncate-tpch-set.sql
    //const string db_name_ = "tpch_unioned_250"; // plaintext case first
    // different DBs for different tests to bump up the output size - don't want empty output!
    void runTest(const int &test_id, const SortDefinition &expected_sort, const string &db_name);

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
TpcHTest::runTest(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
    string test_name = "q" + std::to_string(test_id);
    string query = tpch_queries[test_id];


    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, query, false, 0);
    expected->setSortOrder(expected_sort);
    if(DIAGNOSE == 1) {
        ASSERT_GT(expected->getTupleCount(),  0);
    }

    PlanParser<bool> plan_reader(db_name, test_name, 0);
    shared_ptr<PlainOperator> root = plan_reader.getRoot();


    std::cout << "Have execution plan: \n" << *root << std::endl;
    shared_ptr<PlainTable> observed = root->run();

    std::cout << "Expected output: " << *expected << std::endl;

    std::cout << "Output: " << observed->toString(false) << std::endl;
    observed = DataUtilities::removeDummies(observed);


    ASSERT_EQ(*expected, *observed);

}


TEST_F(TpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, expected_sort, "tpch_unioned_50");
}


TEST_F(TpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, expected_sort, "tpch_unioned_50");
}


TEST_F(TpcHTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    // to get non-empty results, run with tpch_unioned_1000 - runs for ~40 mins
    string db_name = (DIAGNOSE == 1) ? "tpch_unioned_1000" : "tpch_unioned_50";
    runTest(5, expected_sort, db_name);
}

// GB agg expected 2 input tuples:
//  o_year |   volume   |         n_nation
//--------+------------+---------------------------
//   1995 | 30585.1476 | GERMANY
//   1996 | 19327.8852 | SAUDI ARABIA

TEST_F(TpcHTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, expected_sort, "tpch_unioned_1000");
}
/* testing Q8 joins
TEST_F(TpcHTest, tpch_q8_op5) {
    string expected_query = "WITH lhs AS (SELECT l_partkey, l_orderkey, l_suppkey, l_extendedprice * (1.0 - l_discount) volume\n"
                 "FROM part JOIN lineitem ON p_partkey = l_partkey\n"
                 "WHERE p_type = 'PROMO BRUSHED COPPER'\n"
                 "ORDER BY l_orderkey),"
                 "rhs AS (SELECT s_suppkey, CASE WHEN n_name = 'EGYPT' THEN 1.0 ELSE 0.0 END AS nation_check\n"
                            "FROM supplier JOIN nation ON n_nationkey = s_nationkey\n"
                            "ORDER BY s_suppkey) "
                            "SELECT * FROM lhs JOIN rhs ON l_suppkey = s_suppkey  ORDER BY l_orderkey";

    string db_name = "tpch_unioned_1000";
    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    SortDefinition expected_sort{ColumnSort(1, SortDirection::ASCENDING)};
    expected->setSortOrder(expected_sort);

    PlanParser<bool> plan_reader(db_name, "q8", 0);


    shared_ptr<PlainOperator> join = plan_reader.getOperator(5);
    shared_ptr<PlainTable> observed = join->run();
    observed = DataUtilities::removeDummies(observed);


    ASSERT_EQ(*expected, *observed);




}

TEST_F(TpcHTest, tpch_q8_op2) {
    string expected_query = "WITH lhs AS (SELECT c_nationkey, c_custkey\n"
                            "FROM customer c JOIN nation n1 ON c_nationkey = n_nationkey\n"
                            "                JOIN region ON r_regionkey = n_regionkey\n"
                            "WHERE r_name = 'MIDDLE EAST' \n"
                            "ORDER BY c_custkey),"
                            "rhs AS (SELECT o_orderkey, o_custkey, o_orderyear::INT AS o_year\n" // , NOT (o_orderdate >= DATE '1995-01-01' AND o_orderdate <= DATE '1996-12-31') AS dummy_tag
                            "FROM orders\n"
                            " WHERE (o_orderdate >= DATE '1995-01-01' AND o_orderdate <= DATE '1996-12-31') "
                            "ORDER BY o_custkey, o_orderkey) "
                            "SELECT * FROM lhs JOIN rhs ON c_custkey = o_custkey ORDER BY o_custkey, o_orderkey";

    string db_name = "tpch_unioned_1000";
    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    SortDefinition expected_sort{ColumnSort(3, SortDirection::ASCENDING), ColumnSort(2, SortDirection::ASCENDING)};
    expected->setSortOrder(expected_sort);


    PlanParser<bool> plan_reader(db_name, "q8", 0);


    shared_ptr<PlainOperator> join = plan_reader.getOperator(2);
    shared_ptr<PlainTable> observed = join->run();
    observed = DataUtilities::removeDummies(observed);


    ASSERT_EQ(*expected, *observed);




}

// l_orderkey = o_orderkey
// lhs: "op2": custkey, rhs: "op5": suppkey
TEST_F(TpcHTest, tpch_q8_op6) {
    string expected_query = "SELECT c_nationkey, c_custkey, o_orderkey, o_custkey, o_orderyear::INT o_year, l_partkey, l_orderkey, l_suppkey,  l_extendedprice * (1.0 - l_discount)::NUMERIC volume, s_suppkey, CASE WHEN n2.n_name = 'EGYPT' THEN 1.0 ELSE 0.0 END AS nation_check\n"
                            "from\n"
                            "part p,\n"
                            "supplier s,\n"
                            "lineitem l,\n"
                            "orders o,\n"
                            "customer c,\n"
                            "nation n1,\n"
                            "nation n2,\n"
                            "region r\n"
                            "    where\n"
                            "      p.p_partkey = l.l_partkey\n"
                            "      and s.s_suppkey = l.l_suppkey\n"
                            "      and l.l_orderkey = o.o_orderkey\n"
                            "      and o.o_custkey = c.c_custkey\n"
                            "      and c.c_nationkey = n1.n_nationkey\n"
                            "      and n1.n_regionkey = r.r_regionkey\n"
                            "      and r.r_name = 'MIDDLE EAST'\n"
                            "      and s.s_nationkey = n2.n_nationkey\n"
                            "      and o.o_orderdate between date '1995-01-01' and date '1996-12-31'\n"
                            "      and p.p_type = 'PROMO BRUSHED COPPER'"
                            "ORDER BY l_orderkey";

    string db_name = "tpch_unioned_1000";
    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    SortDefinition expected_sort{ColumnSort(6, SortDirection::ASCENDING)};
    expected->setSortOrder(expected_sort);


    PlanParser<bool> plan_reader(db_name, "q8", 0);


    shared_ptr<PlainOperator> join = plan_reader.getOperator(6);

    shared_ptr<PlainTable> observed = join->run();
    observed = DataUtilities::removeDummies(observed);
    ASSERT_EQ(*expected, *observed);


}
*/


// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(TpcHTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, expected_sort, "tpch_unioned_250");

}


// TODO: still has empty output
TEST_F(TpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    string db_name = (DIAGNOSE == 1) ? "tpch_unioned_10000" : "tpch_unioned_50";

    runTest(18, expected_sort, db_name);
}

