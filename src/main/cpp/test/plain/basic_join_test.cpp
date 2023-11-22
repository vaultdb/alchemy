#include <operators/sql_input.h>
#include <operators/basic_join.h>
#include "plain_base_test.h"
#include "util/field_utilities.h"
#include <gflags/gflags.h>

DEFINE_int32(cutoff, 5, "limit clause for queries");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column or compressed)");

class BasicJoinTest : public PlainBaseTest {


protected:

    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                                    "FROM customer  "
                                    "WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) + " "
                                    "ORDER BY c_custkey ";

    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                                  "FROM orders "
                                  "WHERE o_custkey <= " + std::to_string(FLAGS_cutoff) + " "
                                  "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority ";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                                    "FROM lineitem "
                                    "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= " + std::to_string(FLAGS_cutoff) + ")  "
                                    "ORDER BY l_orderkey, revenue ";
};





TEST_F(BasicJoinTest, test_tpch_q3_customer_orders) {

    // get inputs from local oblivious ops
    // first 5 customers, propagate this constraint up the join tree for the test

   std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                        "orders_cte AS (" + orders_sql_ + ") "
                                        "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR o_custkey <> c_custkey) dummy "
                                        "FROM customer_cte, orders_cte "
                                        "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


   PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql,true);

    SqlInput *customer_input = new SqlInput(db_name_, customer_sql_, true);
    SqlInput *orders_input = new SqlInput(db_name_, orders_sql_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> * customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1,  customer_input, 4);

    BasicJoin<bool> join(orders_input, customer_input, customer_orders_predicate);
    PlainTable  *observed = join.run();



    ASSERT_EQ(*expected, *observed);

    delete expected;

}


TEST_F(BasicJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                    "lineitem_cte AS (" + lineitem_sql_ + ") "
                                 "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority,(odummy OR ldummy OR o_orderkey <> l_orderkey) dummy "
                                 "FROM lineitem_cte, orders_cte "
                                 "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority";

    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SqlInput *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true);
    SqlInput *orders_input = new SqlInput(db_name_, orders_sql_, true);


    // output schema: lineitem, orders
    // l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<bool> * predicate = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0, orders_input, 2);

    BasicJoin<bool> join(lineitem_input, orders_input, predicate);


    PlainTable *observed = join.run();

    ASSERT_EQ(*expected, *observed);

    delete expected;
}



// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(BasicJoinTest, test_tpch_q3_lineitem_orders_customer) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                          "lineitem_cte AS (" + lineitem_sql_ + "), "
                                           "customer_cte AS (" + customer_sql_ + ") "
                                                 "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR ldummy OR o_orderkey <> l_orderkey OR c_custkey <> o_custkey) dummy "
                                                 "FROM lineitem_cte JOIN orders_cte ON o_orderkey = l_orderkey"
                                                 "     JOIN customer_cte ON c_custkey = o_custkey "
                                                 "WHERE NOT (cdummy OR odummy OR ldummy) "
                                                 "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";

    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SqlInput *customer_input = new SqlInput(db_name_, customer_sql_, true);
    SqlInput *orders_input = new SqlInput(db_name_, orders_sql_, true);
    SqlInput *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> * customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1, customer_input, 4);
    BasicJoin<bool> *customer_orders_join = new BasicJoin(orders_input, customer_input, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0, customer_orders_join, 2);
    BasicJoin full_join(lineitem_input, customer_orders_join, lineitem_orders_predicate);


    PlainTable *observed = full_join.run();
    DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);
    delete expected;

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;	
    int i =  RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;
}


