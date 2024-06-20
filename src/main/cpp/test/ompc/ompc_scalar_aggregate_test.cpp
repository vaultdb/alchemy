#include <util/data_utilities.h>
#include <operators/operator.h>
#include <operators/support/aggregate_id.h>
#include <operators/secure_sql_input.h>
#include <operators/scalar_aggregate.h>
#include <opt/operator_cost_model.h>
#include "expression/generic_expression.h"
#include "expression/math_expression_nodes.h"
#include <operators/packed_table_scan.h>
#include <test/ompc/ompc_base_test.h>

#if __has_include("emp-rescu/emp-rescu.h")



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54312, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65466, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wires, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");


using namespace vaultdb;

class OMPCScalarAggregateTest : public OmpcBaseTest {

protected:
    void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;

    std::string src_path_ = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path_ = src_path_ + "/packed_pages/";

    const int lineitem_limit_ = 50;

    const std::string lineitem_sql_ = "SELECT l_orderkey, l_linenumber \n"
                                      "FROM lineitem \n"
                                      "ORDER BY l_orderkey \n"
                                      "LIMIT " + std::to_string(lineitem_limit_);
};

void OMPCScalarAggregateTest::runTest(const string &expected_sql,
                                        const vector<ScalarAggregateDefinition> & aggregators) const {

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit_);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table to l_orderkey, l_linenumber
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(0, 0);
    lineitem_builder.addMapping(3, 1);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);


    // provide the aggregator with inputs:
    ScalarAggregate<Bit> *aggregate = new ScalarAggregate(lineitem_project, aggregators);
    aggregate->setOperatorId(-2);
    auto aggregated = aggregate->run();

    if(FLAGS_validation) {
        PlainTable *observed = aggregated->reveal();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        ASSERT_EQ(*expected, *observed);
    }

}

TEST_F(OMPCScalarAggregateTest, test_count) {
    std::string expected_sql = "WITH input AS (" + lineitem_sql_ + ") SELECT COUNT(*)::BIGINT cnt FROM input";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expected_sql, aggregators);

}

TEST_F(OMPCScalarAggregateTest, test_min) {
    std::string expected_sql = "WITH input AS (" + lineitem_sql_ + ") SELECT MIN(l_linenumber) min_lineno FROM input";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(OMPCScalarAggregateTest, test_max) {
    std::string expected_sql = "WITH input AS (" + lineitem_sql_ + ") SELECT MAX(l_linenumber) max_lineno FROM input";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(OMPCScalarAggregateTest, test_sum) {
    std::string expected_sql = "WITH input AS (" + lineitem_sql_ + ") SELECT SUM(l_linenumber)::INTEGER sum_lineno FROM input";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};
    runTest(expected_sql, aggregators);

}

TEST_F(OMPCScalarAggregateTest, test_avg) {
    std::string expected_sql = "WITH input AS (" + lineitem_sql_ + ") SELECT FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM input";
    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(OMPCScalarAggregateTest, test_tpch_q1_sums) {

    std::string lineitem_sql = "SELECT * \n"
                                "FROM lineitem \n"
                                "ORDER BY l_orderkey \n"
                                "LIMIT " + std::to_string(lineitem_limit_);

    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, "
                        "l_extendedprice * (1.0 - l_discount) AS disc_price, "
                        "l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge "
                        " FROM (" + lineitem_sql + ") selection";


    string expected_sql = "SELECT SUM(l_quantity) sum_qty, "
                          "SUM(l_extendedprice) sum_base_price, "
                          "SUM(disc_price) sum_disc_price, "
                          "SUM(charge) sum_charge "
                          "FROM (" + inputQuery + ") subquery";

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit_);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(8, 0);
    lineitem_builder.addMapping(9, 1);
    lineitem_builder.addMapping(4, 2);
    lineitem_builder.addMapping(5, 3);
    lineitem_builder.addMapping(6, 4);
    // l_extendedprice * (1.0 - l_discount) AS disc_price
    InputReference<emp::Bit> *extendedprice_field = new InputReference<emp::Bit>(5, packed_lineitem_table_scan->getOutputSchema());
    InputReference<emp::Bit> *discount_field = new InputReference<emp::Bit>(6, packed_lineitem_table_scan->getOutputSchema());
    LiteralNode<emp::Bit> *one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
    MinusNode<emp::Bit> *one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
    TimesNode<emp::Bit> *extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
    Expression<emp::Bit> *disc_price_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "disc_price", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(disc_price_expr, 5);
    // l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge
    InputReference<emp::Bit> *tax_field = new InputReference<emp::Bit>(7, packed_lineitem_table_scan->getOutputSchema());
    PlusNode<emp::Bit> *one_plus_tax = new PlusNode<emp::Bit>(one_literal, tax_field);
    TimesNode<emp::Bit> *times_tax = new TimesNode<emp::Bit>(extended_price_times_discount, one_plus_tax);
    Expression<emp::Bit> *charge_expr = new GenericExpression<emp::Bit>(times_tax, "charge", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(charge_expr, 6);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};

    ScalarAggregate<emp::Bit> *aggregate = new ScalarAggregate(lineitem_project, aggregators);
    aggregate->setOperatorId(-2);
    SecureTable *aggregated = aggregate->run();

    if(FLAGS_validation) {
        PlainTable *observed = aggregated->reveal();
        DataUtilities::removeDummies(observed);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);

        ASSERT_EQ(*expected, *observed);
    }

}

