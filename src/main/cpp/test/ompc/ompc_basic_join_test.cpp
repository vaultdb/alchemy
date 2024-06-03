#include <gtest/gtest.h>
#include <operators/basic_join.h>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/project.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include "util/field_utilities.h"
#include <expression/comparator_expression_nodes.h>
#include "expression/generic_expression.h"
#include "expression/math_expression_nodes.h"
#include <operators/packed_table_scan.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43443, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 5, "limit clause for queries");
DEFINE_int32(ctrl_port, 65450, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column, wire_packed or compressed)");


using namespace vaultdb;

class OMPCBasicJoinTest : public EmpBaseTest {
protected:

    std::string src_path_ = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path_ = src_path_ + "/packed_pages/";

    const int customer_limit_ = 10; // smaller: 5

    const int orders_limit_ = 50; // smaller: 30

    const int lineitem_limit_ = 100; // smaller: 90

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




TEST_F(OMPCBasicJoinTest, test_tpch_q3_customer_orders) {


    std::string sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                    "orders_cte AS (" + orders_sql_ + ") "
                      "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                      "FROM customer_cte, orders_cte "
                      "WHERE c_custkey = o_custkey "
                      "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey"; // ignore NOT cdummy AND NOT odummy for now

    // Table scan for orders
    PackedTableScan<emp::Bit> *packed_orders_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, orders_limit_);
    packed_orders_table_scan->setOperatorId(0);

    SecureTable *orders_table = packed_orders_table_scan->run();
    cout << "Orders table ID: " << ((PackedColumnTable *) orders_table)->table_id_ << '\n';
    cout << "Orders first row: " << orders_table->revealRow(0).toString(true) << '\n';


    // Project orders table to o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy
    ExpressionMapBuilder<Bit> orders_builder(packed_orders_table_scan->getOutputSchema());
    orders_builder.addMapping(0, 0);
    orders_builder.addMapping(1, 1);
    orders_builder.addMapping(4, 2);
    orders_builder.addMapping(7, 3);

    Project<Bit> *orders_project = new Project(packed_orders_table_scan, orders_builder.getExprs());
    orders_project->setOperatorId(1);


    // Table scan for customer
    PackedTableScan<emp::Bit> *packed_customer_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "customer", packed_pages_path_, FLAGS_party, customer_limit_);
    packed_customer_table_scan->setOperatorId(2);

    SecureTable *customer_table = packed_customer_table_scan->run();
    cout << "Customer table ID: " << ((PackedColumnTable *) customer_table)->table_id_ << '\n';
    cout << "Customer first row: " << customer_table->revealRow(0).toString(true) << '\n';


    // Project customer table to c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy
    ExpressionMapBuilder<Bit> customer_builder(packed_customer_table_scan->getOutputSchema());
    customer_builder.addMapping(0, 0);

    Project<Bit> *customer_project = new Project(packed_customer_table_scan, customer_builder.getExprs());
    customer_project->setOperatorId(3);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_project, 1, customer_project, 4);

    SecureTable *op_table = orders_project->getOutput();
    SecureTable *cp_table = customer_project->getOutput();
    cout << "op table id: " << ((PackedColumnTable *) op_table)->table_id_ << '\n';
    cout << "cp table id: " << ((PackedColumnTable *) cp_table)->table_id_ << '\n';

    cout << "Orders projected first row: " << op_table->revealRow(0).toString(true) << '\n';
    cout << "Customer projected first row: " << cp_table->revealRow(0).toString(true) << '\n';

    BasicJoin<emp::Bit> *join = new BasicJoin(orders_project, customer_project, predicate);
    join->setOperatorId(4);
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


TEST_F(OMPCBasicJoinTest, test_tpch_q3_lineitem_orders) {


    // get inputs from local oblivious ops
    // first 3 customers, propagate this constraint up the join tree for the test
    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
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

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lineitem_project, 0,
                                                                                      orders_project, 2);

    BasicJoin<emp::Bit> *join = new BasicJoin(lineitem_project, orders_project, predicate);
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



TEST_F(OMPCBasicJoinTest, test_tpch_q3_lineitem_orders_customer) {

    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                           "lineitem_cte AS (" + lineitem_sql_ + "), "
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
    BasicJoin<Bit> *customer_orders_join = new BasicJoin(orders_project, customer_project, customer_orders_predicate);
    customer_orders_join->setOperatorId(-2);

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
    Expression<emp::Bit> * lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>( lineitem_project, 0, customer_orders_join, 2);
    BasicJoin<Bit> *full_join = new BasicJoin(lineitem_project, customer_orders_join, lineitem_orders_predicate);
    full_join->setOperatorId(-2);
    SecureTable *join_res = full_join->run();

    if(FLAGS_validation) {
        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(full_join->getOutputSchema().getFieldCount());
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


