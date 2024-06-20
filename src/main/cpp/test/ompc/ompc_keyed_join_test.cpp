#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/ompc/ompc_base_test.h>
#include <operators/keyed_join.h>
#include <operators/project.h>
#include "util/field_utilities.h"
#include "opt/operator_cost_model.h"
#include "expression/generic_expression.h"
#include "expression/math_expression_nodes.h"
#include <operators/packed_table_scan.h>

#if __has_include("emp-rescu/emp-rescu.h")


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43442, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(cutoff, 10, "limit clause for queries");
DEFINE_int32(ctrl_port, 65458, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wires, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");

using namespace vaultdb;
using namespace Logging;

class OMPCKeyedJoinTest : public OmpcBaseTest {
protected:

    std::string src_path_ = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path_ = src_path_ + "/packed_pages/";

    const int customer_limit_ = 5;

    const int orders_limit_ = 30;

    const int lineitem_limit_ = 90;

    const std::string customer_sql_ = "SELECT c_custkey \n" // ignore c_mktsegment <> 'HOUSEHOLD' cdummy for now
                                      "FROM customer \n"
                                      "ORDER BY c_custkey \n"
                                      "LIMIT " + std::to_string(customer_limit_);

    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority \n" // ignore o_orderdate >= date '1995-03-25' odummy for now
                                    "FROM orders \n"
                                    "ORDER BY o_orderkey \n"
                                    "LIMIT " + std::to_string(orders_limit_);

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue \n" // ignore l_shipdate <= date '1995-03-25' ldummy for now
                                      "FROM lineitem \n"
                                      "ORDER BY l_orderkey \n"
                                      "LIMIT " + std::to_string(lineitem_limit_);

};


TEST_F(OMPCKeyedJoinTest, test_tpch_q3_customer_orders) {
    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                           "customer_cte AS (" + customer_sql_ + ") "
                      "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                      "FROM orders_cte, customer_cte "
                      "WHERE o_custkey = c_custkey "
                      "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey"; // ignore NOT cdummy AND NOT odummy for now


    // Table scan for orders
    PackedTableScan<emp::Bit> *packed_orders_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, orders_limit_);
    packed_orders_table_scan->setOperatorId(-2);

    // Project orders table to o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy
    ExpressionMapBuilder<Bit> orders_builder(packed_orders_table_scan->getOutputSchema());
    orders_builder.addMapping(0, 0);
    orders_builder.addMapping(1, 1);
    orders_builder.addMapping(4, 2);
    orders_builder.addMapping(7, 3);

    Project<Bit> *orders_project = new Project(packed_orders_table_scan, orders_builder.getExprs());
    orders_project->setOperatorId(-2);

    // Table scan for customer
    PackedTableScan<emp::Bit> *packed_customer_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "customer", packed_pages_path_, FLAGS_party, customer_limit_);
    packed_customer_table_scan->setOperatorId(-2);

    // Project customer table to c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy
    ExpressionMapBuilder<Bit> customer_builder(packed_customer_table_scan->getOutputSchema());
    customer_builder.addMapping(0, 0);

    Project<Bit> *customer_project = new Project(packed_customer_table_scan, customer_builder.getExprs());
    customer_project->setOperatorId(-2);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_project, 1, customer_project, 4);

    // test fkey-pkey join
    KeyedJoin<emp::Bit> *join = new KeyedJoin(orders_project, customer_project, predicate);
    join->setOperatorId(-2);
    SecureTable *join_res = join->run();

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
        join_res->order_by_ = sort_def; // reveal() will sort for this
        PlainTable *observed = join_res->reveal();
        DataUtilities::removeDummies(observed);

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql,false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }

}

