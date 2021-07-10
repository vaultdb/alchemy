#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/basic_join.h>
#include <util/utilities.h>

using namespace emp;
using namespace vaultdb;


class BasicJoinTest : public ::testing::Test {


protected:
    void SetUp() override { setup_plain_prot(false, ""); };
    void TearDown() override{  finalize_plain_prot(); };

    const std::string dbName = "tpch_unioned";

    // SELECT c_custkey,  (c_mktsegment != 'HOUSEHOLD') cdummy
    // FROM customer
    // ORDER BY c_custkey;
    // input is equal to all tuples in table

    const std::string customerSql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                                    "FROM customer  "
                                    "WHERE c_custkey <= 5 "
                                    "ORDER BY c_custkey ";

    const std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                                  "FROM orders "
                                  "WHERE o_custkey <= 5 "
                                  "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority ";

    const std::string lineitemSql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                                    "FROM lineitem "
                                    "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= 5)  "
                                    "ORDER BY l_orderkey, revenue ";
};





TEST_F(BasicJoinTest, test_tpch_q3_customer_orders) {

    // get inputs from local oblivious ops
    // first 5 customers, propagate this constraint up the join tree for the test

   std::string expectedResultSql = "WITH customer_cte AS (" + customerSql + "), "
                                        "orders_cte AS (" + ordersSql + ") "
                                        "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR o_custkey <> c_custkey) dummy "
                                        "FROM customer_cte, orders_cte "
                                        "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


   std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput customerInput(dbName, customerSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<bool> customer_orders_predicate = Utilities::getEqualityPredicate<bool>(1, 4);

    BasicJoin<bool> join(&ordersInput, &customerInput, customer_orders_predicate);


    std::shared_ptr<PlainTable > observed = join.run();


    ASSERT_EQ(*expected, *observed);

}


TEST_F(BasicJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                        "lineitem_cte AS (" + lineitemSql + ") "
                                        "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority,(odummy OR ldummy OR o_orderkey <> l_orderkey) dummy "
                                         "FROM lineitem_cte, orders_cte "
                                          "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority";

    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput lineitemInput(dbName, lineitemSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);


    // output schema: lineitem, orders
    // l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority
    BoolExpression<bool> predicate = Utilities::getEqualityPredicate<bool>(0, 2);


    BasicJoin<bool> joinOp(&lineitemInput, &ordersInput, predicate);


    std::shared_ptr<PlainTable > observed = joinOp.run();



    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}



// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(BasicJoinTest, test_tpch_q3_lineitem_orders_customer) {


    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                          "lineitem_cte AS (" + lineitemSql + "), "
                                           "customer_cte AS (" + customerSql + ") "
                                                 "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR ldummy OR o_orderkey <> l_orderkey OR c_custkey <> o_custkey) dummy "
                                                 "FROM lineitem_cte, orders_cte, customer_cte "
                                                 "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";

    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput customerInput(dbName, customerSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);
    SqlInput lineitemInput(dbName, lineitemSql, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<bool> customer_orders_predicate = Utilities::getEqualityPredicate<bool>(1, 4);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<bool> lineitem_orders_predicate = Utilities::getEqualityPredicate<bool>(0, 2);



    BasicJoin customerOrdersJoin(&ordersInput, &customerInput, customer_orders_predicate);
    BasicJoin fullJoin(&lineitemInput, &customerOrdersJoin, lineitem_orders_predicate);


    std::shared_ptr<PlainTable > observed = fullJoin.run();



    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}




