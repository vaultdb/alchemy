#include <operators/secure_sql_input.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <operators/sort.h>
#include <operators/filter.h>
#include <operators/basic_join.h>
#include "util/field_utilities.h"
#include <query_table/secure_tuple.h>
#include <expression/comparator_expression_nodes.h>
#include "expression/generic_expression.h"
#include "expression/math_expression_nodes.h"
#include <operators/packed_table_scan.h>
#include <query_table/packed_column_table.h>
#include <test/ompc/ompc_base_test.h>

#if __has_include("emp-rescu/emp-rescu.h")


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54345, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(cutoff, 10, "limit clause for queries");
DEFINE_int32(ctrl_port, 65465, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wire_path, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");
DEFINE_int32(unpacked_page_size_bits, 2048, "unpacked page size in bits");
DEFINE_int32(page_cnt, 50, "number of pages in buffer pool");



using namespace vaultdb;

class OMPCSortTest : public OmpcBaseTest {
protected:

    const std::string src_path_ = Utilities::getCurrentWorkingDirectory();
    const std::string packed_pages_path_ = src_path_ + "/packed_pages/";

};



TEST_F(OMPCSortTest, tpchQ01Sort) {

    std::string limit_sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(FLAGS_cutoff);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";

    SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING),
                            ColumnSort(1, SortDirection::ASCENDING)};

    PackedTableScan<emp::Bit> *packed_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, FLAGS_cutoff);
    packed_table_scan->setOperatorId(-2);

    ExpressionMapBuilder<Bit> builder(packed_table_scan->getOutputSchema());
    builder.addMapping(8, 0);
    builder.addMapping(9, 1);

    Project<Bit> *project = new Project(packed_table_scan, builder.getExprs());
    project->setOperatorId(-2);

    Sort<emp::Bit> *sort = new Sort(project, sort_def);
    sort->setOperatorId(-2);

    SecureTable *sorted = sort->run();

    if(FLAGS_validation) {
        PlainTable *observed = sorted->revealInsecure();
        DataUtilities::removeDummies(observed);
        ASSERT_TRUE(DataUtilities::verifyCollation(observed));

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
    }


}

TEST_F(OMPCSortTest, tpchQ03Sort) {

    int lineitem_limit = 30;
    int orders_limit = 10;

    std::string lineitem_sql_ = "SELECT  l_orderkey, l_linenumber, l_extendedprice * (1 - l_discount) revenue \n"
                                "FROM lineitem \n"
                                "ORDER BY l_orderkey, l_linenumber \n"
                                "LIMIT " + std::to_string(lineitem_limit);

    std::string orders_sql_ = "SELECT o_orderkey, o_shippriority \n"
                              "FROM orders \n"
                              "ORDER BY o_orderkey \n"
                              "LIMIT " + std::to_string(orders_limit);

    std::string sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
                           "lineitem_cte AS (" + lineitem_sql_ + ") "
                      "SELECT * "
                      "FROM lineitem_cte, orders_cte "
                      "WHERE l_orderkey = o_orderkey "
                      "ORDER BY revenue DESC, o_shippriority";

    SortDefinition sort_def{ColumnSort (2, SortDirection::DESCENDING), // revenue
                            ColumnSort (4, SortDirection::ASCENDING)}; // o_shippriority

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table to l_orderkey, l_linenumber, l_extendedprice * (1 - l_discount) revenue
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(0, 0);
    lineitem_builder.addMapping(3, 1);
    InputReference<emp::Bit> *extendedprice_field = new InputReference<emp::Bit>(5, packed_lineitem_table_scan->getOutputSchema());
    InputReference<emp::Bit> *discount_field = new InputReference<emp::Bit>(6, packed_lineitem_table_scan->getOutputSchema());
    LiteralNode<emp::Bit> *one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
    MinusNode<emp::Bit> *one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
    TimesNode<emp::Bit> *extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
    Expression<emp::Bit> *revenue_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "revenue", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(revenue_expr, 2);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);

    // Table scan for orders
    PackedTableScan<emp::Bit> *packed_orders_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, orders_limit);
    packed_orders_table_scan->setOperatorId(-2);

    // Project orders table to o_orderkey, o_shippriority
    ExpressionMapBuilder<Bit> orders_builder(packed_orders_table_scan->getOutputSchema());
    orders_builder.addMapping(0, 0);
    orders_builder.addMapping(7, 1);

    Project<Bit> *orders_project = new Project(packed_orders_table_scan, orders_builder.getExprs());
    orders_project->setOperatorId(-2);

    // join output schema: (lineitem, orders)
    // l_orderkey, l_linenumber, revenue, o_orderkey, o_shippriority
    Expression<emp::Bit> * predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lineitem_project, 0,
                                                                                      orders_project, 3);

    BasicJoin<emp::Bit> *join = new BasicJoin(lineitem_project, orders_project, predicate);
    join->setOperatorId(-2);

    Sort<Bit> *sort = new Sort(join, sort_def);
    sort->setOperatorId(-2);
    SecureTable *sorted = sort->run();

    if(FLAGS_validation) {

        PlainTable *observed = sorted->revealInsecure();
        DataUtilities::removeDummies(observed);

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort_def;

        //     commented out because of floating point comparison issues, validating with *expected instead
        //        ASSERT_TRUE(DataUtilities::verifyCollation(observed));
        ASSERT_EQ(*expected, *observed);
    }

}


