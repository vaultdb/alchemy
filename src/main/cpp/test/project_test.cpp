#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/project.h>


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

PlainField calculateRevenue(const PlainTuple & aTuple) {
    const PlainField extendedPrice = aTuple.getField(5);
    const PlainField discount = aTuple.getField(6);
    const PlainField one(FieldType::FLOAT, (float_t) 1.0);

    // l.l_extendedprice * (1 - l.l_discount)
    return extendedPrice * (one - discount);
}

// variant of Q3 expressions
TEST_F(ProjectionTest, q3Lineitem) {
    std::string srcSql = "SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::string expectedOutputSql = "SELECT l_orderkey, " + DataUtilities::queryDatetime("l_shipdate") + ",  l_extendedprice * (1 - l_discount) revenue FROM (" + srcSql + ") src ";

    std::shared_ptr<PlainTable > expected =  DataUtilities::getQueryResults("tpch_alice", expectedOutputSql, false);


    SqlInput input("tpch_alice", srcSql, false);

    Project project(&input);

    Expression<bool> revenueExpression(&calculateRevenue, "revenue", FieldType::FLOAT);



    project.addColumnMapping(0, 0);
    project.addColumnMapping(10, 1);
    project.addExpression(revenueExpression, 2);



    std::shared_ptr<PlainTable > observed = project.run();



    ASSERT_EQ(*expected, *observed);
}

