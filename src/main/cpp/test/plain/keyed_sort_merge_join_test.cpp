#include "plain_base_test.h"
#include "util/field_utilities.h"
#include <operators/sql_input.h>
#include <operators/keyed_sort_merge_join.h>
#include <operators/sort.h>

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>



DEFINE_int32(cutoff, 10, "limit clause for queries");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column or compressed)");


class KeyedSortMergeJoinTest :  public PlainBaseTest  {

protected:
 

    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY c_custkey";


    const std::string orders_cust_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                    "FROM orders \n"
                                    "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                    " ORDER BY o_custkey, o_orderkey";

    const std::string orders_lineitem_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                         "FROM orders \n"
                                         "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                         " ORDER BY o_orderkey";


    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= " + std::to_string(FLAGS_cutoff) + ")  \n"
                                     " ORDER BY l_orderkey, revenue, l_dummy ";

    SortDefinition customer_sort_ = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition orders_cust_sort_{ColumnSort (1, SortDirection::ASCENDING), ColumnSort (0, SortDirection::ASCENDING)};
    SortDefinition orders_lineitem_sort_{ColumnSort (0, SortDirection::ASCENDING)};
    SortDefinition lineitem_sort_ = DataUtilities::getDefaultSortDefinition(2);





};




TEST_F(KeyedSortMergeJoinTest, test_tpch_q3_customer_orders) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                          "orders_cte AS (" + orders_cust_sql_ + ") "
                                   "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                   "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                   "WHERE NOT o_dummy AND NOT c_dummy "
                                    "ORDER BY o_orderkey ";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto *orders_input = new SqlInput(db_name_, orders_cust_sql_, true, orders_cust_sort_);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1, customer_input, 4);

    auto join = new KeyedSortMergeJoin(orders_input, customer_input,  predicate);
   auto joined = join->run();
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    Sort<bool> sorter(joined, sort_def);
    PlainTable *observed = sorter.run();
    expected->order_by_ = observed->order_by_;
    DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

    delete expected;


}


TEST_F(KeyedSortMergeJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_lineitem_sql_ + "), "
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (o_dummy OR l_dummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);


    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);
    auto *orders_input = new SqlInput(db_name_, orders_lineitem_sql_, true, orders_lineitem_sort_);

    // output schema: lineitem, orders
    // l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<bool> *predicate  = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0, orders_input,
                                                                              2);

    auto join = new KeyedSortMergeJoin(lineitem_input, orders_input, 0, predicate);
    Sort<bool> sorter(join, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed = sorter.run();

    DataUtilities::removeDummies(observed);
    DataUtilities::removeDummies(expected);
    expected->order_by_ = observed->order_by_;

    ASSERT_EQ(*expected, *observed);

    delete expected;

}



TEST_F(KeyedSortMergeJoinTest, test_tpch_q3_lineitem_orders_customer) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_cust_sql_ + "), \n"
									"lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                    "customer_cte AS (" + customer_sql_ + "),\n "
                                    "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (o_dummy OR l_dummy OR c_dummy) dummy \n"
                                    "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                    "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
									"SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                    "FROM cross_product \n"
                                    "WHERE matched \n"
									"ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);


    auto *customer_input = new SqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto *orders_input = new SqlInput(db_name_, orders_cust_sql_, true, orders_cust_sort_);
    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey

    Expression<bool> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1,
                                                                                             customer_input, 4);
    auto *customer_orders_join = new KeyedSortMergeJoin (orders_input, customer_input, 0, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> *lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0,
                                                                                             customer_orders_join, 2);



    auto *full_join = new KeyedSortMergeJoin(lineitem_input, customer_orders_join, 0, lineitem_orders_predicate);


	SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
    Sort<bool> observed_sort(full_join, sort_def);
    auto observed = observed_sort.run();

    DataUtilities::removeDummies(observed);

    expected->order_by_ = observed->order_by_;
    DataUtilities::removeDummies(expected);



    ASSERT_EQ(*expected, *observed);

    delete expected;

}



TEST_F(KeyedSortMergeJoinTest, test_tpch_q3_customer_orders_reversed) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                             "orders_cte AS (" + orders_cust_sql_ + ") "
                                                                                                             "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,(c_dummy OR o_dummy) dummy "
                                                                                                             "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                             "WHERE NOT c_dummy AND NOT o_dummy "
                                                                                                             "ORDER BY c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority ";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto *orders_input = new SqlInput(db_name_, orders_cust_sql_, true, orders_cust_sort_);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(customer_input, 0, orders_input,
                                                                             2);

    auto join = new KeyedSortMergeJoin(customer_input, orders_input, 1, predicate);
    Sort sorter(join, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed = sorter.run();

    DataUtilities::removeDummies(observed);
    expected->order_by_ = observed->order_by_;


    ASSERT_EQ(*expected, *observed);

    delete expected;


}


TEST_F(KeyedSortMergeJoinTest, test_tpch_q3_lineitem_orders_reversed) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_lineitem_sql_ + "), "
                                                     "lineitem_cte AS (" + lineitem_sql_ + ") "
                                                     " SELECT  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue "
                                                     " FROM orders_cte JOIN lineitem_cte ON l_orderkey = o_orderkey "
                                                     " WHERE NOT o_dummy AND NOT l_dummy "
                                                     " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);


    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);
    auto *orders_input = new SqlInput(db_name_, orders_lineitem_sql_, true, orders_lineitem_sort_);


    // output schema:  orders, lineitem
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    Expression<bool> *predicate  = FieldUtilities::getEqualityPredicate<bool>(orders_input, 0, lineitem_input,
                                                                              4);
    auto join = new KeyedSortMergeJoin(orders_input, lineitem_input, 1, predicate);
    Sort<bool> sorter(join, DataUtilities::getDefaultSortDefinition(6));
    PlainTable *observed = sorter.run();

    DataUtilities::removeDummies(observed);
    expected->order_by_ = observed->order_by_;
	DataUtilities::removeDummies(expected);


    ASSERT_EQ(*expected, *observed);


    delete expected;

}



TEST_F(KeyedSortMergeJoinTest, test_tpch_q3_lineitem_orders_customer_reversed) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_cust_sql_ + "), \n"
									"lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                    "customer_cte AS (" + customer_sql_ + "),\n "
                                    "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (o_dummy OR l_dummy OR c_dummy) dummy \n"
                                    "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                    "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                    "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,  l_orderkey, revenue, dummy \n"
                                    "FROM cross_product \n"
                                    "WHERE matched \n"
									"ORDER BY c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);


    auto *customer_input = new SqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto *orders_input = new SqlInput(db_name_, orders_cust_sql_, true, orders_cust_sort_);
    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority

    Expression<bool> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(customer_input, 0,
                                                                                             orders_input, 2);

    auto customer_orders_join = new KeyedSortMergeJoin(customer_input, orders_input, 1, customer_orders_predicate);

    // join output schema:
    //   c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    Expression<bool> *lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(customer_orders_join, 1,
                                                                                             lineitem_input, 5);



    auto full_join = new KeyedSortMergeJoin(customer_orders_join, lineitem_input, 1, lineitem_orders_predicate);


    PlainTable *observed = full_join->run()->clone();
	SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
    Sort<bool> observed_sort(observed, sort_def);
    observed = observed_sort.run();

	DataUtilities::removeDummies(observed);
    expected->order_by_ = observed->order_by_;

	DataUtilities::removeDummies(expected);

    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete full_join;




}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

