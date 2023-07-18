#include "emp_base_test.h"
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include <parser/plan_parser.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <opt/operator_cost_model.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65478, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


using namespace emp;
using namespace vaultdb;


class PlanCostTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch.sql
    // limit input to first N tuples per SQL statement
    int input_tuple_limit_ = FLAGS_cutoff;

    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    void traverseTree(SecureOperator *op);

};


// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
PlanCostTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());



    string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/queries-" + test_name + ".sql";
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + test_name + ".json";

    PlanParser<Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();
    auto result = root->run();


}

void PlanCostTest::traverseTree(SecureOperator *op) {

    // set up children
    if(!op->isLeaf()) {
        traverseTree(op->getChild(0));
        auto rhs = op->getChild(1);
        if(rhs != nullptr) traverseTree(rhs);
    }

    size_t gate_cost = OperatorCostModel::operatorCost(op);
    cout << "Gate cost for " << op->toString() << " is " << gate_cost << endl;
    op->run();
    size_t observed_gates = op->getGateCount();
    double relative_error = fabs((double) gate_cost - (double) observed_gates) / (double) observed_gates;
    cout << "Observed gates: " << observed_gates << ", relative error: " << relative_error << endl;
    ASSERT_LE(relative_error, 0.1);

}

TEST_F(PlanCostTest, tpch_q01) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, unioned_db_);
}


TEST_F(PlanCostTest, tpch_q03) {

    input_tuple_limit_ = 500;
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, unioned_db_);
}

TEST_F(PlanCostTest, tpch_q05) {
    input_tuple_limit_ = 200;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, unioned_db_);

}

TEST_F(PlanCostTest, tpch_q08) {
    input_tuple_limit_ = 400;
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, unioned_db_);
}

// *passes in around ~42 secs on codd2
TEST_F(PlanCostTest, tpch_q09) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, unioned_db_);

}
// passes in ~2.5 mins
TEST_F(PlanCostTest, tpch_q18) {
    input_tuple_limit_ = 200;

    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};


    runTest(18, "q18", expected_sort, unioned_db_);
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}


