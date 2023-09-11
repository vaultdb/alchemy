#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <parser/plan_parser.h>


using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


class FullyOptimizedTest : public EmpBaseTest {
protected:

    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);
    void runStubTest(string & sql_plan, string & json_plan, string & expected_query, SortDefinition & expected_sort, const string & unioned_db);
    int input_tuple_limit_ = -1;

};


void
FullyOptimizedTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {


    this->initializeBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);
    string local_db = db_name_;

//    cout << " Observed DB : "<< local_db << " - Bit Packed: " << SystemConfiguration::getInstance().bitPackingEnabled() <<  endl;

    // Gate count measurement
    auto start_gates = SystemConfiguration::getInstance().emp_manager_->andGateCount();

    // Comm Cost measurement
    auto start_comm_cost = SystemConfiguration::getInstance().emp_manager_->getCommCost();

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_5/Fully_Optimized/fully_optimized-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_5/Fully_Optimized/fully_optimized-"  + test_name + ".json";

    // Initialize memory measurement
    size_t initial_memory = Utilities::checkMemoryUtilization(true);

    // Start measuring time
    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

//    std::cout << "Query tree: " << root->printTree() << endl;

    SecureTable *result = root->run();

    // Measure CPU Time
    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);

    // Measure Runtime
    double duration = time_from(startTime) / 1e6;

    cout << "Runtime: " << duration << " sec, CPU Time: " << secureClockTicksPerSecond << " sec, CPU clock ticks: " << secureClockTicks << ", CPU clock ticks per second: " << CLOCKS_PER_SEC << "\n";

    auto end_gates = SystemConfiguration::getInstance().emp_manager_->andGateCount();
    float e2e_gates = (float) (end_gates - start_gates);
    float cost_estimate = (float) root->planCost();
    float relative_error = (fabs(e2e_gates - cost_estimate) / e2e_gates) * 100.0f;
    cout << "End-to-end estimated gates: " << cost_estimate <<  ". observed gates: " << end_gates - start_gates << " gates, relative error (%)=" << relative_error << endl;

    // Measure and print memory after execution
    size_t peak_memory = Utilities::checkMemoryUtilization(true);
    size_t memory_usage = peak_memory - initial_memory;
    cout << "Initial Memory: " << initial_memory << " bytes, Peak Memory After Execution: " << peak_memory << " bytes" << ", Memory Usage: " << memory_usage << " bytes" << endl;

    // Comm Cost measurement
    auto end_comm_cost = SystemConfiguration::getInstance().emp_manager_->getCommCost();
    double bandwidth = (end_comm_cost - start_comm_cost) / duration;  // Assuming 'duration' is the time taken for this communication in seconds
    cout << "Bandwidth: " << bandwidth << " Bps" << endl;

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;
    }
}


string
FullyOptimizedTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
    string alice_db = FLAGS_unioned_db;
    string bob_db = FLAGS_unioned_db;
    boost::replace_first(alice_db, "unioned", "alice");
    boost::replace_first(bob_db, "unioned", "bob");

    string query = tpch_queries[test_id];

    if(input_tuple_limit_ > 0) {
        query = truncated_tpch_queries[test_id];
        boost::replace_all(query, "$LIMIT", std::to_string(input_tuple_limit_));
        boost::replace_all(query, "$ALICE_DB", alice_db);
        boost::replace_all(query, "$BOB_DB", bob_db);
    }

    return query;
}



TEST_F(FullyOptimizedTest, tpch_q1) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, FLAGS_unioned_db);

}


TEST_F(FullyOptimizedTest, tpch_q3) {

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, FLAGS_unioned_db);
}


TEST_F(FullyOptimizedTest, tpch_q5) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, FLAGS_unioned_db);
}


TEST_F(FullyOptimizedTest, tpch_q8) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, FLAGS_unioned_db);
}



TEST_F(FullyOptimizedTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, FLAGS_unioned_db);

}




TEST_F(FullyOptimizedTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};
    runTest(18, "q18", expected_sort, FLAGS_unioned_db);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
