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
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_bool(plan_enumeration, false, "run plan enumeration phase if true (default false)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


class AlchemyTest : public EmpBaseTest {
protected:

    void runTest(const int &test_id, const SortDefinition &expected_sort);
    void runTest_baseline(const int &test_id, const SortDefinition &expected_sort);
    void runTest_handcode(const int &test_id, const SortDefinition &expected_sort);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int input_tuple_limit_ = -1;

};


void
AlchemyTest::runTest(const int &test_id, const SortDefinition &expected_sort) {
    string test_name = "q" + std::to_string(test_id);

    this->initializeBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/Auto_Optimized/auto_optimized-"  + test_name + ".json";

    // Gate count measurement
    auto start_gates = SystemConfiguration::getInstance().emp_manager_->andGateCount();

    // Comm Cost measurement
    auto start_comm_cost = SystemConfiguration::getInstance().emp_manager_->getCommCost();

    // Initialize memory measurement
    size_t initial_memory = Utilities::checkMemoryUtilization(true);

    // Start measuring time
    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<Bit> parser(db_name_, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    time_point<high_resolution_clock> BeforePlanEnumeration = clock_start();

    root = parser.optimizeTree();

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

void AlchemyTest::runTest_baseline(const int &test_id,
                                              const SortDefinition &expected_sort) {
    this->initializeBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);
    string test_name = "q" + std::to_string(test_id);

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".json";

    PlanParser<emp::Bit> parser(db_name_, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();
    SecureTable *result = root->run();

    cout << "Baseline : " << endl;
    cout << root->printTree() << endl;

    cout << "Predicted gate count: " << std::to_string(root->planCost()) << endl;
    cout << "Observed gate count: " << std::to_string(root->planGateCount()) << endl;
    cout << "Runtime: " << std::to_string(root->planRuntime()) << endl;

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;
    }
}

void
AlchemyTest::runTest_handcode(const int &test_id, const SortDefinition &expected_sort) {
    string test_name = "q" + std::to_string(test_id);
    this->initializeBitPacking(FLAGS_unioned_db);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);
    cout << " Observed DB : "<< db_name_ << endl;

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/MPC_minimization/queries-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/MPC_minimization/mpc-"  + test_name + ".json";

    PlanParser <emp::Bit> parser(db_name_, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    SecureTable *result = root->run();
    cout << "Baseline : " << endl;
    cout << root->printTree() << endl;

    cout << "Predicted gate count: " << std::to_string(root->planCost()) << endl;
    cout << "Observed gate count: " << std::to_string(root->planGateCount()) << endl;
    cout << "Runtime: " << std::to_string(root->planRuntime()) << endl;

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;
    }
}


string
AlchemyTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
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

TEST_F(AlchemyTest, tpch_q1_baseline) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_baseline(1,  expected_sort);
}


TEST_F(AlchemyTest, tpch_q3_baseline) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(1, SortDirection::DESCENDING),
                             ColumnSort(2, SortDirection::ASCENDING)};
    runTest_baseline(3,  expected_sort);
}


TEST_F(AlchemyTest, tpch_q5_baseline) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(5,  expected_sort);
}

TEST_F(AlchemyTest, tpch_q8_baseline) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_baseline(8,  expected_sort);
}



TEST_F(AlchemyTest, tpch_q9_baseline) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(9,  expected_sort);
}

TEST_F(AlchemyTest, tpch_q18_baseline) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(4, SortDirection::DESCENDING),
                             ColumnSort(3, SortDirection::ASCENDING)};

    runTest_baseline(18,  expected_sort);
}

TEST_F(AlchemyTest, tpch_q1_handcode) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_handcode(1, expected_sort);
}

TEST_F(AlchemyTest, tpch_q3_handcode) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(1, SortDirection::DESCENDING),
                             ColumnSort(2, SortDirection::ASCENDING)};
    runTest_handcode(3, expected_sort);
}

TEST_F(AlchemyTest, tpch_q5_handcode) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(5, expected_sort);
}

TEST_F(AlchemyTest, tpch_q8_handcode) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_handcode(8, expected_sort);
}


TEST_F(AlchemyTest, tpch_q9_handcode) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(9, expected_sort);
}


TEST_F(AlchemyTest, tpch_q18_handcode) {
    if (FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being true";
    }

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(4, SortDirection::DESCENDING),
                             ColumnSort(3, SortDirection::ASCENDING)};
    runTest_handcode(18, expected_sort);
}


TEST_F(AlchemyTest, tpch_q1) {
    if (!FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being false";
    }

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, expected_sort);
}

TEST_F(AlchemyTest, tpch_q3) {
    if (!FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being false";
    }

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(1, SortDirection::DESCENDING),
                             ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, expected_sort);
}


TEST_F(AlchemyTest, tpch_q5) {
    if (!FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being false";
    }

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, expected_sort);
}


TEST_F(AlchemyTest, tpch_q8) {
    if (!FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being false";
    }

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, expected_sort);
}



TEST_F(AlchemyTest, tpch_q9) {
    if (!FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being false";
    }

    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, expected_sort);
}


TEST_F(AlchemyTest, tpch_q18) {
    if (!FLAGS_plan_enumeration) {
        GTEST_SKIP() << "Skipping due to FLAGS_plan_enumeration being false";
    }

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(4, SortDirection::DESCENDING),
                             ColumnSort(3, SortDirection::ASCENDING)};
    runTest(18, expected_sort);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
