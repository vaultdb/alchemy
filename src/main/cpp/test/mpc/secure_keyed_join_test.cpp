#include <gtest/gtest.h>
#include <stdexcept>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include <operators/keyed_join.h>
#include <expression/comparator_expression_nodes.h>

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43442, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");

using namespace vaultdb;

class SecureKeyedJoinTest : public EmpBaseTest {
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




TEST_F(SecureKeyedJoinTest, test_tpch_q3_customer_orders) {


    std::string expectedResultSql = "WITH customer_cte AS (" + customerSql + "), "
                                                                             "orders_cte AS (" + ordersSql + ") "
                                                                                                             "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,(cdummy OR odummy) dummy "
                                                                                                             "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(unioned_db_, expectedResultSql, true);


    SortDefinition  cust_sort = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition  orders_sort = DataUtilities::getDefaultSortDefinition(4);
    SecureSqlInput customerInput(db_name_, customerSql, true, cust_sort, netio_, FLAGS_party);
    SecureSqlInput ordersInput(db_name_, ordersSql, true, orders_sort, netio_, FLAGS_party);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<emp::Bit> predicate = Utilities::getEqualityPredicate<emp::Bit>(1, 4);


    KeyedJoin join(&ordersInput, &customerInput, predicate);
    std::shared_ptr<SecureTable> join_result = join.run();

    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(join_result->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&join, sortDefinition);
    shared_ptr<PlainTable> observed = sort.run()->reveal();
    expected->setSortOrder(sortDefinition);

        ASSERT_EQ(*expected, *observed);


}


TEST_F(SecureKeyedJoinTest, test_tpch_q3_lineitem_orders) {


// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                                                         "lineitem_cte AS (" + lineitemSql + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (odummy OR ldummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte )\n"
                                                                                                             "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";

    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(unioned_db_, expectedResultSql, true);

    SecureSqlInput lineitemInput(db_name_, lineitemSql, true, netio_, FLAGS_party);
    SecureSqlInput ordersInput(db_name_, ordersSql, true, netio_, FLAGS_party);


    // join output schema:
    //  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue,
    BoolExpression<emp::Bit> predicate = Utilities::getEqualityPredicate<emp::Bit>(0, 4);


    // test pkey-fkey join
    KeyedJoin join(&ordersInput, &lineitemInput, 1,  predicate);


    std::shared_ptr<SecureTable> joinResult = join.run();
    std::unique_ptr<PlainTable> observed = joinResult->reveal();

    size_t sort_column_cnt = joinResult->getSchema()->getFieldCount();
    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(sort_column_cnt);


        ASSERT_EQ(*expected, *observed);


}




// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(SecureKeyedJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), \n"
                                                                         "lineitem_cte AS (" + lineitemSql + "), \n"
                                                                                                             "customer_cte AS (" + customerSql + "),\n "
                                                                                                                                                 "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                                                                                                                                 "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                                                                                                                                 "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                                                                 "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                                                                                                                                 "FROM cross_product \n"
                                                                                                                                                 "WHERE matched";

    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(unioned_db_, expectedResultSql, true);

    SecureSqlInput customerInput(db_name_, customerSql, true, netio_, FLAGS_party);
    SecureSqlInput lineitemInput(db_name_, lineitemSql, true, netio_, FLAGS_party);
    SecureSqlInput ordersInput(db_name_, ordersSql, true, netio_, FLAGS_party);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<emp::Bit> customer_orders_predicate = Utilities::getEqualityPredicate<emp::Bit>(1, 4);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    BoolExpression<emp::Bit> lineitem_orders_predicate = Utilities::getEqualityPredicate<emp::Bit>(0, 2);


    KeyedJoin customerOrdersJoin(&ordersInput, &customerInput, customer_orders_predicate);

    KeyedJoin fullJoin(&lineitemInput, &customerOrdersJoin, lineitem_orders_predicate);


    std::shared_ptr<PlainTable> joinResult = fullJoin.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&fullJoin, sortDefinition);
    std::shared_ptr<PlainTable> observed = sort.run()->reveal();
    expected->setSortOrder(sortDefinition);

        ASSERT_EQ(*expected, *observed);


}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


