#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/basic_join.h>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include <operators/expression/comparator_expression_nodes.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");

using namespace vaultdb;

class SecureBasicJoinTest : public EmpBaseTest {
protected:



    const std::string customerSql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy \n"
                                           "FROM customer  \n"
                                           "WHERE c_custkey < 3 \n"
                                           "ORDER BY c_custkey";

    const std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy \n"
                                  "FROM orders \n"
                                  "WHERE o_custkey < 3 \n"
                                  "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitemSql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy \n"
                                    "FROM lineitem \n"
                                    "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey < 3)  \n"
                                    "ORDER BY l_orderkey, revenue ";




};




TEST_F(SecureBasicJoinTest, test_tpch_q3_customer_orders) {


        std::string expectedResultSql = "WITH customer_cte AS (" + customerSql + "), "
                                                                             "orders_cte AS (" + ordersSql + ") "
                                                                                                             "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR o_custkey <> c_custkey) dummy "
                                                                                                             "FROM customer_cte, orders_cte "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(unionedDb, expectedResultSql, true);

    SecureSqlInput customerInput(dbName, customerSql, true, netio, FLAGS_party);
    SecureSqlInput ordersInput(dbName, ordersSql, true, netio, FLAGS_party);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<emp::Bit> predicate = Utilities::getEqualityPredicate<emp::Bit>(1, 4);

    BasicJoin join(&ordersInput, &customerInput, predicate);

    std::shared_ptr<PlainTable> joinResult = join.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&join, sortDefinition);
    std::shared_ptr<PlainTable> observed = sort.run()->reveal();

    expected->setSortOrder(sortDefinition);

    if(!IGNORE_BOB)
        ASSERT_EQ(*expected, *observed);

}


TEST_F(SecureBasicJoinTest, test_tpch_q3_lineitem_orders) {


// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), \n"
                                                                     "lineitem_cte AS (" + lineitemSql + ") "
                                                                                                         "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority,(odummy OR ldummy OR o_orderkey <> l_orderkey) dummy "
                                                                                                         "FROM lineitem_cte, orders_cte "
                                                                                                         "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority";


    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(unionedDb, expectedResultSql, true);

    SecureSqlInput lineitemInput(dbName, lineitemSql, true, netio, FLAGS_party);
    SecureSqlInput ordersInput(dbName, ordersSql, true, netio, FLAGS_party);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<emp::Bit> predicate = Utilities::getEqualityPredicate<emp::Bit>(0, 2);

    BasicJoin join(&lineitemInput, &ordersInput, predicate);


    std::shared_ptr<SecureTable> joinResult = join.run();
    std::unique_ptr<PlainTable> joinResultDecrypted = joinResult->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&join, sortDefinition);
    std::shared_ptr<PlainTable> observed = sort.run()->reveal();

    expected->setSortOrder(sortDefinition);
    if(!IGNORE_BOB)
        ASSERT_EQ(*expected, *observed);

}



// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(SecureBasicJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                      "lineitem_cte AS (" + lineitemSql + "), "
                                        "customer_cte AS (" + customerSql + ") "
                                         "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR ldummy OR o_orderkey <> l_orderkey OR c_custkey <> o_custkey) dummy "
                                             "FROM lineitem_cte, orders_cte, customer_cte "
                                             "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";

    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(unionedDb, expectedResultSql, true);

    SecureSqlInput customerInput(dbName, customerSql, true, netio, FLAGS_party);
    SecureSqlInput ordersInput(dbName, ordersSql, true, netio, FLAGS_party);
    SecureSqlInput lineitemInput(dbName, lineitemSql, true, netio, FLAGS_party);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<emp::Bit> customer_orders_predicate = Utilities::getEqualityPredicate<emp::Bit>(1, 4);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<emp::Bit> lineitem_orders_predicate = Utilities::getEqualityPredicate<emp::Bit>(0, 2);


    BasicJoin customerOrdersJoin(&ordersInput, &customerInput, customer_orders_predicate);

    BasicJoin fullJoin(&lineitemInput, &customerOrdersJoin, lineitem_orders_predicate);


    std::shared_ptr<PlainTable> joinResult = fullJoin.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&fullJoin, sortDefinition);
    std::shared_ptr<PlainTable> observed = sort.run()->reveal();
    expected->setSortOrder(sortDefinition);

    if(!IGNORE_BOB)
        ASSERT_EQ(*expected, *observed);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


