#include <gtest/gtest.h>
#include <stdexcept>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include <operators/keyed_sort_merge_join.h>
#include "util/field_utilities.h"
#include "opt/operator_cost_model.h"
#include "util/logger.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43455, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for generator");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 10, "limit clause for queries");
DEFINE_int32(ctrl_port, 65470, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");


using namespace vaultdb;
using namespace Logging;

class SecureKeyedSortMergeJoinTest : public EmpBaseTest {
protected:


    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY c_custkey";


    const std::string orders_cust_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                         "FROM orders \n"
                                         "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                         " ORDER BY o_custkey, o_orderkey";

    const std::string orders_lineitem_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                             "FROM orders \n"
                                             "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                             " ORDER BY o_orderkey";


    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= " + std::to_string(FLAGS_cutoff) +  ")  \n"
                                       " ORDER BY l_orderkey, revenue, l_dummy ";

    SortDefinition customer_sort_ = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition orders_cust_sort_{ColumnSort (1, SortDirection::ASCENDING), ColumnSort (0, SortDirection::ASCENDING)};
    SortDefinition orders_lineitem_sort_{ColumnSort (0, SortDirection::ASCENDING)};
    SortDefinition lineitem_sort_ = DataUtilities::getDefaultSortDefinition(2);




};

TEST_F(SecureKeyedSortMergeJoinTest, test_tpch_q3_customer_orders) {
    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                     "orders_cte AS (" + orders_cust_sql_ + ") "
                               "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                               "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                               "WHERE NOT o_dummy AND NOT c_dummy "
                               "ORDER BY o_orderkey";


    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto orders_input = new SecureSqlInput(db_name_, orders_cust_sql_, true, orders_cust_sort_);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                                                       customer_input, 4);

    KeyedSortMergeJoin join(orders_input, customer_input, predicate);
    // Join output schema: (#0 shared-int32(13) orders.o_orderkey, #1 shared-int32(8) orders.o_custkey, #2 shared-int64(28) orders.o_orderdate, #3 shared-int32(1) orders.o_shippriority, #4 shared-int32(8) customer.c_custkey)
    auto joined = join.run();


    Logger* log = get_log();

    log->write("Predicted gate count: " + std::to_string(OperatorCostModel::operatorCost(&join)), Level::INFO);
    log->write("Observed gate count: " + std::to_string(join.getGateCount()), Level::INFO);
    log->write("Runtime: " + std::to_string(join.getRuntimeMs()), Level::INFO);

    if(FLAGS_validation) {
        auto sort_def = DataUtilities::getDefaultSortDefinition(1);
        joined->order_by_ = sort_def;
        PlainTable *observed = joined->reveal();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;
    }

}




TEST_F(SecureKeyedSortMergeJoinTest, test_tpch_q3_lineitem_orders) {
    // get inputs from oblivious ops
    // first N customers, propagate this constraint up the join tree for the test
    std::string expected_sql = "WITH orders_cte AS (" + orders_lineitem_sql_ + "), "
                                     "lineitem_cte AS (" + lineitem_sql_ + ") "
                              "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue \n"
                              "FROM orders_cte o JOIN lineitem_cte l ON l_orderkey = o_orderkey \n"
                              "WHERE NOT o_dummy AND NOT l_dummy "
                              "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";

    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);
    auto orders_input = new SecureSqlInput(db_name_, orders_lineitem_sql_, true, orders_lineitem_sort_);

    // join output schema:
    //  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue,
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 0,
                                                                                      lineitem_input, 4);
    KeyedSortMergeJoin join(orders_input, lineitem_input, 1, predicate);
    auto joined = join.run();

	Logger* log = get_log();	

    log->write("Predicted gate count: " + std::to_string(OperatorCostModel::operatorCost(&join)), Level::INFO);
	log->write("Observed gate count: " + std::to_string(join.getGateCount()), Level::INFO);
	log->write("Runtime: " + std::to_string(join.getRuntimeMs()), Level::INFO);

    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql,  false);
        PlainTable *observed = joined->reveal();
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(6);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
        delete expected;

    }


}

// compose C-O-L join
TEST_F(SecureKeyedSortMergeJoinTest, test_tpch_q3_lineitem_orders_customer) {
    std::string expected_sql = "WITH orders_cte AS (" + orders_cust_sql_ + "), \n"
                                     "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                     "customer_cte AS (" + customer_sql_ + ")\n "
                               "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey \n"
                               "FROM customer_cte JOIN orders_cte ON o_custkey = c_custkey "
                               " JOIN lineitem_cte ON o_orderkey = l_orderkey "
                               " WHERE NOT c_dummy AND NOT o_dummy AND NOT l_dummy "
                               " ORDER BY  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey  \n";

    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql,  false);


    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto orders_input = new SecureSqlInput(db_name_, orders_cust_sql_, true, orders_cust_sort_);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *co_predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                                                          customer_input, 4);

    auto co_join = new KeyedSortMergeJoin(orders_input, customer_input, co_predicate);


    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *lineitem_orders_predicate = (GenericExpression<Bit> *)  FieldUtilities::getEqualityPredicate<emp::Bit>(
            lineitem_input, 0,
            co_join,
            2);

    KeyedSortMergeJoin col_join(lineitem_input, co_join, lineitem_orders_predicate);
    auto joined = col_join.run();

	Logger* log = get_log();	

    log->write("Predicted gate count: " + std::to_string(OperatorCostModel::operatorCost(&col_join)), Level::INFO);
	log->write("Observed gate count: " + std::to_string(col_join.getGateCount()), Level::INFO);
	log->write("Runtime: " + std::to_string(col_join.getRuntimeMs()), Level::INFO);

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
        joined->order_by_ = sort_def;
        PlainTable *observed = joined->reveal();

        expected->order_by_ = sort_def;
        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;
    }

}





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}