TEST_F(OMPCSortTest, tpchQ05Sort) {

    std::string limit_sql = "SELECT l_orderkey, l_linenumber, l_extendedprice * (1 - l_discount) revenue FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(FLAGS_cutoff);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY revenue DESC";

    SortDefinition sort_def{ColumnSort (2, SortDirection::DESCENDING)};

    PackedTableScan<emp::Bit> *packed_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, FLAGS_cutoff);
    packed_table_scan->setOperatorId(-2);

    ExpressionMapBuilder<Bit> builder(packed_table_scan->getOutputSchema());
    builder.addMapping(0, 0);
    builder.addMapping(3, 1);
    InputReference<emp::Bit> *extendedprice_field = new InputReference<emp::Bit>(5, packed_table_scan->getOutputSchema());
    InputReference<emp::Bit> *discount_field = new InputReference<emp::Bit>(6, packed_table_scan->getOutputSchema());
    LiteralNode<emp::Bit> *one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
    MinusNode<emp::Bit> *one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
    TimesNode<emp::Bit> *extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
    Expression<emp::Bit> *revenue_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "revenue", FieldType::SECURE_FLOAT);
    builder.addExpression(revenue_expr, 2);

    Project<Bit> *project = new Project(packed_table_scan, builder.getExprs());
    project->setOperatorId(-2);

    Sort<Bit> *sort = new Sort(project, sort_def);
    SecureTable *sorted = sort->run();

    if(FLAGS_validation) {

        PlainTable *observed  = sorted->revealInsecure();
        DataUtilities::removeDummies(observed);

        ASSERT_TRUE(DataUtilities::verifyCollation(observed));

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = observed->order_by_;

        ASSERT_EQ(*expected, *observed);
    }

}



TEST_F(OMPCSortTest, tpchQ08Sort) {

    std::string limit_sql = "SELECT o_orderyear, o_orderkey FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(FLAGS_cutoff);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY o_orderyear, o_orderkey DESC";

    SortDefinition sort_def {ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};

    PackedTableScan<emp::Bit> *packed_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, FLAGS_cutoff);
    packed_table_scan->setOperatorId(-2);

    ExpressionMapBuilder<Bit> builder(packed_table_scan->getOutputSchema());
    builder.addMapping(9, 0);
    builder.addMapping(0, 1);

    Project<Bit> *project = new Project(packed_table_scan, builder.getExprs());
    project->setOperatorId(-2);

    Sort<Bit> *sort = new Sort<emp::Bit>(project, sort_def);
    SecureTable *sorted = sort->run();

    if(FLAGS_validation) {
        PlainTable *observed  = sorted->revealInsecure();
        DataUtilities::removeDummies(observed);
        ASSERT_TRUE(DataUtilities::verifyCollation(observed));

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = observed->order_by_;

        ASSERT_EQ(*expected, *observed);
    }
}

