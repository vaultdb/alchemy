#include "plain/plain_base_test.h"
#include <data/csv_reader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"
#include <parser/plan_deparser.h>
#include <boost/algorithm/string/replace.hpp>
#include "operators/join.h"
#include "parser/expression_deparser.h"
#include "parser/expression_parser.h"


// e.g., /plan_parser_test --filter="PlanDeparserTest.tpch_q1"
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "row", "storage model for tables (row or column)");


class PlanDeparserTest : public PlainBaseTest {


protected:

    // limit input to first N tuples per SQL statement
    int limit_ = 10;

    void runTest(const int &test_id, const string & expected_plan);

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void PlanDeparserTest::runTest(const int &test_id, const string & expected_plan) {
    string test_name = "q" + std::to_string(test_id);
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + test_name + ".json";

    PlanParser<bool> plan_reader(db_name_, plan_file, limit_);
    PlainOperator *root = plan_reader.getRoot();
    std::string observed_plan = root->printTree();

    string json_plan = PlanDeparser<bool>::deparse(root);


    cout << "JSON Plan: " << json_plan << endl;
    delete root;

}


TEST_F(PlanDeparserTest, conjunction_expression) {
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q9.json";
    PlanParser<bool> plan_reader(db_name_, plan_file);

    auto join =  (Join<bool> *) plan_reader.getOperator(5);
    auto predicate = join->getPredicate();
    auto expr = ((GenericExpression<bool> *) predicate)->root_;

    ExpressionDeparser<bool> deparser(expr);
    string observed = deparser.toString();

    vector<string> search_strs = {"\"dummy-tag\"", "\"input-party\"", "\"input-limit\"", "\"field\"", "\"input\"", "\"foreign-key\"", "\"nullable\"", " \"literal\"", "precision", "\"scale\"", "\"read_lhs\"", "\"binary_mode\""};

    observed = Utilities::eraseValueQuotes(observed, search_strs);

    auto parsed_expr = ExpressionParser<bool>::parseJSONExpression(observed, join->getChild(0)->getOutputSchema(), join->getChild(1)->getOutputSchema());
    auto parsed = ((GenericExpression<bool> *) parsed_expr)->root_;

    ASSERT_EQ(*expr, *parsed);
}
// sort not really needed in Q1 MPC pipeline, retaining it to demo the hand-off from one op to the next
TEST_F(PlanDeparserTest, tpch_q01) {
    string expected_plan = "";
    runTest(1, expected_plan);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
