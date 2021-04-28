#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/project.h>
#include <expression/function_expression.h>

#include <expression/generic_expression.h>
#include <expression/expression_node.h>
#include <expression/math_expression_nodes.h>



using namespace emp;
using namespace vaultdb;


class ProjectionTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");
    };

    void TearDown() override{
        finalize_plain_prot();
    };

};




// unencrypted case
/* Deprecated
 *
 * class RevenueExpression : public Expression {
    Field oneValue;
public:
    RevenueExpression() : Expression("revenue", FieldType::FLOAT) {
        oneField = Value((int32_t) 1);
    }

    ~RevenueExpression() {}

    Field expressionCall(const QueryTuple & aTuple) const  {
        Field extendedPrice = aTuple.getField(5).getValue();
        Field discount = aTuple.getField(6).getValue();

        // l.l_extendedprice * (1 - l.l_discount)
        return extendedPrice * (oneField - discount);
    }

    // needed for boost::variant
    RevenueExpression& operator=(const RevenueExpression & src) {
        this->alias = src.getAlias();
        this->expressionType = FieldType::FLOAT;

    }

};
*/

/* version 2
PlainField calculateRevenue(const PlainTuple & aTuple) {
    const PlainField extendedPrice = aTuple.getField(5);
    const PlainField discount = aTuple.getField(6);
    const PlainField one(FieldType::FLOAT, (float_t) 1.0);

    // l.l_extendedprice * (1 - l.l_discount)
    return extendedPrice * (one - discount);
} */

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

    std::shared_ptr<PlainTable > expected =  DataUtilities::getQueryResults("tpch_unioned", expectedOutputSql, false);


    SqlInput input("tpch_unioned", srcSql, false);


    Project project(&input);
    shared_ptr<Expression<bool> > revenue_expression(new GenericExpression<bool>(getRevenueExpression(), "revenue", FieldType::FLOAT));


    project.addColumnMapping(0, 0);
    project.addColumnMapping(10, 1);

    project.addInputReference(0, 0);
    project.addInputReference(10, 1);

    project.addExpression(revenue_expression, 2);



    std::shared_ptr<PlainTable > observed = project.run();



    ASSERT_EQ(*expected, *observed);
}