TEST_F(OMPCKeyedJoinTest, test_tpch_q3_lineitem_orders) {
    // get inputs from local oblivious ops
    // first 3 customers, propagate this constraint up the join tree for the test
    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                           "lineitem_cte AS (" + lineitem_sql_ + ") "
                      "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority "
                      "FROM lineitem_cte, orders_cte "
                      "WHERE l_orderkey = o_orderkey "
                      "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority"; // ignore NOT odummy AND NOT ldummy for now

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit_);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table to l_orderkey, l_extendedprice * (1 - l_discount) revenue
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(0, 0);
    InputReference<emp::Bit> *extendedprice_field = new InputReference<emp::Bit>(5, packed_lineitem_table_scan->getOutputSchema());
    InputReference<emp::Bit> *discount_field = new InputReference<emp::Bit>(6, packed_lineitem_table_scan->getOutputSchema());
    LiteralNode<emp::Bit> *one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
    MinusNode<emp::Bit> *one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
    TimesNode<emp::Bit> *extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
    Expression<emp::Bit> *revenue_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "revenue", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(revenue_expr, 1);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);

    // Table scan for orders
    PackedTableScan<emp::Bit> *packed_orders_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, orders_limit_);
    packed_orders_table_scan->setOperatorId(-2);

    // Project orders table to o_orderkey, o_custkey, o_orderdate, o_shippriority
    ExpressionMapBuilder<Bit> orders_builder(packed_orders_table_scan->getOutputSchema());
    orders_builder.addMapping(0, 0);
    orders_builder.addMapping(1, 1);
    orders_builder.addMapping(4, 2);
    orders_builder.addMapping(7, 3);

    Project<Bit> *orders_project = new Project(packed_orders_table_scan, orders_builder.getExprs());
    orders_project->setOperatorId(-2);

    // join output schema: (lineitem, orders)
    // l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lineitem_project, 0, orders_project, 2);

    // test pkey-fkey join
    KeyedJoin<emp::Bit> *join = new KeyedJoin(lineitem_project, orders_project, predicate);
    join->setOperatorId(-2);
    SecureTable *join_res = join->run();

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
        join_res->order_by_ = sort_def;
        // sort too slow with n^2 secret shared rows
        PlainTable  *observed = join_res->revealInsecure();
        DataUtilities::removeDummies(observed);
        Sort<bool> sorter(observed, sort_def);
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }


}

// compose C-O-L join
TEST_F(OMPCKeyedJoinTest, test_tpch_q3_lineitem_orders_customer) {
    std::string sql = "WITH lineitem_cte AS (" + lineitem_sql_ + "), "
                           "orders_cte AS (" + orders_sql_ + "), "
                           "customer_cte AS (" + customer_sql_ + ") "
                     "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                     "FROM lineitem_cte, orders_cte, customer_cte "
                     "WHERE l_orderkey = o_orderkey AND c_custkey = o_custkey "
                     "ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey"; // ignore NOT odummy AND NOT ldummy AND NOT cdummy for now

    // Table scan for orders
    PackedTableScan<emp::Bit> *packed_orders_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, orders_limit_);
    packed_orders_table_scan->setOperatorId(-2);

    // Project orders table to o_orderkey, o_custkey, o_orderdate, o_shippriority
    ExpressionMapBuilder<Bit> orders_builder(packed_orders_table_scan->getOutputSchema());
    orders_builder.addMapping(0, 0);
    orders_builder.addMapping(1, 1);
    orders_builder.addMapping(4, 2);
    orders_builder.addMapping(7, 3);

    Project<Bit> *orders_project = new Project(packed_orders_table_scan, orders_builder.getExprs());
    orders_project->setOperatorId(-2);

    // Table scan for customer
    PackedTableScan<emp::Bit> *packed_customer_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "customer", packed_pages_path_, FLAGS_party, customer_limit_);
    packed_customer_table_scan->setOperatorId(-2);

    // Project customer table to c_custkey
    ExpressionMapBuilder<Bit> customer_builder(packed_customer_table_scan->getOutputSchema());
    customer_builder.addMapping(0, 0);

    Project<Bit> *customer_project = new Project(packed_customer_table_scan, customer_builder.getExprs());
    customer_project->setOperatorId(-2);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * customer_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_project, 1,customer_project,4);
    KeyedJoin<Bit> *co_join = new KeyedJoin(orders_project, customer_project, customer_orders_predicate);
    co_join->setOperatorId(-2);

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit_);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table to l_orderkey, l_extendedprice * (1 - l_discount) revenue
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(0, 0);
    InputReference<emp::Bit> *extendedprice_field = new InputReference<emp::Bit>(5, packed_lineitem_table_scan->getOutputSchema());
    InputReference<emp::Bit> *discount_field = new InputReference<emp::Bit>(6, packed_lineitem_table_scan->getOutputSchema());
    LiteralNode<emp::Bit> *one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
    MinusNode<emp::Bit> *one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
    TimesNode<emp::Bit> *extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
    Expression<emp::Bit> *revenue_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "revenue", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(revenue_expr, 1);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>( lineitem_project, 0, co_join, 2);
    KeyedJoin<Bit> *col_join = new KeyedJoin(lineitem_project, co_join, lineitem_orders_predicate);
    col_join->setOperatorId(-2);
    SecureTable *join_res = col_join->run();

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(col_join->getOutputSchema().getFieldCount());
        join_res->order_by_ = sort_def;
        // sort too slow with n^2 secret shared rows
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
#endif


