#include <gtest/gtest.h>
#include <stdexcept>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include <operators/keyed_join.h>
#include <expression/comparator_expression_nodes.h>
#include "util/field_utilities.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43442, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");

using namespace vaultdb;

class SecureKeyedJoinTest : public EmpBaseTest {
protected:

    int cutoff_ = 100;

    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey < " + std::to_string(cutoff_) +
                                      " ORDER BY c_custkey";


    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                    "FROM orders \n"
                                    "WHERE o_custkey <  " + std::to_string(cutoff_) +
                                    " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey < " + std::to_string(cutoff_) + ")  \n"
                                                                                                                                          " ORDER BY l_orderkey, revenue ";





};




TEST_F(SecureKeyedJoinTest, test_tpch_q3_customer_orders) {


    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                          "orders_cte AS (" + orders_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                                                                                            "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                            "WHERE NOT o_dummy AND NOT c_dummy "
                                                                                                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";

    this->initializeBitPacking(unioned_db_);
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);


    SortDefinition  customer_sort = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition  orders_sort = DataUtilities::getDefaultSortDefinition(4);
    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, storage_model_, customer_sort, netio_, FLAGS_party);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, orders_sort, netio_, FLAGS_party);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    // should use 9/32 bits
    GenericExpression<emp::Bit> *predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                      customer_input, 4);


    KeyedJoin join(orders_input, customer_input, predicate);
    PlainTable *observed = join.run()->reveal();



    expected->setSortOrder(observed->getSortOrder());
    std::cout << "Operator tree: \n" << join.printTree() << '\n';


    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete observed;


}


TEST_F(SecureKeyedJoinTest, test_tpch_q3_customer_orders_no_bit_packing) {

    this->disableBitPacking();

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                          "orders_cte AS (" + orders_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                                                                                            "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                            "WHERE NOT o_dummy AND NOT c_dummy "
                                                                                                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);



    SortDefinition  customer_sort = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition  orders_sort = DataUtilities::getDefaultSortDefinition(4);
    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, storage_model_, customer_sort, netio_, FLAGS_party);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, orders_sort, netio_, FLAGS_party);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    // uses 11/32 bits
    GenericExpression<emp::Bit> *predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                                                       customer_input, 4);


    KeyedJoin join(orders_input, customer_input, predicate);
    PlainTable *observed = join.run()->reveal();


    expected->setSortOrder(observed->getSortOrder());

    std::cout << "Operator tree: \n" << join.printTree() << '\n';

    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete observed;


}



TEST_F(SecureKeyedJoinTest, test_tpch_q3_lineitem_orders) {


// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                      "lineitem_cte AS (" + lineitem_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue \n"
                                                                                                            "FROM orders_cte o JOIN lineitem_cte l ON l_orderkey = o_orderkey \n"
                                                                                                            "WHERE NOT o_dummy AND NOT l_dummy "
                                                                                                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";

    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, storage_model_, netio_, FLAGS_party);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, netio_, FLAGS_party);


    // join output schema:
    //  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue,
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 0,
                                                                                      lineitem_input, 4);
   // test pkey-fkey join
    KeyedJoin join(orders_input, lineitem_input, 1, predicate);


    PlainTable *observed = join.run()->reveal();
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(6);
    Sort<bool> observed_sort(observed, sort_def);
    observed = observed_sort.run();

    expected->setSortOrder(sort_def);
    std::cout << "Operator tree: \n" << join.printTree() << '\n';
    ASSERT_EQ(*expected, *observed);
    delete expected;
   // delete observed; - covered by sort

}


TEST_F(SecureKeyedJoinTest, test_tpch_q3_lineitem_orders_no_bit_packing) {

    this->disableBitPacking();

// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                      "lineitem_cte AS (" + lineitem_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue \n"
                                                                                                            "FROM orders_cte o JOIN lineitem_cte l ON l_orderkey = o_orderkey \n"
                                                                                                            "WHERE NOT o_dummy AND NOT l_dummy "
                                                                                                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";

    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, netio_, FLAGS_party);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, storage_model_, netio_, FLAGS_party);


    // join output schema:
    //  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue,
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 0,
                                                                                      lineitem_input, 4);
    // test pkey-fkey join
    KeyedJoin join(orders_input, lineitem_input, 1, predicate);


    PlainTable *observed = join.run()->reveal();

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(6);
    Sort<bool> observed_sort(observed, sort_def);
    observed = observed_sort.run();

    expected->setSortOrder(sort_def);

    std::cout << "Operator tree: \n" << join.printTree() << '\n';

    ASSERT_EQ(*expected, *observed);
    delete expected;

}



// compose C-O-L join
TEST_F(SecureKeyedJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                                                      "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                                                                                            "customer_cte AS (" + customer_sql_ + ")\n "
                                                                                                                                                  "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey \n"
                                                                                                                                                  "FROM customer_cte JOIN orders_cte ON o_custkey = c_custkey "
                                                                                                                                                  " JOIN lineitem_cte ON o_orderkey = l_orderkey "
                                                                                                                                                  " WHERE NOT c_dummy AND NOT o_dummy AND NOT l_dummy "
                                                                                                                                                  " ORDER BY  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey  \n";

    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, storage_model_, netio_, FLAGS_party);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, netio_, FLAGS_party);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, storage_model_, netio_, FLAGS_party);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(
            orders_input, 1,
            customer_input,
            4);

    auto co_join = new KeyedJoin (orders_input, customer_input, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(
            lineitem_input, 0,
            co_join,
            2);


    auto col_join = new KeyedJoin(lineitem_input, co_join, lineitem_orders_predicate);


    PlainTable *observed = col_join->run()->reveal();
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
    Sort<bool> observed_sort(observed, sort_def);
    observed = observed_sort.run();


    expected->setSortOrder(observed->getSortOrder());

    std::cout << "Operator tree: \n" << col_join->printTree() << '\n';

    ASSERT_EQ(*expected, *observed);
    delete col_join;
    delete expected;
    delete observed;


}

TEST_F(SecureKeyedJoinTest, test_tpch_q3_lineitem_orders_customer_no_bit_packing) {

    this->disableBitPacking();
    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                                                      "lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                                                                                            "customer_cte AS (" + customer_sql_ + ")\n "
                                                                                                                                                  "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey \n"
                                                                                                                                                  "FROM customer_cte JOIN orders_cte ON o_custkey = c_custkey "
                                                                                                                                                  " JOIN lineitem_cte ON o_orderkey = l_orderkey "
                                                                                                                                                  " WHERE NOT c_dummy AND NOT o_dummy AND NOT l_dummy "
                                                                                                                                                  " ORDER BY  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey  \n";

    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, storage_model_, netio_, FLAGS_party);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, netio_, FLAGS_party);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, storage_model_, netio_, FLAGS_party);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(
            orders_input, 1,
            customer_input,
            4);

    auto co_join = new KeyedJoin (orders_input, customer_input, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(
            lineitem_input, 0,
            co_join,
            2);


    auto col_join = new KeyedJoin(lineitem_input, co_join, lineitem_orders_predicate);


    PlainTable *observed = col_join->run()->reveal();

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
    Sort<bool> observed_sort(observed, sort_def);
    observed = observed_sort.run();


    expected->setSortOrder(observed->getSortOrder());
    std::cout << "Operator tree: \n" << col_join->printTree() << '\n';

    ASSERT_EQ(*expected, *observed);
    delete col_join;
    delete expected;
    delete observed;


}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


