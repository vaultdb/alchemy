
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <query_table/field/field_factory.h>
#include <expression/generic_expression.h>
#include <expression/comparator_expression_nodes.h>
#include "plain_base_test.h"

DEFINE_string(storage, "row", "storage model for tables (row or column)");


class FilterTest :  public PlainBaseTest  { };





TEST_F(FilterTest, test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";

    SqlInput input(db_name_, sql, storage_model_, false);

    PlainTable *output = input.run(); // a smoke test for the operator infrastructure
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, sql, storage_model_, false);

    ASSERT_EQ(*expected, *output);

    delete expected;

}




TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";
    std::string expected_sql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";


   PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, storage_model_, true);
   auto input = new SqlInput(db_name_, sql, storage_model_, false);

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

