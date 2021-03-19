#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <operators/fkey_pkey_join.h>


using namespace emp;
using namespace vaultdb;


class ForeignKeyPrimaryKeyJoinTest : public ::testing::Test {


protected:
    void SetUp() override { setup_plain_prot(false, ""); };
    void TearDown() override{  finalize_plain_prot(); };

    const std::string dbName = "tpch_unioned";

    const std::string customerSql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                                    "FROM customer  "
                                    "WHERE c_custkey <= 5 "
                                    "ORDER BY c_custkey";

    const std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                                  "FROM orders "
                                  "WHERE o_custkey <= 5 "
                                  "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitemSql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                                    "FROM lineitem "
                                    "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= 5)  "
                                    "ORDER BY l_orderkey, revenue ";
};





TEST_F(ForeignKeyPrimaryKeyJoinTest, test_tpch_q3_customer_orders) {

    std::string expectedResultSql = "WITH customer_cte AS (" + customerSql + "), "
                                          "orders_cte AS (" + ordersSql + ") "
                                   "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,(cdummy OR odummy) dummy "
                                   "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                    "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    std::shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput customerInput(dbName, customerSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey
    std::shared_ptr<BinaryPredicate<BoolField> > customerOrdersPredicate(new JoinEqualityPredicate<BoolField> (customerOrdersOrdinals));

    KeyedJoin join(&ordersInput, &customerInput, customerOrdersPredicate);

    std::shared_ptr<QueryTable<BoolField> > observed = join.run();


    ASSERT_EQ(*expected, *observed);

}


TEST_F(ForeignKeyPrimaryKeyJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                                                         "lineitem_cte AS (" + lineitemSql + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (odummy OR ldummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    std::shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput lineitemInput(dbName, lineitemSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);


    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey
    std::shared_ptr<BinaryPredicate<BoolField> > lineitemOrdersPredicate(new JoinEqualityPredicate<BoolField> (lineitemOrdersOrdinals));

    KeyedJoin join(&lineitemInput, &ordersInput, lineitemOrdersPredicate);

    std::shared_ptr<QueryTable<BoolField> > observed = join.run();



    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}



// compose C-O-L join should produce one output tuple, order ID 210945
// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(ForeignKeyPrimaryKeyJoinTest, test_tpch_q3_lineitem_orders_customer) {


    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), \n"
                                     "lineitem_cte AS (" + lineitemSql + "), \n"
                                     "customer_cte AS (" + customerSql + "),\n "
                                     "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                     "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                     "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                          "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                          "FROM cross_product \n"
                                          "WHERE matched";


    std::shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);


    SqlInput customerInput(dbName, customerSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);
    SqlInput lineitemInput(dbName, lineitemSql, true);


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey
    std::shared_ptr<BinaryPredicate<BoolField> > customerOrdersPredicate(new JoinEqualityPredicate<BoolField>(customerOrdersOrdinals));

    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey
    std::shared_ptr<BinaryPredicate<BoolField> > lineitemOrdersPredicate(new JoinEqualityPredicate<BoolField> (lineitemOrdersOrdinals));


    KeyedJoin customerOrdersJoin(&ordersInput, &customerInput, customerOrdersPredicate);

    KeyedJoin fullJoin(&lineitemInput, &customerOrdersJoin, lineitemOrdersPredicate);


    std::shared_ptr<QueryTable<BoolField> > observed = fullJoin.run();



    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}





