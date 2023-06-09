#include <gtest/gtest.h>
#include <operators/basic_join.h>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include "util/field_utilities.h"


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43443, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");

using namespace vaultdb;

class SecureBasicJoinTest : public EmpBaseTest {
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




TEST_F(SecureBasicJoinTest, test_tpch_q3_customer_orders) {


        std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                             "orders_cte AS (" + orders_sql_ + ") "
                                                                                                             "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                                                                                             "FROM customer_cte, orders_cte "
                                                                                                             "WHERE NOT cdummy AND NOT odummy AND c_custkey = o_custkey "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, storage_model_, netio_, FLAGS_party);
    auto orders_input= new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, netio_, FLAGS_party);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                     customer_input, 4);

    auto join = new BasicJoin(orders_input, customer_input, predicate);



    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
    Sort<emp::Bit> sort(join, sort_def);

   PlainTable *observed = sort.run()->reveal();
    expected->setSortOrder(sort_def);

    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete observed;


}


TEST_F(SecureBasicJoinTest, test_tpch_q3_lineitem_orders) {


// get inputs from local oblivious ops
// first 3 customers, propagate this constraint up the join tree for the test
std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                                                                     "lineitem_cte AS (" + lineitem_sql_ + ") "
                                                                                                         "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority "
                                                                                                         "FROM lineitem_cte, orders_cte "
                                                                                                         "WHERE NOT odummy AND NOT ldummy AND l_orderkey = o_orderkey "
                                                                                                         "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority";


    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, storage_model_, netio_, FLAGS_party);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, netio_, FLAGS_party);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lineitem_input, 0,
                                                                                      orders_input, 2);

    auto join = new BasicJoin(lineitem_input, orders_input, predicate);




    SortDefinition  sortDefinition = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
    Sort<emp::Bit> sort(join, sortDefinition);
    PlainTable *observed = sort.run()->reveal();

    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;


}



// compose C-O-L join should produce one output tuple, order ID 210945
TEST_F(SecureBasicJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                      "lineitem_cte AS (" + lineitem_sql_ + "), "
                                        "customer_cte AS (" + customer_sql_ + ") "
                                         "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                             "FROM lineitem_cte, orders_cte, customer_cte "
                                             "WHERE NOT odummy AND NOT ldummy AND NOT cdummy AND l_orderkey = o_orderkey AND c_custkey = o_custkey "
                                             "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";

    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, storage_model_, netio_, FLAGS_party);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, storage_model_, netio_, FLAGS_party);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, storage_model_, netio_, FLAGS_party);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * customer_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(
            orders_input, 1,
            customer_input,
            4);
    auto customer_orders_join = new BasicJoin (orders_input, customer_input, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(
            lineitem_input, 0,
            customer_orders_join,
            2);



    auto full_join = new BasicJoin (lineitem_input, customer_orders_join, lineitem_orders_predicate);




    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(full_join->getOutputSchema().getFieldCount());
    Sort<emp::Bit> sort(full_join, sort_def);
    PlainTable *observed = sort.run()->reveal();
    expected->setSortOrder(sort_def);

        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


