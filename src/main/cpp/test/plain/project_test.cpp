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
shared_ptr<ExpressionNode<bool> > getRevenueExpression() {
    shared_ptr<ExpressionNode<bool> > extended_price(new InputReferenceNode<bool>(5));
    shared_ptr<ExpressionNode<bool> > discount(new InputReferenceNode<bool>(6));
    shared_ptr<ExpressionNode<bool> > one(new LiteralNode<bool>(Field<bool>(FieldType::FLOAT, (float_t) 1.0)));

    shared_ptr<ExpressionNode<bool> > minus(new MinusNode<bool>(one, discount));

    return shared_ptr<ExpressionNode<bool>> (new TimesNode<bool>(extended_price, minus));


}


// variant of Q3 expressions
TEST_F(ProjectionTest, q3Lineitem) {
    std::string srcSql = "SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::string expectedOutputSql = "SELECT l_orderkey, " + DataUtilities::queryDatetime("l_shipdate") + ",  l_extendedprice * (1 - l_discount) revenue FROM (" + srcSql + ") src ";

    std::shared_ptr<PlainTable > expected =  DataUtilities::getQueryResults(db_name_, expectedOutputSql, false);


    SqlInput input(db_name_, srcSql, false);


    ExpressionMapBuilder<bool> builder(input.getOutputSchema());
    builder.addMapping(0, 0);
    builder.addMapping(10, 1);
    shared_ptr<Expression<bool> > revenue_expression(new GenericExpression<bool>(getRevenueExpression(), "revenue", FieldType::FLOAT));

    builder.addExpression(revenue_expression, 2);
    Project project(&input, builder.getExprs());

    std::shared_ptr<PlainTable > observed = project.run();



    ASSERT_EQ(*expected, *observed);
}