TEST_F(OMPCScalarAggregateTest, test_tpch_q1_avg_cnt) {
    std::string lineitem_sql = "SELECT * \n"
                                "FROM lineitem \n"
                                "ORDER BY l_orderkey \n"
                                "LIMIT " + std::to_string(lineitem_limit_);

    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge \n"
                 " FROM (" + lineitem_sql + ") selection";

    string expected_sql =  "select \n"
                           "  avg(l_quantity) as avg_qty, \n"
                           "  avg(l_extendedprice) as avg_price, \n"
                           "  avg(l_discount) as avg_disc, \n"
                           "  count(*)::BIGINT as count_order \n"
                           "from (" + sql + ") subq\n";

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit_);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(8, 0);
    lineitem_builder.addMapping(9, 1);
    lineitem_builder.addMapping(4, 2);
    lineitem_builder.addMapping(5, 3);
    lineitem_builder.addMapping(6, 4);
    // l_extendedprice * (1.0 - l_discount) AS disc_price
    InputReference<emp::Bit> *extendedprice_field = new InputReference<emp::Bit>(5, packed_lineitem_table_scan->getOutputSchema());
    InputReference<emp::Bit> *discount_field = new InputReference<emp::Bit>(6, packed_lineitem_table_scan->getOutputSchema());
    LiteralNode<emp::Bit> *one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
    MinusNode<emp::Bit> *one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
    TimesNode<emp::Bit> *extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
    Expression<emp::Bit> *disc_price_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "disc_price", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(disc_price_expr, 5);
    // l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge
    InputReference<emp::Bit> *tax_field = new InputReference<emp::Bit>(7, packed_lineitem_table_scan->getOutputSchema());
    PlusNode<emp::Bit> *one_plus_tax = new PlusNode<emp::Bit>(one_literal, tax_field);
    TimesNode<emp::Bit> *times_tax = new TimesNode<emp::Bit>(extended_price_times_discount, one_plus_tax);
    Expression<emp::Bit> *charge_expr = new GenericExpression<emp::Bit>(times_tax, "charge", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(charge_expr, 6);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);

    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    ScalarAggregate<emp::Bit> *aggregate = new ScalarAggregate(lineitem_project, aggregators);
    aggregate->setOperatorId(-2);
    SecureTable *aggregated = aggregate->run();

    if(FLAGS_validation) {
        PlainTable *observed = aggregated->reveal();
        DataUtilities::removeDummies(observed);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);

        ASSERT_EQ(*expected, *observed);
    }

}

