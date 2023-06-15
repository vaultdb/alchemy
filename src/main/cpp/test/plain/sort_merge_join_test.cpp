#include "plain_base_test.h"
#include "util/field_utilities.h"
#include <operators/sql_input.h>
#include <operators/sort_merge_join.h>
#include <expression/comparator_expression_nodes.h>

DEFINE_string(storage, "row", "storage model for tables (row or column)");

class SortMergeJoinTest :  public PlainBaseTest  {

protected:


    int cutoff_ = 3;

    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey < " + std::to_string(cutoff_) +
                                      " ORDER BY c_custkey";


    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                    "FROM orders \n"
                                    "WHERE o_custkey <  " + std::to_string(cutoff_) +
                                    " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey < " + std::to_string(cutoff_) + ")  \n"
                                                                                                                                          " ORDER BY l_orderkey, revenue ";




};




//TEST_F(SortMergeJoinTest, test_tpch_q3_customer_orders) {
//
//    string customer_sql = "SELECT c_custkey \n"
//                          "FROM customer  \n"
//                          "WHERE c_custkey < " + std::to_string(cutoff_-1) +
//                          " ORDER BY c_custkey";
//
//    string orders_sql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority  \n"
//                              "FROM orders \n"
//                              "WHERE o_custkey <  " + std::to_string(cutoff_) +
//                              " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";
//
//    //PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);
//
//    auto *customer_input = new SqlInput(db_name_, customer_sql, storage_model_, false);
//    auto *orders_input = new SqlInput(db_name_, orders_sql, storage_model_, false);
//
//    std::cout << "LHS: " << customer_input->getOutput()->toString(true) << '\n';
//    std::cout << "RHS: " << orders_input->getOutput()->toString(true) << '\n';
//
//    // join output schema: (orders, customer)
//    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
//    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1, customer_input, 4);
//
//    SortMergeJoin join(orders_input, customer_input, 0, predicate);
//    PlainTable * observed = join.run();
//
//    std::cout << "Observed: " << *observed << endl;
//   // ASSERT_EQ(*expected, *observed);
//
//    //delete expected;
//
//
//}

TEST_F(SortMergeJoinTest, test_tpch_q3_customer_orders) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                          "orders_cte AS (" + orders_sql_ + ") "
                                   "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,(c_dummy OR o_dummy) dummy "
                                   "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                    "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, storage_model_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, storage_model_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1, customer_input, 4);

    SortMergeJoin join(orders_input, customer_input, 0, predicate);
    PlainTable * observed = join.run();

    ASSERT_EQ(*expected, *observed);

    delete expected;


}


TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (o_dummy OR l_dummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);

    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, storage_model_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, storage_model_, true);

    // output schema: lineitem, orders
    // l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<bool> *predicate  = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0, orders_input,
                                                                              2);

    SortMergeJoin join(lineitem_input, orders_input, 0, predicate);

    PlainTable *observed = join.run();

   ASSERT_EQ(*expected, *observed);

    delete expected;

}



// compose C-O-L join should produce one output tuple, order ID 210945
// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders_customer) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                     "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                     "customer_cte AS (" + customer_sql_ + "),\n "
                                     "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (o_dummy OR l_dummy OR c_dummy) dummy \n"
                                     "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                     "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                          "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                          "FROM cross_product \n"
                                          "WHERE matched";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);


    auto *customer_input = new SqlInput(db_name_, customer_sql_, storage_model_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, storage_model_, true);
    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, storage_model_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey

    Expression<bool> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1,
                                                                                             customer_input, 4);
    auto *customer_orders_join = new SortMergeJoin (orders_input, customer_input, 0, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> *lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0,
                                                                                             customer_orders_join, 2);



    SortMergeJoin full_join(lineitem_input, customer_orders_join, 0, lineitem_orders_predicate);


    PlainTable *observed = full_join.run();


    ASSERT_EQ(*expected, *observed);

    delete expected;


}



TEST_F(SortMergeJoinTest, test_tpch_q3_customer_orders_reversed) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                             "orders_cte AS (" + orders_sql_ + ") "
                                                                                                             "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,(c_dummy OR o_dummy) dummy "
                                                                                                             "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, storage_model_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, storage_model_, true);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(customer_input, 0, orders_input,
                                                                             2);

    auto join = new SortMergeJoin(customer_input, orders_input, 1, predicate);
    PlainTable *observed = join->run();

    ASSERT_EQ(*expected, *observed);

    delete expected;


}


TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders_reversed) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (o_dummy OR l_dummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);

    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, storage_model_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, storage_model_, true);

    // output schema: lineitem, orders
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    Expression<bool> *predicate  = FieldUtilities::getEqualityPredicate<bool>(orders_input, 0, lineitem_input,
                                                                              4);

    auto join = new SortMergeJoin(orders_input, lineitem_input, 1, predicate);

    PlainTable *observed = join->run();


    ASSERT_EQ(*expected, *observed);


    delete expected;
    delete join;

}



TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders_customer_reversed) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                                                                                             "customer_cte AS (" + customer_sql_ + "),\n "
                                                                                                                                                 "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (o_dummy OR l_dummy OR c_dummy) dummy \n"
                                                                                                                                                 "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                                                                                                                                 "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                                                                 "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,  l_orderkey, revenue, dummy \n"
                                                                                                                                                 "FROM cross_product \n"
                                                                                                                                                 "WHERE matched";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);


    auto *customer_input = new SqlInput(db_name_, customer_sql_, storage_model_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, storage_model_, true);
    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, storage_model_, true);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority

    Expression<bool> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(customer_input, 0,
                                                                                             orders_input, 2);

    auto customer_orders_join = new SortMergeJoin(customer_input, orders_input, 1, customer_orders_predicate);

    // join output schema:
    //   c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    Expression<bool> *lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(customer_orders_join, 1,
                                                                                             lineitem_input, 5);



    auto full_join = new SortMergeJoin(customer_orders_join, lineitem_input, 1, lineitem_orders_predicate);


    PlainTable *observed = full_join->run();


    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete full_join;




}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

