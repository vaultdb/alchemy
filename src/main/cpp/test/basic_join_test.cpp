#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <operators/basic_join.h>


using namespace emp;
using namespace vaultdb;


class BasicJoinTest : public ::testing::Test {


protected:
    void SetUp() override { setup_plain_prot(false, ""); };
    void TearDown() override{  finalize_plain_prot(); };

    const std::string dbName = "tpch_unioned";

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


   std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput customerInput(dbName, customerSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);


    ConjunctiveEqualityPredicate customerOrdersOrdinals = {EqualityPredicate(1, 0)}; //  o_custkey, c_custkey

    std::shared_ptr<BinaryPredicate<BoolField> > customerOrdersPredicate(new JoinEqualityPredicate<BoolField>(customerOrdersOrdinals));

    BasicJoin join(&ordersInput, &customerInput, customerOrdersPredicate);


    std::shared_ptr<QueryTable> observed = join.run();


    ASSERT_EQ(*expected, *observed);

}


TEST_F(BasicJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                        "lineitem_cte AS (" + lineitemSql + ") "
                                        "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority,(odummy OR ldummy OR o_orderkey <> l_orderkey) dummy "
                                         "FROM lineitem_cte, orders_cte "
                                          "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority";

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput lineitemInput(dbName, lineitemSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);


    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey

    std::shared_ptr<BinaryPredicate<BoolField> > customerOrdersPredicate(new JoinEqualityPredicate<BoolField>(lineitemOrdersOrdinals));

    BasicJoin<BoolField> joinOp(&lineitemInput, &ordersInput, customerOrdersPredicate);


    std::shared_ptr<QueryTable> observed = joinOp.run();



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

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput customerInput(dbName, customerSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);
    SqlInput lineitemInput(dbName, lineitemSql, true);


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey
    std::shared_ptr<BinaryPredicate<BoolField> > customerOrdersPredicate(new JoinEqualityPredicate<BoolField>(customerOrdersOrdinals));

    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey
    std::shared_ptr<BinaryPredicate<BoolField> > lineitemOrdersPredicate(new JoinEqualityPredicate<BoolField>(lineitemOrdersOrdinals));


    BasicJoin customerOrdersJoin(&ordersInput, &customerInput, customerOrdersPredicate);
    BasicJoin fullJoin(&lineitemInput, &customerOrdersJoin, lineitemOrdersPredicate);


    std::shared_ptr<QueryTable> observed = fullJoin.run();



    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}




