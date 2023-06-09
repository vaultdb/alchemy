#include "plain/plain_base_test.h"
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"

DEFINE_string(storage, "row", "storage model for tables (row or column)");



class TpcHTest : public PlainBaseTest {


protected:



    // depends on truncate-tpch-set.sql
    // different DBs for different tests to bump up the output size - don't want empty output!
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);

    int input_tuple_limit_ = 0;


};

void
TpcHTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {
    string query = tpch_queries[test_id];

    PlainTable *expected = DataUtilities::getExpectedResults(db_name, query, false, 0, storage_model_);
    expected->setSortOrder(expected_sort);

    PlanParser<bool> plan_reader(db_name, test_name, storage_model_, input_tuple_limit_);
    PlainOperator *root = plan_reader.getRoot();

    PlainTable *observed = root->run();
    DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete root;

}


TEST_F(TpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, db_name_);
}

TEST_F(TpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, db_name_);
}

//join(join(join(customer, orders), lineitem), supplier)
TEST_F(TpcHTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, db_name_);
}

TEST_F(TpcHTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, db_name_);
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(TpcHTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, db_name_);

}




TEST_F(TpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest(18, "q18", expected_sort, db_name_);
}

