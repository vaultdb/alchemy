#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/support/EmpBaseTest.h>
#include <operators/fkey_pkey_join.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");

using namespace vaultdb;

class SecurePkeyFkeyJoinTest : public EmpBaseTest {
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




TEST_F(SecurePkeyFkeyJoinTest, test_tpch_q3_customer_orders) {


    std::string expectedResultSql = "WITH customer_cte AS (" + customerSql + "), "
                                                                             "orders_cte AS (" + ordersSql + ") "
                                                                                                             "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,(cdummy OR odummy) dummy "
                                                                                                             "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(unionedDb, expectedResultSql, true);


    SecureSqlInput customerInput(dbName, customerSql, true, netio, FLAGS_party);
    SecureSqlInput ordersInput(dbName, ordersSql, true, netio, FLAGS_party);


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey

    std::shared_ptr<BinaryPredicate<SecureBoolField> > customerOrdersPredicate(new JoinEqualityPredicate<SecureBoolField> (customerOrdersOrdinals));

    KeyedJoin join(&ordersInput, &customerInput, customerOrdersPredicate);

    std::shared_ptr<QueryTable> joinResult = join.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema().getFieldCount());
    Sort<SecureBoolField> sort(&join, sortDefinition);
    shared_ptr<QueryTable> observed = sort.run()->reveal();
    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(expected->toString(true), observed->toString(true));
    ASSERT_EQ(*expected, *observed);

}


TEST_F(SecurePkeyFkeyJoinTest, test_tpch_q3_lineitem_orders) {


// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                                                         "lineitem_cte AS (" + lineitemSql + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (odummy OR ldummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(unionedDb, expectedResultSql, true);

    SecureSqlInput lineitemInput(dbName, lineitemSql, true, netio, FLAGS_party);
    SecureSqlInput ordersInput(dbName, ordersSql, true, netio, FLAGS_party);

    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey

    std::shared_ptr<BinaryPredicate<SecureBoolField>  > customerOrdersPredicate(new JoinEqualityPredicate<SecureBoolField>(lineitemOrdersOrdinals));

    KeyedJoin join(&lineitemInput, &ordersInput, customerOrdersPredicate);


    std::shared_ptr<QueryTable> joinResult = join.run();
    std::unique_ptr<QueryTable> joinResultDecrypted = joinResult->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema().getFieldCount());
    Sort<SecureBoolField>  sort(&join, sortDefinition);
    std::shared_ptr<QueryTable> observed = sort.run()->reveal();
    expected->setSortOrder(sortDefinition);




    ASSERT_EQ(observed->toString(true), expected->toString(true));
    ASSERT_EQ(*expected, *observed);

}



// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(SecurePkeyFkeyJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), \n"
                                                                         "lineitem_cte AS (" + lineitemSql + "), \n"
                                                                                                             "customer_cte AS (" + customerSql + "),\n "
                                                                                                                                                 "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                                                                                                                                 "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                                                                                                                                 "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                                                                 "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                                                                                                                                 "FROM cross_product \n"
                                                                                                                                                 "WHERE matched";

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(unionedDb, expectedResultSql, true);

    SecureSqlInput customerInput(dbName, customerSql, true, netio, FLAGS_party);
    SecureSqlInput lineitemInput(dbName, lineitemSql, true, netio, FLAGS_party);
    SecureSqlInput ordersInput(dbName, ordersSql, true, netio, FLAGS_party);


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey
    std::shared_ptr<BinaryPredicate<SecureBoolField> > customerOrdersPredicate(new JoinEqualityPredicate<SecureBoolField>(customerOrdersOrdinals));

    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey
    std::shared_ptr<BinaryPredicate<SecureBoolField> > lineitemOrdersPredicate(new JoinEqualityPredicate<SecureBoolField>(lineitemOrdersOrdinals));


    KeyedJoin customerOrdersJoin(&ordersInput, &customerInput, customerOrdersPredicate);

    KeyedJoin fullJoin(&lineitemInput, &customerOrdersJoin, lineitemOrdersPredicate);


    std::shared_ptr<QueryTable> joinResult = fullJoin.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema().getFieldCount());
    Sort<SecureBoolField> sort(&fullJoin, sortDefinition);
    std::shared_ptr<QueryTable> observed = sort.run()->reveal();
    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}



