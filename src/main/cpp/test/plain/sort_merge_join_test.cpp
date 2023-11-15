#include "plain_base_test.h"
#include <operators/sort.h>

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>

#include <test/support/tpch_queries.h>
#include <operators/sql_input.h>
#include <operators/sort_merge_join.h>
#include "util/field_utilities.h"


DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class SortMergeJoinTest :  public PlainBaseTest  {

protected:
 
// simulating TPC-H Q5
    const std::string customer_sql_ = "SELECT c_custkey, c_nationkey, r_name = 'EUROPE' c_dummy \n"
                                      "FROM customer  JOIN nation ON c_nationkey = n_nationkey"
                                      "     JOIN region ON r_regionkey = n_regionkey \n"
                                      " ORDER BY c_custkey "
                                      "LIMIT " + std::to_string(FLAGS_cutoff);


    const std::string supplier_sql_ = "SELECT s_suppkey, s_nationkey, r_name = 'EUROPE' s_dummy \n"
                                      "FROM supplier  JOIN nation ON c_nationkey = n_nationkey"
                                      "     JOIN region ON r_regionkey = n_regionkey \n"
                                      " ORDER BY s_suppkey "
                                      "LIMIT " + std::to_string(FLAGS_cutoff);

    SortDefinition customer_sort_ = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition supplier_sort_ = DataUtilities::getDefaultSortDefinition(1);
    // TODO: check this - will depend on operator implementation
    SortDefinition customer_supplir_sort_ = {{0, SortDirection::ASCENDING}, {2, SortDirection::ASCENDING}};


};


TEST_F(SortMergeJoinTest, test_tpch_q3_customer_orders) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                     "supplier_cte AS (" + supplier_sql_ + ") "
                               "SELECT * "
                               "FROM  customer_cte JOIN supplier_cte ON c_nationkey = s_nationkey "
                               "WHERE NOT c_dummy AND NOT s_dummy "
                               "ORDER BY c_custkey ";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    auto customer_input = new SqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto supplier_input = new SqlInput(db_name_, supplier_sql_, true, supplier_sort_);

    // join output schema: (customer, supplier)
    // c_custkey, c_nationkey, s_suppkey, s_nationkey
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(customer_input, 0, supplier_input, 2);

    auto join = new SortMergeJoin<bool>(customer_input, supplier_input,  predicate);
    auto joined = join->run();
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    Sort<bool> sorter(joined, sort_def);
    PlainTable *observed = sorter.run();
    expected->order_by_ = observed->order_by_;
    DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

    delete expected;


}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
