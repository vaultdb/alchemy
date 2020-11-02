#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/project.h>


using namespace emp;
using namespace vaultdb::types;



class ProjectionTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};




// unencrypted case
/*class RevenueExpression : public Expression {
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

    // needed for boost::variant
    RevenueExpression& operator=(const RevenueExpression & src) {
        this->alias = src.getAlias();
        this->expressionType = types::TypeId::FLOAT32;

    }

};
*/

types::Value calculateRevenue(const QueryTuple & aTuple) {
    Value extendedPrice = aTuple.getField(5).getValue();
    Value discount = aTuple.getField(6).getValue();

    // l.l_extendedprice * (1 - l.l_discount)
    return extendedPrice * (Value((float) 1.0) - discount);
}

// variant of Q3 expressions
TEST_F(ProjectionTest, q3Lineitem) {
    std::string srcSql = "SELECT * FROM lineitem ORDER BY l_comment LIMIT 10";
    std::string expectedOutputSql = "SELECT l_orderkey, " + DataUtilities::queryDatetime("l_shipdate") + ",  l_extendedprice * (1 - l_discount) revenue FROM (" + srcSql + ") src ";

    std::shared_ptr<QueryTable> expected =  DataUtilities::getQueryResults("tpch_alice", expectedOutputSql, false);


    std::cout << "Expected query results:\n" << *expected << std::endl;


    std::shared_ptr<Operator> input(new SqlInput("tpch_alice", srcSql, false));

    Project *projectOp = new Project(input);
    // each op creates a shared_ptr to this
    std::shared_ptr<Operator> project = projectOp->getPtr();

    Expression revenueExpression(&calculateRevenue, "revenue", TypeId::FLOAT32);



    projectOp->addColumnMapping(0, 0);
    projectOp->addColumnMapping(10, 1);
    projectOp->addExpression(revenueExpression, 2);



    std::shared_ptr<QueryTable> observed = project->run();

    std::cout << "Observed query results: \n" << *observed << std::endl;


    ASSERT_EQ(*expected, *observed);
}

