
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <query_table/field/field_factory.h>
#include <expression/generic_expression.h>
#include <expression/comparator_expression_nodes.h>
#include "plain_base_test.h"

DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column or compressed)");


class FilterTest :  public PlainBaseTest  { };





TEST_F(FilterTest, test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT " + std::to_string(FLAGS_cutoff);

    SqlInput input(db_name_, sql, false);

    PlainTable *output = input.run(); // a smoke test for the operator infrastructure
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, sql, false);

    ASSERT_EQ(*expected, *output);

    delete expected;

}




TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT " + std::to_string(FLAGS_cutoff);
    std::string expected_sql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";


   PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql,  true);
   auto input = new SqlInput(db_name_, sql, false);

   // expression setup
    // l_linenumber == 1
   auto read_field = new InputReference<bool>(1, input->getOutputSchema());
   auto constant_input = new LiteralNode<bool>(Field<bool>(FieldType::INT, 1));
   auto equality_check = new EqualNode<bool>((ExpressionNode<bool> *) read_field, (ExpressionNode<bool> *) constant_input);

   auto expression = new GenericExpression<bool>    (equality_check, "predicate", FieldType::BOOL);

    Filter<bool> filter(input, expression);


    PlainTable *result = filter.run();


    ASSERT_EQ(*expected,  *result);

    delete expected;

    delete equality_check;
    delete constant_input;
    delete read_field;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;
}

