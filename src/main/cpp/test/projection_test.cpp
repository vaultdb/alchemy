//
// Created by Jennie Rogers on 9/2/20.
//


#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>


using namespace emp;
using namespace vaultdb::types;



class ProjectionTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};




// unencrypted case
class RevenueExpression : public Expression {
    Value oneValue;
public:
    RevenueExpression() : Expression("revenue", types::TypeId::FLOAT32) {
        oneValue = Value((int32_t) 1);
    }

    ~RevenueExpression() {}

    types::Value expressionCall(const QueryTuple & aTuple) const  {
        Value extendedPrice = aTuple.getField(5).getValue();
        Value discount = aTuple.getField(6).getValue();

        // l.l_extendedprice * (1 - l.l_discount)
        return extendedPrice * (oneValue - discount);
    }


};


// variant of Q3 expressions
TEST_F(ProjectionTest, q3Lineitem) {
    std::string srcSql = "SELECT * FROM lineitem ORDER BY l_comment LIMIT 10";
    std::string expectedOutputSql = "SELECT l_orderkey, l_shipdate,  l.l_extendedprice * (1 - l.l_discount) revenue FROM (" + srcSql + ") src ";
    std::shared_ptr<Operator> input(new SqlInput("tpch_alice", srcSql, false));

    ProjectionDefinition projectionDefinition;

    ProjectionMapping orderkey(0,0);
    ProjectionMapping shipdate(10, 1);
    Expression revenue = RevenueExpression();

    projectionDefinition[0] = ColumnProjection(orderkey);
    projectionDefinition[1] = ColumnProjection(shipdate);
    projectionDefinition[2] = ColumnProjection(revenue);




}

