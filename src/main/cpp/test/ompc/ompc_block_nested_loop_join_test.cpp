#include <gtest/gtest.h>
#include <operators/block_nested_loop_join.h>
#include <gflags/gflags.h>
#include <operators/project.h>
#include <operators/sort.h>
#include <test/ompc/ompc_base_test.h>
#include "util/field_utilities.h"
#include <operators/stored_table_scan.h>

#if __has_include("emp-rescu/emp-rescu.h")

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43443, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_int32(cutoff, 5, "limit clause for queries"); // not used here
DEFINE_int32(ctrl_port, 65450, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(wire_path, "wires", "relative path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");
DEFINE_int32(unpacked_page_size_bits, 2048, "unpacked page size in bits");
DEFINE_int32(page_cnt, 50, "number of pages in unpacked buffer pool");


using namespace vaultdb;

class OMPCBlockNestedLoopJoinTest : public OmpcBaseTest {
protected:

  // small
    const int customer_limit_ =   150;
    const int orders_limit_ = 200; 
    const int lineitem_limit_ =  500; 
  
  // medium
  /*  const int customer_limit_ =   150;
    const int orders_limit_ = 1500;
    const int lineitem_limit_ =  1500;
  */

  // large
  /*  const int customer_limit_ =   150;
    const int orders_limit_ = 1500; 
    const int lineitem_limit_ =  6005; 
  */

    const std::string customer_sql_ = "SELECT c_custkey \n" // ignore c_mktsegment <> 'HOUSEHOLD' cdummy for now
                                      "FROM customer \n"
                                      "ORDER BY c_custkey \n"
                                      "LIMIT " + std::to_string(customer_limit_);

    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority \n" // ignore o_orderdate >= date '1995-03-25' odummy for now
                                    "FROM orders \n"
                                    "ORDER BY o_orderkey \n"
                                    "LIMIT " + std::to_string(orders_limit_);

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice, l_discount \n" // ignore l_shipdate <= date '1995-03-25' ldummy for now
                                      "FROM lineitem \n"
                                      "ORDER BY l_orderkey, l_linenumber \n"
                                      "LIMIT " + std::to_string(lineitem_limit_);

    Operator<Bit> *getCustomers();
    Operator<Bit> *getOrders();
    Operator<Bit> *getLineitem();
};

Operator<Bit> *OMPCBlockNestedLoopJoinTest::getCustomers() {
    return new StoredTableScan<Bit>("customer", "c_custkey", customer_limit_);
}

Operator<Bit> *OMPCBlockNestedLoopJoinTest::getOrders() {
    vector<int> ordinals = {0, 1, 4, 7};
    return new StoredTableScan<Bit>("orders", ordinals, orders_limit_);

}

Operator<Bit> *OMPCBlockNestedLoopJoinTest::getLineitem() {
    return  new StoredTableScan<Bit>("lineitem", "l_orderkey, l_extendedprice, l_discount", lineitem_limit_);
}


TEST_F(OMPCBlockNestedLoopJoinTest, test_tpch_q3_customer_orders) {


    std::string sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                            "orders_cte AS (" + orders_sql_ + ") "
                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                            "FROM customer_cte, orders_cte "
                            "WHERE c_custkey = o_custkey "
                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey"; // ignore NOT cdummy AND NOT odummy for now

    time_point<high_resolution_clock> load_start_time = high_resolution_clock::now();
    Operator<Bit> *orders = getOrders();
    Operator<Bit> *customers = getCustomers();
    double load_runtime = time_from(load_start_time)/1e6;
    cout << "data load time: " << load_runtime << "s" << endl;

    // test fkey-pkey join
    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders, 1, customers, 4);
    BlockNestedLoopJoin<emp::Bit> *join = new BlockNestedLoopJoin(orders, customers, 0, predicate);
    time_point<high_resolution_clock> querying_start_time = high_resolution_clock::now();
    SecureTable *join_res = join->run();
    double querying_runtime = time_from(querying_start_time)/1e6;
    cout << "querying time: " << querying_runtime << "s" << endl;
    Utilities::checkMemoryUtilization(true);

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
        PlainTable *observed = join_res->revealInsecure();
        DataUtilities::removeDummies(observed);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql,false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }
}


TEST_F(OMPCBlockNestedLoopJoinTest, test_tpch_q3_lineitem_orders) {


    // get inputs from local oblivious ops
    // first 3 customers, propagate this constraint up the join tree for the test
    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                            "lineitem_cte AS (" + lineitem_sql_ + ") "
                       "SELECT l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority "
                       "FROM lineitem_cte, orders_cte "
                       "WHERE l_orderkey = o_orderkey "
                       "ORDER BY l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority"; // ignore NOT odummy AND NOT ldummy for now

    time_point<high_resolution_clock> load_start_time = high_resolution_clock::now();
    Operator<Bit> *lineitem = getLineitem();
    Operator<Bit> *orders = getOrders();
    double load_runtime = time_from(load_start_time)/1e6;
    cout << "data load time: " << load_runtime << "s" << endl;

    // test pkey-fkey join
    // join output schema: (lineitem, orders)
    // l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lineitem, 0, orders, 3 );
    BlockNestedLoopJoin<emp::Bit> *join = new BlockNestedLoopJoin(lineitem, orders, 0, predicate);
    time_point<high_resolution_clock> querying_start_time = high_resolution_clock::now();
    SecureTable *join_res = join->run();
    double querying_runtime = time_from(querying_start_time)/1e6;
    cout << "querying time: " << querying_runtime << "s" << endl;
    Utilities::checkMemoryUtilization(true);

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
        PlainTable  *observed = join_res->revealInsecure();
        DataUtilities::removeDummies(observed);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }
}



TEST_F(OMPCBlockNestedLoopJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                            "lineitem_cte AS (" + lineitem_sql_ + "), "
                            "customer_cte AS (" + customer_sql_ + ") "
                       "SELECT l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                       "FROM lineitem_cte, orders_cte, customer_cte "
                       "WHERE l_orderkey = o_orderkey AND c_custkey = o_custkey "
                       "ORDER BY l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey"; // ignore NOT odummy AND NOT ldummy AND NOT cdummy for now

    time_point<high_resolution_clock> load_start_time = high_resolution_clock::now();
    Operator<Bit> *orders = getOrders();
    Operator<Bit> *customers = getCustomers();
    Operator<Bit> *lineitem = getLineitem();
    double load_runtime = time_from(load_start_time)/1e6;
    cout << "data load time: " << load_runtime << "s" << endl;

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * customer_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders, 1,customers,4);
    BlockNestedLoopJoin<Bit> *co_join = new BlockNestedLoopJoin(orders, customers, 0, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, l_extendedprice, l_discount, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>( lineitem, 0, co_join, 3);
    BlockNestedLoopJoin<Bit> *col_join = new BlockNestedLoopJoin(lineitem, co_join, 0, lineitem_orders_predicate);
    time_point<high_resolution_clock> querying_start_time = high_resolution_clock::now();
    SecureTable *join_res = col_join->run();
    double querying_runtime = time_from(querying_start_time)/1e6;
    cout << "querying time: " << querying_runtime << "s" << endl;
    Utilities::checkMemoryUtilization(true);

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(col_join->getOutputSchema().getFieldCount());
        PlainTable *observed = join_res->revealInsecure();
        DataUtilities::removeDummies(observed);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
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
#else
int main(int argc, char **argv) {
    std::cout << "emp-rescu backend not found!" << std::endl;
}


#endif
