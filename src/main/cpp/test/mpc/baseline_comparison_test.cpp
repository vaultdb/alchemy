#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <query_table/secure_tuple.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <parser/plan_parser.h>
#include <util/logger.h>

using namespace emp;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_600", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_600", "alice db name");
DEFINE_string(bob_db, "tpch_bob_600", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, false, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(bitpacking, "packed", "bit packed or non-bit packed");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");

using namespace Logging;

class BaselineComparisonTest : public EmpBaseTest {


protected:

    void controlBitPacking(const string &db_name);
    void runTest_baseline(const int &test_id, const SortDefinition &expected_sort);
    void runTest_handcode(const int &test_id, const SortDefinition &expected_sort);
    string  generateExpectedOutputQuery(const int & test_id);
    void runTest_all_mpc(const int &test_id,
                         const SortDefinition &expected_sort);

    int input_tuple_limit_ = -1;

};

void
BaselineComparisonTest::controlBitPacking(const string &db_name) {
	Logger* log = get_log();
    if(FLAGS_bitpacking == "packed") {
        this->initializeBitPacking(db_name);
        log->write("Bit Packed - Expected DB : " + db_name, Level::DEBUG);
    }
    else {
        this->disableBitPacking();
        std::cout << "Non Bit Packed - Expected DB : " << db_name << "\n";
    }
}

void BaselineComparisonTest::runTest_all_mpc(const int &test_id,
                                              const SortDefinition &expected_sort) {
    controlBitPacking(FLAGS_unioned_db);
    string test_name = "q" + std::to_string(test_id);
    string expected_query = generateExpectedOutputQuery(test_id);

    //cout << " Observed DB : "<< local_db << endl;

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->order_by_ = expected_sort;

//    ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/zk-" + test_name + ".json";

    PlanParser<emp::Bit> parser(db_name_, plan_file, input_tuple_limit_, true);
    SecureOperator *root = parser.getRoot();
    //cout << "Parsed baseline plan for " << test_name <<  ":\n " << root->printTree() << endl;
    SecureTable *result = root->run();

    Logger* log = get_log();
    log->write(root->printTree(), Level::DEBUG);
    log->write("All MPC Baseline: ", Level::DEBUG);


    log->write("Predicted gate count: " + std::to_string(root->planCost()), Level::INFO);
    log->write("Observed gate count: " + std::to_string(root->planGateCount()), Level::INFO);
    log->write("Runtime: " + std::to_string(root->planRuntime()), Level::INFO);

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();

        ASSERT_EQ(*expected, *observed);
        ASSERT_TRUE(!observed->empty()); // want all tests to produce output

        delete observed;
        delete expected;
    }
}


void BaselineComparisonTest::runTest_baseline(const int &test_id,
                                         const SortDefinition &expected_sort) {
    controlBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id);
    string test_name = "q" + std::to_string(test_id);

    //cout << " Observed DB : "<< local_db << endl;

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->order_by_ = expected_sort;

//    ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".json";

    PlanParser<emp::Bit> parser(db_name_, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();
//    cout << "Parsed baseline plan for " << test_name <<  ":\n " << root->printTree() << endl;
    SecureTable *result = root->run();

	Logger* log = get_log();	
    log->write(root->printTree(), Level::DEBUG);
	log->write("Baseline : ", Level::DEBUG);
	

    log->write("Predicted gate count: " + std::to_string(root->planCost()), Level::INFO);	
	log->write("Observed gate count: " + std::to_string(root->planGateCount()), Level::INFO);
	log->write("Runtime: " + std::to_string(root->planRuntime()), Level::INFO);

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();

        ASSERT_EQ(*expected, *observed);
        ASSERT_TRUE(!observed->empty()); // want all tests to produce output

        delete observed;
        delete expected;
    }
}

void
BaselineComparisonTest::runTest_handcode(const int &test_id, const SortDefinition &expected_sort) {
    string test_name = "q" + std::to_string(test_id);
    controlBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id);
    cout << " Observed DB : "<< db_name_ << endl;

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->order_by_ = expected_sort;

    // ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/MPC_minimization/queries-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/MPC_minimization/mpc-"  + test_name + ".json";

    PlanParser <emp::Bit> parser(db_name_, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    //cout << "Parsed hand-opt plan for " << test_name <<  ":\n " << root->printTree() << endl;

    SecureTable *result = root->run();


	Logger* log = get_log();
	log->write("Handcode : ", Level::DEBUG);
    log->write(root->printTree(), Level::DEBUG);	

    log->write("Predicted gate count: " + std::to_string(root->planCost()), Level::INFO);	
	log->write("Observed gate count: " + std::to_string(root->planGateCount()), Level::INFO);
	log->write("Runtime: " + std::to_string(root->planRuntime()), Level::INFO);

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();

        ASSERT_EQ(*expected, *observed);
	    ASSERT_TRUE(!observed->empty()); // want all tests to produce output

        delete observed;
        delete expected;
    }
}

string
BaselineComparisonTest::generateExpectedOutputQuery(const int &test_id) {

    string query = tpch_queries[test_id];

    if(input_tuple_limit_ > 0) {
        query = truncated_tpch_queries[test_id];
        boost::replace_all(query, "$LIMIT", std::to_string(input_tuple_limit_));
    }

    return query;
}


TEST_F(BaselineComparisonTest, tpch_q1_all_mpc) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_all_mpc(1,  expected_sort);
}


TEST_F(BaselineComparisonTest, tpch_q3_all_mpc) {
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_all_mpc(3,  expected_sort);
}


TEST_F(BaselineComparisonTest, tpch_q5_all_mpc) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_all_mpc(5,  expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q8_all_mpc) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_all_mpc(8,  expected_sort);
}



TEST_F(BaselineComparisonTest, tpch_q9_all_mpc) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_all_mpc(9,  expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q18_all_mpc) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_all_mpc(18,  expected_sort);
}


TEST_F(BaselineComparisonTest, tpch_q1_baseline) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_baseline(1,  expected_sort);
}


TEST_F(BaselineComparisonTest, tpch_q3_baseline) {
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_baseline(3,  expected_sort);
}


TEST_F(BaselineComparisonTest, tpch_q5_baseline) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(5,  expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q8_baseline) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_baseline(8,  expected_sort);
}



TEST_F(BaselineComparisonTest, tpch_q9_baseline) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(9,  expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q18_baseline) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_baseline(18,  expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q1_handcode) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_handcode(1, expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q3_handcode) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_handcode(3, expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q5_handcode) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(5, expected_sort);
}

TEST_F(BaselineComparisonTest, tpch_q8_handcode) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_handcode(8, expected_sort);
}


TEST_F(BaselineComparisonTest, tpch_q9_handcode) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(9, expected_sort);

}


TEST_F(BaselineComparisonTest, tpch_q18_handcode) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_handcode(18, expected_sort);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;
}
