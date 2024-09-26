#include "plain_base_test.h"
#include "operators/sql_input.h"
#include "data/csv_reader.h"
#include "parser/plan_parser.h"
#include "test/support/tpch_queries.h"


DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column or compressed)");

class TpcHTest : public PlainBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest(const int &test_id, const string & plan_file, const SortDefinition &expected_sort);

    int input_tuple_limit_ = 0;


};

void
TpcHTest::runTest(const int &test_id, const string & plan_file, const SortDefinition &expected_sort) {
    string expected_sql = tpch_queries[test_id];

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 0);
    expected->order_by_ = expected_sort;

    PlanParser<bool> plan_reader(db_name_,plan_file, input_tuple_limit_, true);
    PlainOperator *root = plan_reader.getRoot();

    PlainTable *observed = root->run();
    DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete root;

}


TEST_F(TpcHTest, tpch_q01) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
   string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q1.json";

    runTest(1, plan_file, expected_sort);
}

TEST_F(TpcHTest, tpch_q03) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};

   string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q3.json";

    runTest(3, plan_file, expected_sort);
}

//join(join(join(customer, orders), lineitem), supplier)
TEST_F(TpcHTest, tpch_q05) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q5.json";

    runTest(5, plan_file, expected_sort);
}

TEST_F(TpcHTest, tpch_q08) {
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q8.json";

    runTest(8, plan_file, expected_sort);
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(TpcHTest, tpch_q09) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
       string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q9.json";

    runTest(9, plan_file, expected_sort);

}




TEST_F(TpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/plain-q18.json";
    runTest(18, plan_file, expected_sort);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;
}

