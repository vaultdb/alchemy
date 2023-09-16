#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/zk/zk_base_test.h>
#include <operators/keyed_join.h>
#include "util/field_utilities.h"
#include "operators/zk_sql_input.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43448, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(empty_db, "tpch_empty", "empty db name");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


using namespace vaultdb;

class ZkKeyedJoinTest : public ZkTest {
protected:



    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy \n"
                                     "FROM customer  \n"
                                     "WHERE c_custkey < 3 \n"
                                     "ORDER BY c_custkey";

    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy \n"
                                   "FROM orders \n"
                                   "WHERE o_custkey < 3 \n"
                                   "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy \n"
                                     "FROM lineitem \n"
                                     "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey < 3)  \n"
                                     "ORDER BY l_orderkey, revenue ";




};




TEST_F(ZkKeyedJoinTest, test_tpch_q3_customer_orders) {



    std::string expectedResultSql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                           "orders_cte AS (" + orders_sql_ + ") "
                                     "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,(cdummy OR odummy) dummy "
                                     "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                     "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";



    PlainTable * expected = DataUtilities::getQueryResults(unioned_db_, expectedResultSql, true);
    auto customer_input = new ZkSqlInput(db_name_, customer_sql_, true);
    auto orders_input = new ZkSqlInput(db_name_, orders_sql_, true);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    auto predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                    customer_input, 4);


    KeyedJoin join(orders_input, customer_input, predicate);

    PlainTable * joinResult = join.run()->reveal();


    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(joinResult->getSchema().getFieldCount());
    Sort<emp::Bit> sort(&join, sortDefinition);
    PlainTable * observed = sort.run()->reveal();


    expected->setSortOrder(sortDefinition);


    ASSERT_EQ(*expected, *observed);

}


TEST_F(ZkKeyedJoinTest, test_tpch_q3_lineitem_orders) {


// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (odummy OR ldummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    PlainTable * expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, true);

    auto lineitem_input = new ZkSqlInput(db_name_, lineitem_sql_, true);
    auto orders_input = new ZkSqlInput(db_name_, orders_sql_, true);



    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<Bit>(lineitem_input, 0, orders_input, 2);

    KeyedJoin join(lineitem_input, orders_input, predicate);


    PlainTable *revealed = join.run()->reveal();


    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition( revealed->getSchema().getFieldCount());
    Sort sort(revealed, sort_def);
    PlainTable *observed = sort.run();

    expected->setSortOrder(sort_def);
    ASSERT_EQ(*expected, *observed);

}



// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(ZkKeyedJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string expected_result_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                                                                                             "customer_cte AS (" + customer_sql_ + "),\n "
                                                                                                                                                 "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                                                                                                                                 "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                                                                                                                                 "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                                                                 "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                                                                                                                                 "FROM cross_product \n"
                                                                                                                                                 "WHERE matched";

    PlainTable * expected = DataUtilities::getQueryResults(unioned_db_, expected_result_sql, true);

    auto customer_input = new ZkSqlInput(db_name_, customer_sql_, true);
    auto orders_input = new ZkSqlInput(db_name_, orders_sql_, true);
    auto lineitem_input = new ZkSqlInput(db_name_, lineitem_sql_, true);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    auto customer_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                   customer_input, 4);




    auto  co_join = new KeyedJoin<emp::Bit>(orders_input, customer_input, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<Bit>(lineitem_input, 0, co_join, 2);


    KeyedJoin full_join(lineitem_input, co_join, lineitem_orders_predicate);


    PlainTable * revealed = full_join.run()->reveal();


    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(revealed->getSchema().getFieldCount());
    Sort sort(revealed, sort_def);
    PlainTable * observed = sort.run()->reveal();
    expected->setSortOrder(sort_def);

    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


