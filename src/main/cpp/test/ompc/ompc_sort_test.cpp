#include <operators/secure_sql_input.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <test/mpc/emp_base_test.h>
#include <operators/sort.h>
#include <operators/filter.h>
#include <query_table/secure_tuple.h>
#include <expression/comparator_expression_nodes.h>
#include "expression/generic_expression.h"
#include "expression/math_expression_nodes.h"
#include <operators/packed_table_scan.h>
#include <query_table/packed_column_table.h>




DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54345, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 10, "limit clause for queries");
DEFINE_int32(ctrl_port, 65465, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column, wire_packed or compressed)");


using namespace vaultdb;

class OMPCSortTest : public EmpBaseTest {
protected:



};



TEST_F(OMPCSortTest, tpchQ01Sort) {

    int limit = 100;
    std::string limit_sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(limit);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";

    SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING),
                            ColumnSort(1, SortDirection::ASCENDING)};

    std::string src_path = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path = src_path + "/packed_pages/";

    PackedTableScan *packed_table_scan = new PackedTableScan("tpch_unioned_150", "lineitem", packed_pages_path, FLAGS_party, limit);
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


// TODO: wait until join tests passed.
//TEST_F(OMPCSortTest, tpchQ03Sort) {
//
//    string sql = "SELECT l_orderkey, l_linenumber, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey WHERE l_orderkey <= "  + std::to_string(FLAGS_cutoff) +  " ORDER BY l_comment";
//
//    SortDefinition sort_def{ColumnSort (2, SortDirection::DESCENDING), // revenue
//                            ColumnSort (4, SortDirection::ASCENDING)}; // o_orderdate
//
//    auto input = new SecureSqlInput(db_name_, sql, false);
//    Sort<Bit> sort(input, sort_def);
//    auto sorted = sort.run();
//
//    if(FLAGS_validation) {
//
//        PlainTable *observed = sorted->revealInsecure();
//        DataUtilities::removeDummies(observed);
//
//        string expected_sql = "WITH input AS (" + sql + ") SELECT l_orderkey, l_linenumber, revenue, o_shippriority, " + DataUtilities::queryDatetime("o_orderdate") + " FROM input ORDER BY revenue DESC, o_orderdate";
//
//        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
//        expected->order_by_ = sort_def;
//
//        //     commented out because of floating point comparison issues, validating with *expected instead
//        //        ASSERT_TRUE(DataUtilities::verifyCollation(observed));
//        ASSERT_EQ(*expected, *observed);
//
//        delete expected;
//        delete observed;
//
//    }
//
//}


TEST_F(OMPCSortTest, tpchQ05Sort) {

    int limit = 100;
    std::string limit_sql = "SELECT l_orderkey, l_linenumber, l_extendedprice * (1 - l_discount) revenue FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(limit);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY revenue DESC";

    SortDefinition sort_def{ColumnSort (2, SortDirection::DESCENDING)};

    std::string src_path = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path = src_path + "/packed_pages/";

    PackedTableScan *packed_table_scan = new PackedTableScan("tpch_unioned_150", "lineitem", packed_pages_path, FLAGS_party, limit);
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

    int limit = 100;
    std::string limit_sql = "SELECT o_orderyear, o_orderkey FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(limit);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY o_orderyear, o_orderkey DESC";

    SortDefinition sort_def {ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};

    std::string src_path = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path = src_path + "/packed_pages/";

    PackedTableScan *packed_table_scan = new PackedTableScan("tpch_unioned_150", "orders", packed_pages_path, FLAGS_party, limit);
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


// TODO: wait until join tests passed.
// this test is intentionally set to a large input size to test join scaling
// outside of cutoff
// sort scale up has surfaced many bugs in the past, thus having it larger than most.
//TEST_F(OMPCSortTest, tpchQ09Sort) {
//
//    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
//                      "  JOIN supplier s ON s_suppkey = l_suppkey"
//                      "  JOIN nation on n_nationkey = s_nationkey"
//                      " ORDER BY l_comment LIMIT 1000";
//    SortDefinition sort_definition{ColumnSort(2, SortDirection::ASCENDING),
//                                   ColumnSort(0, SortDirection::DESCENDING)};
//
//
//    auto input = new SecureSqlInput(db_name_, sql, false);
//    Sort sort(input, sort_definition);
//    auto sorted = sort.run();
//
//    if(FLAGS_validation) {
//        PlainTable *observed = sorted->revealInsecure();
//        DataUtilities::removeDummies(observed);
//        DataUtilities::verifyCollation(observed);
//        delete observed;
//    }
//
//
//
//
//}

// 18
TEST_F(OMPCSortTest, tpchQ18Sort) {
    int limit = 100;
    std::string limit_sql = "SELECT o_orderkey, o_orderdate, o_totalprice, o_custkey FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(limit);
    string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY o_custkey, o_orderkey";

    SortDefinition sort_def {ColumnSort(3, SortDirection::ASCENDING), ColumnSort(0, SortDirection::ASCENDING)};

    std::string src_path = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path = src_path + "/packed_pages/";

    PackedTableScan *packed_table_scan = new PackedTableScan("tpch_unioned_150", "orders", packed_pages_path, FLAGS_party, limit);
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




