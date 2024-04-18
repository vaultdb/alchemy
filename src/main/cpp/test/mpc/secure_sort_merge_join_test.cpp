#include <gtest/gtest.h>
#include <stdexcept>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include <operators/sort_merge_join.h>
#include "util/field_utilities.h"
#include "opt/operator_cost_model.h"
#include "util/logger.h"


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43455, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for generator");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 1000, "limit clause for queries");
DEFINE_int32(ctrl_port, 65470, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");




class SecureSortMergeJoinTest :  public EmpBaseTest  {


protected:


// simulating TPC-H Q5
    const std::string customer_sql_ = "SELECT c_custkey, c_nationkey, c_mktsegment, r_name = 'EUROPE' c_dummy \n"
                                      "FROM customer  JOIN nation ON c_nationkey = n_nationkey"
                                      "     JOIN region ON r_regionkey = n_regionkey \n"
                                      "WHERE c_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY c_custkey ";





    const std::string supplier_sql_ = "SELECT s_suppkey, s_nationkey, r_name = 'EUROPE' s_dummy \n"
                                      "FROM supplier  JOIN nation ON s_nationkey = n_nationkey"
                                      "     JOIN region ON r_regionkey = n_regionkey \n"
                                      "WHERE s_suppkey <=  " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY s_suppkey";



    SortDefinition customer_sort_ = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition supplier_sort_ = DataUtilities::getDefaultSortDefinition(1);
    // TODO: check this - will depend on operator implementation
    SortDefinition customer_supplir_sort_ = {{0, SortDirection::ASCENDING}, {2, SortDirection::ASCENDING}};




};




TEST_F(SecureSortMergeJoinTest, test_tpch_q3_customer_supplier) {


    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                          "supplier_cte AS (" + supplier_sql_ + ") "
                                                                                                                "SELECT c_custkey, c_nationkey, c_mktsegment,  s_suppkey, s_nationkey "
                                                                                                                "FROM  customer_cte JOIN supplier_cte ON c_nationkey = s_nationkey "
                                                                                                                "WHERE NOT c_dummy AND NOT s_dummy "
                                                                                                                "ORDER BY c_custkey, c_nationkey, c_mktsegment,  s_suppkey, s_nationkey  ";




    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);


    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto supplier_input = new SecureSqlInput(db_name_, supplier_sql_, true, supplier_sort_);


    // join output schema: (customer, supplier)
    // c_custkey, c_nationkey, c_mktsegment, s_suppkey, s_nationkey
    Expression<Bit> *predicate = FieldUtilities::getEqualityPredicate<Bit>(customer_input, 1, supplier_input, 4);

    auto join = new SortMergeJoin<Bit>(customer_input, supplier_input,  predicate);
    //SortMergeJoin join(customer_input, supplier_input, predicate);
    auto joined = join->run();

    Logging::Logger* log = Logging::get_log();

    log->write("Predicted gate count: " + std::to_string(OperatorCostModel::operatorCost(join)), Logging::Level::INFO);
    log->write("Observed gate count: " + std::to_string(join->getGateCount()), Logging::Level::INFO);
    log->write("Runtime: " + std::to_string(join->getRuntimeMs()), Logging::Level::INFO);

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(4);
    Sort<Bit> sorter(joined, sort_def);
    PlainTable *observed = sorter.run()->reveal();
    expected->order_by_ = observed->order_by_;
    DataUtilities::removeDummies(observed);


    ASSERT_EQ(*expected, *observed);


    delete expected;




}


TEST_F(SecureSortMergeJoinTest, test_tpch_q3_customer_supplier_NLJ) {


    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                          "supplier_cte AS (" + supplier_sql_ + ") "
                                                                                                                "SELECT c_custkey, c_nationkey, c_mktsegment,  s_suppkey, s_nationkey "
                                                                                                                "FROM  customer_cte JOIN supplier_cte ON c_nationkey = s_nationkey "
                                                                                                                "WHERE NOT c_dummy AND NOT s_dummy "
                                                                                                                "ORDER BY c_custkey, c_nationkey, c_mktsegment,  s_suppkey, s_nationkey  ";




    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);


    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, customer_sort_);
    auto supplier_input = new SecureSqlInput(db_name_, supplier_sql_, true, supplier_sort_);


    // join output schema: (customer, supplier)
    // c_custkey, c_nationkey, c_mktsegment, s_suppkey, s_nationkey
    Expression<Bit> *predicate = FieldUtilities::getEqualityPredicate<Bit>(customer_input, 1, supplier_input, 4);

    auto join = new BasicJoin<Bit>(customer_input, supplier_input,  predicate);
    //SortMergeJoin join(customer_input, supplier_input, predicate);
    auto joined = join->run();

    Logging::Logger* log = Logging::get_log();

    log->write("Predicted gate count: " + std::to_string(OperatorCostModel::operatorCost(join)), Logging::Level::INFO);
    log->write("Observed gate count: " + std::to_string(join->getGateCount()), Logging::Level::INFO);
    log->write("Runtime: " + std::to_string(join->getRuntimeMs()), Logging::Level::INFO);

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(4);
    Sort<Bit> sorter(joined, sort_def);
    PlainTable *observed = sorter.run()->reveal();
    expected->order_by_ = observed->order_by_;
    DataUtilities::removeDummies(observed);


    ASSERT_EQ(*expected, *observed);


    delete expected;




}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);


    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;


}