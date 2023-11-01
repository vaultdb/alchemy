#include "plain_base_test.h"
#include "data/csv_reader.h"
#include "parser/plan_parser.h"
#include "test/support/tpch_queries.h"
#include "parser/plan_deparser.h"
#include <boost/algorithm/string/replace.hpp>
#include "operators/project.h"
#include "operators/join.h"
#include "parser/expression_deparser.h"
#include "parser/expression_parser.h"

DEFINE_string(filter, "*", "run only the tests passing this filter");


class PlanDeparserTest : public PlainBaseTest {


protected:

    // limit input to first N tuples per SQL statement
    int limit_ = 10;

    void runTest(const int &test_id);

    vector<string> search_strs_ = {"\"dummy-tag\"", "\"input-party\"", "\"input-limit\"", "\"field\"", "\"input\"", "\"foreign-key\"", "\"nullable\"", " \"literal\"", "precision", "\"scale\"", "\"read_lhs\"", "\"binary_mode\"", "\"output-cardinality\"", "\"no-op\""};

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void PlanDeparserTest::runTest(const int &test_id) {
    string test_name = "q" + std::to_string(test_id);
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + test_name + ".json";

    PlanParser<bool> plan_reader(db_name_, plan_file, limit_);
    PlainOperator *expected_root = plan_reader.getRoot();

    string json_plan = PlanDeparser<bool>::deparse(expected_root);

    // re-parse the plan, leaving out limit to test it persisting from JSON
    PlainOperator *observed_root = PlanParser<bool>::parseJSONString(db_name_, json_plan);

    ASSERT_EQ(*expected_root, *observed_root);

    delete expected_root;
    delete observed_root;

}



// expression: ($2 == $6) AND ($1 == $5)
TEST_F(PlanDeparserTest, conjunction_expression) {
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q9.json";
    PlanParser<bool> plan_reader(db_name_, plan_file);

    auto join =  (Join<bool> *) plan_reader.getOperator(5);
    auto predicate = join->getPredicate();
    auto expr = ((GenericExpression<bool> *) predicate)->root_;

    ExpressionDeparser<bool> deparser(expr);
    string observed = deparser.toString();



    observed = Utilities::eraseValueQuotes(observed, search_strs_);

    auto parsed_expr = ExpressionParser<bool>::parseJSONExpression(observed, join->getChild(0)->getOutputSchema(), join->getChild(1)->getOutputSchema());
    auto parsed = ((GenericExpression<bool> *) parsed_expr)->root_;

    ASSERT_EQ(*expr, *parsed);
}

// expression: ($8 * (1.000000 - $9)) - ($3 * $7)
TEST_F(PlanDeparserTest, math_expression) {
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q9.json";
    PlanParser<bool> plan_reader(db_name_, plan_file);

    auto project = (Project<bool> *) plan_reader.getOperator(6);
    auto expression_map = project->getExpressions();
    ExpressionNode<bool> *expr = ((GenericExpression<bool> *)expression_map[2])->root_;


    ExpressionDeparser<bool> deparser(expr);
    string observed = deparser.toString();



    observed = Utilities::eraseValueQuotes(observed, search_strs_);

    auto parsed_expr = ExpressionParser<bool>::parseJSONExpression(observed, project->getChild()->getOutputSchema());
    auto parsed = ((GenericExpression<bool> *) parsed_expr)->root_;
    ASSERT_EQ(*expr, *parsed);
}


TEST_F(PlanDeparserTest, tpch_q01) {
    runTest(1);
}




TEST_F(PlanDeparserTest, tpch_q03) {
    runTest(3);
}


TEST_F(PlanDeparserTest, tpch_q05) {
    runTest(5);
}


TEST_F(PlanDeparserTest, tpch_q08) {
    runTest(8);
}

TEST_F(PlanDeparserTest, tpch_q09) {
    runTest(9);
}

TEST_F(PlanDeparserTest, tpch_q18) {
    runTest(18);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
