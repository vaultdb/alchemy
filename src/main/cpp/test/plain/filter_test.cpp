#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <query_table/field/field_factory.h>
#include <expression/generic_expression.h>
#include <expression/comparator_expression_nodes.h>


using namespace emp;
using namespace vaultdb;


class FilterTest : public ::testing::Test {


protected:
    void SetUp() override { setup_plain_prot(false, ""); };
    void TearDown() override{  finalize_plain_prot(); };

    const std::string dbName = "tpch_unioned";
};





TEST_F(FilterTest, test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";

    SqlInput input(dbName, sql, false);

    std::shared_ptr<PlainTable > output = input.run(); // a smoke test for the operator infrastructure
    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, sql, false);

    ASSERT_EQ(*expected, *output);



}




TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";
    std::string expectedResultSql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";


   std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

   // expression setup
    // l_linenumber == 1
    shared_ptr<InputReferenceNode<bool> > read_field(new InputReferenceNode<bool>(1));
   shared_ptr<LiteralNode<bool> > constant_input(new LiteralNode<bool>(Field<bool>(FieldType::INT, 1)));
   shared_ptr<ExpressionNode<bool> > equality_check(new EqualNode<bool>(read_field, constant_input));
   BoolExpression<bool> expression(equality_check);

    SqlInput input(dbName, sql, false);
    Filter<bool> filter(&input, expression);


    std::shared_ptr<PlainTable > result = filter.run();


    ASSERT_EQ(*expected,  *result);

}