// this test is intentionally set to a large input size to test join scaling
// outside of cutoff
// sort scale up has surfaced many bugs in the past, thus having it larger than most.
TEST_F(OMPCSortTest, tpchQ09Sort) {

    int lineitem_limit = 5;

    int orders_limit = 8;

    int supplier_limit = 3;

    int nation_limit = 2;


    std::string lineitem_sql = "SELECT l_orderkey, l_suppkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(lineitem_limit);

    std::string orders_sql = "SELECT o_orderkey, o_orderyear FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(orders_limit);

    std::string supplier_sql = "SELECT s_suppkey, s_nationkey FROM supplier ORDER BY s_suppkey LIMIT " + std::to_string(supplier_limit);

    std::string nation_sql = "SELECT n_nationkey, n_name FROM nation ORDER BY n_nationkey LIMIT " + std::to_string(nation_limit);

    std::string sql = "WITH orders_cte AS (" + orders_sql + "), \n"
                           "lineitem_cte AS (" + lineitem_sql + "), \n"
                           "supplier_cte AS (" + supplier_sql + "), \n"
                           "nation_cte AS (" + nation_sql + ") \n"
                      "SELECT * \n"
                      "FROM lineitem_cte JOIN orders_cte o  ON l_orderkey  = o_orderkey \n"
                                      "  JOIN supplier_cte  ON l_suppkey   = s_suppkey \n"
                                      "  JOIN nation_cte    ON s_nationkey = n_nationkey \n"
                      "ORDER BY o_orderyear, o_orderkey, n_name";

    SortDefinition sort_definition{ColumnSort(1, SortDirection::ASCENDING),
                                   ColumnSort(0, SortDirection::ASCENDING),
                                   ColumnSort(7, SortDirection::ASCENDING)};

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table to l_orderkey, l_suppkey
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(0, 0);
    lineitem_builder.addMapping(2, 1);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);

    // Table scan for orders
    PackedTableScan<emp::Bit> *packed_orders_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, orders_limit);
    packed_orders_table_scan->setOperatorId(-2);

    // Project orders table to o_orderkey, o_orderyear
    ExpressionMapBuilder<Bit> orders_builder(packed_orders_table_scan->getOutputSchema());
    orders_builder.addMapping(0, 0);
    orders_builder.addMapping(9, 1);

    Project<Bit> *orders_project = new Project(packed_orders_table_scan, orders_builder.getExprs());
    orders_project->setOperatorId(-2);

    // join output schema: (lineitem, orders)
    // l_orderkey, l_suppkey, o_orderkey, o_orderyear
    Expression<emp::Bit> * lo_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lineitem_project, 0,
                                                                                         orders_project, 2);

    BasicJoin<emp::Bit> *lo_join = new BasicJoin(lineitem_project, orders_project, lo_predicate);
    lo_join->setOperatorId(-2);

    // Table scan for supplier
    PackedTableScan<emp::Bit> *packed_supplier_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "supplier", packed_pages_path_, FLAGS_party, supplier_limit);
    packed_supplier_table_scan->setOperatorId(-2);

    // Project supplier table to s_suppkey, s_nationkey
    ExpressionMapBuilder<Bit> supplier_builder(packed_supplier_table_scan->getOutputSchema());
    supplier_builder.addMapping(0, 0);
    supplier_builder.addMapping(3, 1);

    Project<Bit> *supplier_project = new Project(packed_supplier_table_scan, supplier_builder.getExprs());
    supplier_project->setOperatorId(-2);

    // join output schema: (lineitem, orders, supplier)
    // l_orderkey, l_suppkey, o_orderkey, o_orderyear, s_suppkey, s_nationkey
    Expression<emp::Bit> * los_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(lo_join, 1,
                                                                                          supplier_project, 4);

    BasicJoin<emp::Bit> *los_join = new BasicJoin(lo_join, supplier_project, los_predicate);
    los_join->setOperatorId(-2);

    // Table scan for nation
    PackedTableScan<emp::Bit> *packed_nation_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "nation", packed_pages_path_, FLAGS_party, nation_limit);
    packed_nation_table_scan->setOperatorId(-2);

    // Project nation table to n_nationkey, n_name
    ExpressionMapBuilder<Bit> nation_builder(packed_nation_table_scan->getOutputSchema());
    nation_builder.addMapping(0, 0);
    nation_builder.addMapping(1, 1);

    Project<Bit> *nation_project = new Project(packed_nation_table_scan, nation_builder.getExprs());
    nation_project->setOperatorId(-2);

    // join output schema: (lineitem, orders, supplier, nation)
    // l_orderkey, l_suppkey, o_orderkey, o_orderyear, s_suppkey, s_nationkey, n_nationkey, n_name
    Expression<emp::Bit> * losn_predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(los_join, 5,
                                                                                           nation_project, 6);

    BasicJoin<emp::Bit> *losn_join = new BasicJoin(los_join, nation_project, losn_predicate);
    losn_join->setOperatorId(-2);

    Sort<Bit> *sort = new Sort(losn_join, sort_definition);
    sort->setOperatorId(-2);
    SecureTable *sorted = sort->run();

    if(FLAGS_validation) {
        PlainTable *observed = sorted->revealInsecure();
        DataUtilities::removeDummies(observed);
        ASSERT_TRUE(DataUtilities::verifyCollation(observed));

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = observed->order_by_;

        ASSERT_EQ(*expected, *observed);
    }
}

// 18
TEST_F(OMPCSortTest, tpchQ18Sort) {

    std::string limit_sql = "SELECT o_orderkey, o_orderdate, o_totalprice, o_custkey FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(FLAGS_cutoff);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY o_custkey, o_orderkey";

    SortDefinition sort_def {ColumnSort(3, SortDirection::ASCENDING), ColumnSort(0, SortDirection::ASCENDING)};

    PackedTableScan<emp::Bit> *packed_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "orders", packed_pages_path_, FLAGS_party, FLAGS_cutoff);
    packed_table_scan->setOperatorId(-2);

    ExpressionMapBuilder<Bit> builder(packed_table_scan->getOutputSchema());
    builder.addMapping(0, 0);
    builder.addMapping(4, 1);
    builder.addMapping(3, 2);
    builder.addMapping(1, 3);

    Project<Bit> *project = new Project(packed_table_scan, builder.getExprs());
    project->setOperatorId(-2);

    Sort<Bit> *sort = new Sort(project, sort_def);
    SecureTable *sorted = sort->run();

    if(FLAGS_validation) {
        PlainTable *observed = sorted->revealInsecure();
        DataUtilities::removeDummies(observed);

        ASSERT_TRUE(DataUtilities::verifyCollation(observed));

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = observed->order_by_;

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

