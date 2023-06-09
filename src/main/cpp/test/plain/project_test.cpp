#include "plain_base_test.h"
#include <util/type_utilities.h>
#include <operators/sql_input.h>
#include <operators/project.h>
#include <expression/function_expression.h>
#include <expression/generic_expression.h>
#include <expression/expression_node.h>
#include <expression/math_expression_nodes.h>



class ProjectionTest : public PlainBaseTest { };


// l.l_extendedprice * (1 - l.l_discount)
GenericExpression<bool> *getRevenueExpression(const QuerySchema &input) {
    InputReference<bool>  extended_price(5, input);
    InputReference<bool> discount(6, input);
    LiteralNode<bool> one(Field<bool>(FieldType::FLOAT, (float_t) 1.0));

    MinusNode<bool> minus(&one, &discount);
    TimesNode<bool> times(&extended_price, &minus);

    return new GenericExpression<bool>(&times, "revenue", FieldType::FLOAT);


}


// variant of Q3 expressions
TEST_F(ProjectionTest, q3Lineitem) {
    std::string sql = "SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::string expected_sql = "SELECT l_orderkey, " + DataUtilities::queryDatetime("l_shipdate") + ",  l_extendedprice * (1 - l_discount) revenue FROM (" + sql + ") src ";

    PlainTable *expected =  DataUtilities::getQueryResults(db_name_, expected_sql, false);


    SqlInput *input = new SqlInput(db_name_, sql, false);


    ExpressionMapBuilder<bool> builder(input->getOutputSchema());
    builder.addMapping(0, 0);
    builder.addMapping(10, 1);

    Expression<bool> *revenue_expression = getRevenueExpression(input->getOutputSchema());

    builder.addExpression(revenue_expression, 2);
    Project project(input, builder.getExprs());

    PlainTable *observed = project.run();



    ASSERT_EQ(*expected, *observed);

    delete expected;
}