TEST_F(OMPCScalarAggregateTest, tpch_q1) {
    std::string lineitem_sql = "SELECT * \n"
                               "FROM lineitem \n"
                               "ORDER BY l_orderkey \n"
                               "LIMIT " + std::to_string(lineitem_limit_);

    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge \n"
                        " FROM (" + lineitem_sql + ") selection \n";

    string expected_sql =  "select \n"
                           "  sum(l_quantity) as sum_qty, \n"
                           "  sum(l_extendedprice) as sum_base_price, \n"
                           "  sum(disc_price) as sum_disc_price, \n"
                           "  sum(charge) as sum_charge, \n"
                           "  avg(l_quantity) as avg_qty, \n"
                           "  avg(l_extendedprice) as avg_price, \n"
                           "  avg(l_discount) as avg_disc, \n"
                           "  count(*)::BIGINT as count_order \n"
                           "from (" + inputQuery + ") input ";

    // Table scan for lineitem
    PackedTableScan<emp::Bit> *packed_lineitem_table_scan = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path_, FLAGS_party, lineitem_limit_);
    packed_lineitem_table_scan->setOperatorId(-2);

    // Project lineitem table
    ExpressionMapBuilder<Bit> lineitem_builder(packed_lineitem_table_scan->getOutputSchema());
    lineitem_builder.addMapping(8, 0);
    lineitem_builder.addMapping(9, 1);
    lineitem_builder.addMapping(4, 2);
    lineitem_builder.addMapping(5, 3);
    lineitem_builder.addMapping(6, 4);
    // l_extendedprice * (1.0 - l_discount) AS disc_price
    InputReference<emp::Bit> *extendedprice_field = new InputReference<emp::Bit>(5, packed_lineitem_table_scan->getOutputSchema());
    InputReference<emp::Bit> *discount_field = new InputReference<emp::Bit>(6, packed_lineitem_table_scan->getOutputSchema());
    LiteralNode<emp::Bit> *one_literal = new LiteralNode(Field<Bit>(FieldType::SECURE_FLOAT, emp::Float(1.0)));
    MinusNode<emp::Bit> *one_minus_discount = new MinusNode<emp::Bit>(one_literal, discount_field);
    TimesNode<emp::Bit> *extended_price_times_discount = new TimesNode<emp::Bit>(extendedprice_field, one_minus_discount);
    Expression<emp::Bit> *disc_price_expr = new GenericExpression<emp::Bit>(extended_price_times_discount, "disc_price", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(disc_price_expr, 5);
    // l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge
    InputReference<emp::Bit> *tax_field = new InputReference<emp::Bit>(7, packed_lineitem_table_scan->getOutputSchema());
    PlusNode<emp::Bit> *one_plus_tax = new PlusNode<emp::Bit>(one_literal, tax_field);
    TimesNode<emp::Bit> *times_tax = new TimesNode<emp::Bit>(extended_price_times_discount, one_plus_tax);
    Expression<emp::Bit> *charge_expr = new GenericExpression<emp::Bit>(times_tax, "charge", FieldType::SECURE_FLOAT);
    lineitem_builder.addExpression(charge_expr, 6);

    Project<Bit> *lineitem_project = new Project(packed_lineitem_table_scan, lineitem_builder.getExprs());
    lineitem_project->setOperatorId(-2);

    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    ScalarAggregate<emp::Bit> *aggregate = new ScalarAggregate(lineitem_project, aggregators);
    aggregate->setOperatorId(-2);
    SecureTable *aggregated = aggregate->run();

    if(FLAGS_validation) {
        PlainTable *observed = aggregated->reveal();
        DataUtilities::removeDummies(observed);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);

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
