#include "plain/plain_base_test.h"
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"



// DIAGNOSE = 1 --> all tests produce non-empty query result
#define DIAGNOSE 0

class TpcHTest : public PlainBaseTest {


protected:



    // depends on truncate-tpch-set.sql
    // different DBs for different tests to bump up the output size - don't want empty output!
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);

    int input_tuple_limit_ = 1000;


};

void
TpcHTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {
    string query = tpch_queries[test_id];

    if(test_id == 18 && test_name == "q18-truncated") {
        query = truncated_tpch_queries[18]; // 18++
        boost::replace_all(query, "$LIMIT", std::to_string(input_tuple_limit_));

    }


    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, query, false, 0);
    expected->setSortOrder(expected_sort);
    if(DIAGNOSE == 1) {
        ASSERT_GT(expected->getTupleCount(),  0);
    }

    PlanParser<bool> plan_reader(db_name, test_name, 0);
    shared_ptr<PlainOperator> root = plan_reader.getRoot();

    shared_ptr<PlainTable> observed = root->run();
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

}


TEST_F(TpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, "tpch_unioned_50");
}

TEST_F(TpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, "tpch_unioned_50");
}

//join(join(join(customer, orders), lineitem), supplier)
TEST_F(TpcHTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    // to get non-empty results, run with tpch_unioned_1000 - runs for ~40 mins
    // JMR: commented out o_orderdate filter in q5, now able to get results with tpch_unioned_250 instead
    // this filter only affects dummy tags, so it won't alter the runtimes of our queries
    string db_name = "tpch_unioned_250";
    runTest(5, "q5", expected_sort, db_name);
}

TEST_F(TpcHTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, "tpch_unioned_1000");
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(TpcHTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, "tpch_unioned_250");

}




TEST_F(TpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    string db_name = (DIAGNOSE == 1) ? "tpch_unioned" : "tpch_unioned_50";
    string test_name = (DIAGNOSE == 1) ? "q18-truncated" : "q18";

    runTest(18, test_name, expected_sort, db_name);
}

