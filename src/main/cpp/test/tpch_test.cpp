#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/CsvReader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"




using namespace emp;
using namespace vaultdb;


class TpcHTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");

    };
    void TearDown() override{
        finalize_plain_prot();
    };
    // depends on truncate-tpch.sql
    const string db_name_ = "tpch_unioned_25"; // plaintext case first
    // limit input to first N tuples per SQL statement
    int limit_ = 0;

    void runTest(const int &test_id, const SortDefinition &expected_sort);

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
TpcHTest::runTest(const int &test_id, const SortDefinition &expected_sort) {
    string test_name = "q" + std::to_string(test_id);
    string query = tpch_queries[test_id];

    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name_, query, false, 0);
    expected->setSortOrder(expected_sort);

    PlanParser<bool> plan_reader(db_name_, test_name, limit_);
    shared_ptr<PlainOperator> root = plan_reader.getRoot();


    std::cout << "Have execution plan: \n" << *root << std::endl;
    shared_ptr<PlainTable> observed = root->run();
    std::cout << "Output: " << observed->toString(true) << std::endl;
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

}


TEST_F(TpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, expected_sort);
}


// Lhs input has 244 tuples, 102 of which are real.
//Rhs input has 25 tuples, 6 of which are real.
//True tuple count post-join: 20 observed output card: 244
//Lhs input has 982 tuples, 619 of which are real.
//Rhs input has 244 tuples, 20 of which are real.

//Lhs input has 982 tuples, 619 of which are real.
//Rhs input has 244 tuples, 20 of which are real.
//True tuple count post-join: 4

// Test code is producing:
// Lhs input has 25 tuples, 6 of which are real.
//Rhs input has 244 tuples, 102 of which are real.
//True tuple count post-join: 20
//Lhs input has 244 tuples, 20 of which are real.
//Rhs input has 982 tuples, 619 of which are real.
//True tuple count post-join: 0 *** fail!

//Match found on lhs idx=682, rhs idx=168: (4141668, 28237.091797) (dummy=0), (4141668, 10, 796003200, 0, 10) (dummy=0)
//Match found on lhs idx=683, rhs idx=168: (4141668, 29215.691406) (dummy=0), (4141668, 10, 796003200, 0, 10) (dummy=0)
//Match found on lhs idx=684, rhs idx=168: (4141668, 35286.613281) (dummy=0), (4141668, 10, 796003200, 0, 10) (dummy=0)
//Match found on lhs idx=685, rhs idx=168: (4141668, 70303.289062) (dummy=0), (4141668, 10, 796003200, 0, 10) (dummy=0)

TEST_F(TpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, expected_sort);
}


TEST_F(TpcHTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, expected_sort);
}

TEST_F(TpcHTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, expected_sort);
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(TpcHTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, expected_sort);

}



TEST_F(TpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest(18, expected_sort);
}

