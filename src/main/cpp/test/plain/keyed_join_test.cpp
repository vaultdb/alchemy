#include "plain_base_test.h"
#include "util/field_utilities.h"
#include <operators/sql_input.h>
#include <operators/keyed_join.h>
#include <expression/comparator_expression_nodes.h>

class KeyedJoinTest :  public PlainBaseTest  {

protected:


    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                                    "FROM customer  "
                                   // "WHERE c_custkey <= 10 "
                                    "ORDER BY c_custkey";

    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                                  "FROM orders "
                                  //"WHERE o_custkey <= 10 "
                                  "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                                    "FROM lineitem "
                                  //  "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= 10)  "
                                    "ORDER BY l_orderkey, revenue ";
};





TEST_F(KeyedJoinTest, test_tpch_q3_customer_orders) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                          "orders_cte AS (" + orders_sql_ + ") "
                                   "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,(cdummy OR odummy) dummy "
                                   "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                    "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SqlInput customer_input(db_name_, customer_sql_, true);
    SqlInput orders_input(db_name_, orders_sql_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<bool> predicate = FieldUtilities::getEqualityPredicate<bool>(1, 4);

    KeyedJoin join(&orders_input, &customer_input, predicate);
    std::shared_ptr<PlainTable > observed = join.run();

    ASSERT_EQ(*expected, *observed);
    predicate.reset();

}


TEST_F(KeyedJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (odummy OR ldummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SqlInput lineitem_input(db_name_, lineitem_sql_, true);
    SqlInput orders_input(db_name_, orders_sql_, true);

    // output schema: lineitem, orders
    // l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority
    BoolExpression<bool> predicate  = FieldUtilities::getEqualityPredicate<bool>(0, 2);

    KeyedJoin join(&lineitem_input, &orders_input, predicate);

    std::shared_ptr<PlainTable > observed = join.run();



    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

    predicate.reset();

}



// compose C-O-L join should produce one output tuple, order ID 210945
// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(KeyedJoinTest, test_tpch_q3_lineitem_orders_customer) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                     "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                     "customer_cte AS (" + customer_sql_ + "),\n "
                                     "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                     "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                     "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                          "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                          "FROM cross_product \n"
                                          "WHERE matched";


    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);


    SqlInput customer_input(db_name_, customer_sql_, true);
    SqlInput orders_input(db_name_, orders_sql_, true);
    SqlInput lineitem_input(db_name_, lineitem_sql_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey

    BoolExpression<bool> customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(1, 4);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<bool> lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(0, 2);


    KeyedJoin customerOrdersJoin(&orders_input, &customer_input, customer_orders_predicate);

    KeyedJoin fullJoin(&lineitem_input, &customerOrdersJoin, lineitem_orders_predicate);


    std::shared_ptr<PlainTable > observed = fullJoin.run();


    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

    customer_orders_predicate.reset();
    lineitem_orders_predicate.reset();


}



TEST_F(KeyedJoinTest, test_tpch_q3_customer_orders_reversed) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                             "orders_cte AS (" + orders_sql_ + ") "
                                                                                                             "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,(cdummy OR odummy) dummy "
                                                                                                             "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SqlInput customer_input(db_name_, customer_sql_, true);
    SqlInput orders_input(db_name_, orders_sql_, true);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority
    BoolExpression<bool> predicate = FieldUtilities::getEqualityPredicate<bool>(0, 2);

    KeyedJoin join(&customer_input, &orders_input, 1, predicate);
    std::shared_ptr<PlainTable > observed = join.run();

    ASSERT_EQ(*expected, *observed);

    predicate.reset();

}


TEST_F(KeyedJoinTest, test_tpch_q3_lineitem_orders_reversed) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (odummy OR ldummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SqlInput lineitem_input(db_name_, lineitem_sql_, true);
    SqlInput orders_input(db_name_, orders_sql_, true);

    // output schema: lineitem, orders
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    BoolExpression<bool> predicate  = FieldUtilities::getEqualityPredicate<bool>(0, 4);

    KeyedJoin join(&orders_input, &lineitem_input, 1, predicate);

    std::shared_ptr<PlainTable > observed = join.run();



    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

    predicate.reset();

}



TEST_F(KeyedJoinTest, test_tpch_q3_lineitem_orders_customer_reversed) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                                                                                             "customer_cte AS (" + customer_sql_ + "),\n "
                                                                                                                                                 "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                                                                                                                                 "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                                                                                                                                 "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                                                                 "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,  l_orderkey, revenue, dummy \n"
                                                                                                                                                 "FROM cross_product \n"
                                                                                                                                                 "WHERE matched";


    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);


    SqlInput customer_input(db_name_, customer_sql_, true);
    SqlInput orders_input(db_name_, orders_sql_, true);
    SqlInput lineitem_input(db_name_, lineitem_sql_, true);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority

    BoolExpression<bool> customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(0, 2);

    // join output schema:
    //   c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    BoolExpression<bool> lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(1, 5);


    KeyedJoin customerOrdersJoin(&customer_input, &orders_input, 1, customer_orders_predicate);

    KeyedJoin fullJoin(&customerOrdersJoin, &lineitem_input, 1, lineitem_orders_predicate);


    std::shared_ptr<PlainTable > observed = fullJoin.run();


    ASSERT_EQ(*expected, *observed);

    customer_orders_predicate.reset();
    lineitem_orders_predicate.reset();

}






