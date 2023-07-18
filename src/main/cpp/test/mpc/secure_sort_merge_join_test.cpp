#include <gtest/gtest.h>
#include <stdexcept>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include <operators/sort_merge_join.h>
#include "util/field_utilities.h"
#include "opt/operator_cost_model.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43455, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for generator");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65470, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


using namespace vaultdb;

class SecureSortMergeJoinTest : public EmpBaseTest {
protected:


    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey < " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY c_custkey";


    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                    "FROM orders \n"
                                    "WHERE o_custkey <  " + std::to_string(FLAGS_cutoff) +
                                    " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey < " + std::to_string(FLAGS_cutoff) + ")  \n"
                                                                                                                                          " ORDER BY l_orderkey, revenue ";
    void runCustomerOrdersTest();
    void runLineitemOrdersTest();
    void runLineitemOrdersCustomerTest();




};

void SecureSortMergeJoinTest::runCustomerOrdersTest() {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                          "orders_cte AS (" + orders_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                                                                                            "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                            "WHERE NOT o_dummy AND NOT c_dummy "
                                                                                                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";



    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                                                       customer_input, 4);

    SortMergeJoin join(orders_input, customer_input, predicate);
    // Join output schema: (#0 encrypted-int32(13) orders.o_orderkey, #1 encrypted-int32(8) orders.o_custkey, #2 encrypted-int64(28) orders.o_orderdate, #3 encrypted-int32(1) orders.o_shippriority, #4 encrypted-int32(8) customer.c_custkey)

	std::cout << "Predicted cost: " << OperatorCostModel::operatorCost(&join) << "\n";
    auto joined = join.run();
    if(FLAGS_validation) {
        PlainTable *observed = joined->reveal();
        Sort sorter(observed, DataUtilities::getDefaultSortDefinition(5));
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);
        expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);
        delete expected;
    }

}

void SecureSortMergeJoinTest::runLineitemOrdersTest() {
    // get inputs from local oblivious ops
    // first N customers, propagate this constraint up the join tree for the test
    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                      "lineitem_cte AS (" + lineitem_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue \n"
                                                                                                            "FROM orders_cte o JOIN lineitem_cte l ON l_orderkey = o_orderkey \n"
                                                                                                            "WHERE NOT o_dummy AND NOT l_dummy "
                                                                                                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";

    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql,  false);

    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true);


    // join output schema:
    //  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue,
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 0,
                                                                                      lineitem_input, 4);
    SortMergeJoin join(orders_input, lineitem_input, 1, predicate);
    auto joined = join.run();

    if(FLAGS_validation) {
        PlainTable *observed = joined->reveal();
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(6);
        Sort observed_sort(observed, sort_def);
        observed = observed_sort.run();

        expected->setSortOrder(sort_def);

        ASSERT_EQ(*expected, *observed);
        delete expected;

    }


}

void SecureSortMergeJoinTest::runLineitemOrdersCustomerTest() {
    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                     "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                     "customer_cte AS (" + customer_sql_ + ")\n "
                               "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey \n"
                               "FROM customer_cte JOIN orders_cte ON o_custkey = c_custkey "
                               " JOIN lineitem_cte ON o_orderkey = l_orderkey "
                               " WHERE NOT c_dummy AND NOT o_dummy AND NOT l_dummy "
                               " ORDER BY  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey  \n";

    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql,  false);


    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *co_predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                                                          customer_input, 4);

    auto co_join = new SortMergeJoin(orders_input, customer_input, co_predicate);


    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *lineitem_orders_predicate = (GenericExpression<Bit> *)  FieldUtilities::getEqualityPredicate<emp::Bit>(
            lineitem_input, 0,
            co_join,
            2);

    SortMergeJoin col_join(lineitem_input, co_join, lineitem_orders_predicate);
    auto joined = col_join.run();
    if(FLAGS_validation) {
        PlainTable *observed = col_join.run()->reveal();
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
        Sort<bool> observed_sort(observed, sort_def);
        observed = observed_sort.run();

        expected->setSortOrder(observed->getSortOrder());
        ASSERT_EQ(*expected, *observed);
        delete expected;
    }

}

TEST_F(SecureSortMergeJoinTest, test_tpch_q3_customer_orders) {
    runCustomerOrdersTest();

}


TEST_F(SecureSortMergeJoinTest, test_tpch_q3_customer_orders_no_bitpacking) {
    this->disableBitPacking();
    runCustomerOrdersTest();
}

TEST_F(SecureSortMergeJoinTest, test_tpch_q3_lineitem_orders) {
    runLineitemOrdersTest();
}

TEST_F(SecureSortMergeJoinTest, test_tpch_q3_lineitem_orders_no_bitpacking) {

    this->disableBitPacking();
    runLineitemOrdersTest();


}

// compose C-O-L join
TEST_F(SecureSortMergeJoinTest, test_tpch_q3_lineitem_orders_customer) {

    runLineitemOrdersCustomerTest();
}


TEST_F(SecureSortMergeJoinTest, test_tpch_q3_lineitem_orders_customer_no_bitpacking) {
    this->disableBitPacking();
    runLineitemOrdersCustomerTest();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}


