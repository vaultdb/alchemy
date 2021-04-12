#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/zk/zk_base_test.h>
#include <operators/fkey_pkey_join.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");

using namespace vaultdb;

class ZkKeyedJoinTest : public ZkTest {
protected:



    const std::string customer_sql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy \n"
                                     "FROM customer  \n"
                                     "WHERE c_custkey < 3 \n"
                                     "ORDER BY c_custkey";

    const std::string orders_sql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy \n"
                                   "FROM orders \n"
                                   "WHERE o_custkey < 3 \n"
                                   "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitem_sql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy \n"
                                     "FROM lineitem \n"
                                     "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey < 3)  \n"
                                     "ORDER BY l_orderkey, revenue ";




};




TEST_F(ZkKeyedJoinTest, test_tpch_q3_customer_orders) {


    std::string expectedResultSql = "WITH customer_cte AS (" + customer_sql + "), "
                                                                              "orders_cte AS (" + orders_sql + ") "
                                                                                                               "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR o_custkey <> c_custkey) dummy "
                                                                                                               "FROM customer_cte, orders_cte "
                                                                                                               "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(alice_db, expectedResultSql, true);

    shared_ptr<SecureTable> customer_input = ZkTest::secret_share_input(customer_sql, true);
    shared_ptr<SecureTable> orders_input  = ZkTest::secret_share_input(orders_sql, true);


    ConjunctiveEqualityPredicate customer_orders_ordinals;
    customer_orders_ordinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey
    std::shared_ptr<BinaryPredicate<emp::Bit> > customer_orders_predicate(new JoinEqualityPredicate<emp::Bit>(customer_orders_ordinals));

    KeyedJoin join(orders_input, customer_input, customer_orders_predicate);

    std::shared_ptr<PlainTable> joinResult = join.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&join, sortDefinition);
    std::shared_ptr<PlainTable> observed = sort.run()->reveal();

    expected->setSortOrder(sortDefinition);
    ASSERT_EQ(*expected, *observed);

}


TEST_F(ZkKeyedJoinTest, test_tpch_q3_lineitem_orders) {


// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
    std::string expectedResultSql = "WITH orders_cte AS (" + orders_sql + "), \n"
                                                                          "lineitem_cte AS (" + lineitem_sql + ") "
                                                                                                               "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority,(odummy OR ldummy OR o_orderkey <> l_orderkey) dummy "
                                                                                                               "FROM lineitem_cte, orders_cte "
                                                                                                               "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority";


    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(alice_db, expectedResultSql, true);

    shared_ptr<SecureTable> lineitem_input = ZkTest::secret_share_input(lineitem_sql, true);
    shared_ptr<SecureTable> orders_input  = ZkTest::secret_share_input(orders_sql, true);

    ConjunctiveEqualityPredicate lineitem_orders_ordinals;
    lineitem_orders_ordinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey
    std::shared_ptr<BinaryPredicate<emp::Bit> > lineitem_orders_predicate(new JoinEqualityPredicate<emp::Bit>(lineitem_orders_ordinals));


    KeyedJoin join(lineitem_input, orders_input, lineitem_orders_predicate);


    std::shared_ptr<SecureTable> joinResult = join.run();
    std::unique_ptr<PlainTable> joinResultDecrypted = joinResult->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&join, sortDefinition);
    std::shared_ptr<PlainTable> observed = sort.run()->reveal();

    expected->setSortOrder(sortDefinition);
    ASSERT_EQ(observed->toString(true), expected->toString(true));
    ASSERT_EQ(*expected, *observed);

}



// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(ZkKeyedJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string expected_result_sql = "WITH orders_cte AS (" + orders_sql + "), "
                                                                            "lineitem_cte AS (" + lineitem_sql + "), "
                                                                                                                 "customer_cte AS (" + customer_sql + ") "
                                                                                                                                                      "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, (cdummy OR odummy OR ldummy OR o_orderkey <> l_orderkey OR c_custkey <> o_custkey) dummy "
                                                                                                                                                      "FROM lineitem_cte, orders_cte, customer_cte "
                                                                                                                                                      "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";

    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(alice_db, expected_result_sql, true);

    shared_ptr<SecureTable> customer_input = ZkTest::secret_share_input(customer_sql, true);
    shared_ptr<SecureTable> lineitem_input = ZkTest::secret_share_input(lineitem_sql, true);
    shared_ptr<SecureTable> orders_input  = ZkTest::secret_share_input(orders_sql, true);



    ConjunctiveEqualityPredicate customer_orders_ordinals;
    customer_orders_ordinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey
    std::shared_ptr<BinaryPredicate<emp::Bit> > customer_orders_predicate(new JoinEqualityPredicate<emp::Bit>(customer_orders_ordinals));

    ConjunctiveEqualityPredicate lineitem_orders_ordinals;
    lineitem_orders_ordinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey
    std::shared_ptr<BinaryPredicate<emp::Bit> > lineitem_orders_predicate(new JoinEqualityPredicate<emp::Bit>(lineitem_orders_ordinals));


    KeyedJoin<emp::Bit> customerOrdersJoin(orders_input, customer_input, customer_orders_predicate);

    TableInput<emp::Bit> cte(lineitem_input);
    KeyedJoin full_join(&cte, &customerOrdersJoin, lineitem_orders_predicate);


    std::shared_ptr<PlainTable> joinResult = full_join.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema()->getFieldCount());
    Sort<emp::Bit> sort(&full_join, sortDefinition);
    std::shared_ptr<PlainTable> observed = sort.run()->reveal();
    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


