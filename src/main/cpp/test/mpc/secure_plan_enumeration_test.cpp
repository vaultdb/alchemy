#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <parser/plan_parser.h>
#include <opt/plan_optimizer.h>


using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");


class SecurePlanEnumerationTest : public EmpBaseTest {
protected:

    void runTest(const int &test_id, const SortDefinition &expected_sort);
    void runMultiAggregatesTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name, const string & sql_file, const string &plan_file);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int input_tuple_limit_ = -1;

};


void
SecurePlanEnumerationTest::runTest(const int &test_id, const SortDefinition &expected_sort) {
    string test_name = "q" + std::to_string(test_id);

    this->initializeBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);


    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->order_by_ = expected_sort;

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_6/Auto_Optimized/auto_optimized-"  + test_name + ".json";

    // Gate count measurement
    auto start_gates = SystemConfiguration::getInstance().emp_manager_->andGateCount();

    // Comm Cost measurement
    auto start_comm_cost = SystemConfiguration::getInstance().emp_manager_->getCommCost();

    // Initialize memory measurement
    size_t initial_memory = Utilities::checkMemoryUtilization(true);

    // Start measuring time
    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<Bit> parser(db_name_, plan_file, input_tuple_limit_, true);
    SecureOperator *root = parser.getRoot();

    std::cout << "Original Plan : " << endl;
    std::cout << root->printTree() << endl;

    time_point<high_resolution_clock> BeforePlanEnumeration = clock_start();

    //root = parser.optimizeTree();
    PlanOptimizer<Bit> optimizer(root, parser.getOperatorMap(), parser.getSupportOps(), parser.getInterestingSortOrders());

    root = optimizer.optimizeTree();

    double PlanEnumerationDuration = time_from(BeforePlanEnumeration) / 1e6;

    SecureTable *result = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;


    cout << "Plan Enumeration Time : " << PlanEnumerationDuration << " sec\n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";
    auto end_gates = SystemConfiguration::getInstance().emp_manager_->andGateCount();
    float e2e_gates = (float) (end_gates - start_gates);
    float cost_estimate = (float) root->planCost();
    float relative_error = (fabs(e2e_gates - cost_estimate) / e2e_gates) * 100.0f;
    cout << "End-to-end estimated gates: " << cost_estimate <<  ". observed gates: " << end_gates - start_gates << " gates, relative error (%)=" << relative_error << endl;

    // Measure and print memory after execution
    size_t peak_memory = Utilities::checkMemoryUtilization(true);
    //size_t memory_usage = peak_memory - initial_memory;
    //cout << "Initial Memory: " << initial_memory << " bytes, Peak Memory After Execution: " << peak_memory << " bytes" << ", Memory Usage: " << memory_usage << " bytes" << endl;
    cout << "Memory Usage: " << peak_memory << " bytes" << endl;


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

void
SecurePlanEnumerationTest::runMultiAggregatesTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name, const string & sql_file, const string &plan_file) {


    this->initializeBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name_;

    cout << " Observed DB : "<< local_db << " - Bit Packed" << endl;
    auto start_gates = SystemConfiguration::getInstance().emp_manager_->andGateCount();

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->order_by_ = expected_sort;

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_, true);
    SecureOperator *root = parser.getRoot();

    std::cout << root->printTree() << endl;

    SecureTable *result = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;


    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";
    auto end_gates = SystemConfiguration::getInstance().emp_manager_->andGateCount();
    cout << "End-to-end plan gates: " << root->planCost() << " estimated: " << end_gates - start_gates << " gates." << endl;


    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;
    }
}


string
SecurePlanEnumerationTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
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


// No Aggregate

TEST_F(SecurePlanEnumerationTest, tpch_q1) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, expected_sort);


}


TEST_F(SecurePlanEnumerationTest, tpch_q3) {

SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(1, SortDirection::DESCENDING),
                             ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, expected_sort);
}


TEST_F(SecurePlanEnumerationTest, tpch_q5) {

SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, expected_sort);
}


TEST_F(SecurePlanEnumerationTest, tpch_q8) {

SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, expected_sort);
}



TEST_F(SecurePlanEnumerationTest, tpch_q9) {
// $0 ASC, $1 DESC
SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, expected_sort);

}


TEST_F(SecurePlanEnumerationTest, tpch_q18) {
// -1 ASC, $4 DESC, $3 ASC
SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(4, SortDirection::DESCENDING),
                             ColumnSort(3, SortDirection::ASCENDING)};
    runTest(18, expected_sort);
}

/*
TEST_F(SecurePlanEnumerationTest, two_aggregates_q18) {
    string test_name = "q18";
    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_6/Auto_Optimized/two_aggregates-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_6/Auto_Optimized/two_aggregates-"  + test_name + ".json";

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runMultiAggregatesTest(18, "q18", expected_sort, FLAGS_unioned_db, sql_file, plan_file);
}
*/


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
