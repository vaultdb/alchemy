
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <query_table/field/field_factory.h>
#include <expression/generic_expression.h>
#include <expression/comparator_expression_nodes.h>
#include "plain_base_test.h"


class FilterTest :  public PlainBaseTest  { };





TEST_F(FilterTest, test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";

    SqlInput input(db_name_, sql, false);

    std::shared_ptr<PlainTable > output = input.run(); // a smoke test for the operator infrastructure
    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, sql, false);

    ASSERT_EQ(*expected, *output);



}




TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";
    std::string expectedResultSql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";


   std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expectedResultSql, true);

   // expression setup
    // l_linenumber == 1
   InputReferenceNode<bool> *read_field = new InputReferenceNode<bool>(1);
   LiteralNode<bool> *constant_input = new LiteralNode<bool>(Field<bool>(FieldType::INT, 1));
   ExpressionNode<bool> *equality_check = new EqualNode<bool>((ExpressionNode<bool> *) read_field, (ExpressionNode<bool> *) constant_input);

   GenericExpression<bool> *expression = new GenericExpression<bool>(equality_check, "predicate", FieldType::BOOL);

    SqlInput input(db_name_, sql, false);
    Filter<bool> filter(&input, expression);


    std::shared_ptr<PlainTable > result = filter.run();


    ASSERT_EQ(*expected,  *result);

}

