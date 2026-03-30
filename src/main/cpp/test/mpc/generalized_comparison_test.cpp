#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <opt/operator_cost_model.h>
#include <parser/plan_parser.h>
#include <opt/plan_optimizer.h>

using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_1500", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_1500", "alice db name");
DEFINE_string(bob_db, "tpch_bob_1500", "bob db name");
DEFINE_int32(cutoff, -1, "limit clause for queries");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, false, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");


class GeneralizedComparisonTest : public EmpBaseTest {
protected:
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name, int type_num);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);
    void runStubTest(string & sql_plan, string & json_plan, string & expected_query, SortDefinition & expected_sort, const string & unioned_db);
    int input_tuple_limit_ = -1;

};


void
GeneralizedComparisonTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name, int type_num) {

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);
    string local_db = db_name_;

    // Gate count measurement
    auto start_gates = manager_->andGateCount();

    // Comm Cost measurement
    auto start_comm_cost = manager_->getCommCost();

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
    expected->order_by_ = expected_sort;

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    std::string plan_file = "";
    switch (type_num) {
        case 1:
            plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/baseline_withPKFK/baseline-" + test_name + ".json";
            break;
        case 2:
            plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_4/cost_optimized-" + test_name + ".json";
            break;
    }

    // Initialize memory measurement
    size_t initial_memory = Utilities::checkMemoryUtilization(true);

    // Start measuring time
    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<Bit> parser(db_name_, plan_file, input_tuple_limit_, /* read_from_file */ true);

    SecureOperator *root = parser.getRoot();

    cout << "Original Tree : " << endl;
    cout << root->printTree() << endl;
    auto cost_est = OperatorCostModel::planCost((SecureOperator *) root);
    float secs_per_gate = 0.00065;
    string test_desc = (type_num == 1) ? "baseline" : "optimized";
    cout << "Time estimate for " << test_desc << "-" << test_name << " : " << (cost_est * secs_per_gate) << " secs" << endl;
    //return;

    if(type_num == 2) {
        time_point <high_resolution_clock> BeforeCostOptimization = clock_start();

        root->detectCTE();

        PlanOptimizer<Bit> optimizer(root, parser.getOperatorMap(), parser.getSupportOps(), parser.getInterestingSortOrders());
        root = optimizer.optimizeTree();

        if (root->hasCte()) {
            root = root->canonicalized();
        }

        std::cout << "Cost Optimized Plan : " << endl;
        cout << root->printTree() << endl;

        double CostOptimizationDuration = time_from(BeforeCostOptimization) / 1e6;
        cout << "Cost Optimization Time : " << CostOptimizationDuration << " sec\n";
    }


    SecureTable *result = root->run();

    // Measure CPU Time
    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);

    // Measure Runtime
    double duration = time_from(startTime) / 1e6;

    cout << "Runtime: " << duration << " sec, CPU Time: " << secureClockTicksPerSecond << " sec, CPU clock ticks: " << secureClockTicks << ", CPU clock ticks per second: " << CLOCKS_PER_SEC << "\n";

    auto end_gates = manager_->andGateCount();
    float e2e_gates = (float) (end_gates - start_gates);
    float cost_estimate = (float) root->planCost();
    float relative_error = (fabs(e2e_gates - cost_estimate) / e2e_gates) * 100.0f;
    cout << "End-to-end estimated gates: " << cost_estimate <<  ". observed gates: " << end_gates - start_gates << " gates, relative error (%)=" << relative_error << endl;

    // Measure and print memory after execution
    size_t peak_memory = Utilities::checkMemoryUtilization(true);
    size_t memory_usage = peak_memory - initial_memory;
    cout << "Initial Memory: " << initial_memory << " bytes, Peak Memory After Execution: " << peak_memory << " bytes" << ", Memory Usage: " << memory_usage << " bytes" << endl;

    // Comm Cost measurement
    auto end_comm_cost = manager_->getCommCost();
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
GeneralizedComparisonTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
    string alice_db = FLAGS_unioned_db;
    string bob_db = FLAGS_unioned_db;
    boost::replace_first(alice_db, "unioned", "alice");
    boost::replace_first(bob_db, "unioned", "bob");

    // Getting query from tpch_queries.h
    // string query = tpch_queries[test_id];

    //Modified to read ./conf/sql/tpch/qxx.sql
    std::ostringstream test_id_stream;
    test_id_stream << std::setw(2) << std::setfill('0') << test_id;
    std::string test_id_str = test_id_stream.str();

    std::string query_file_path = Utilities::getCurrentWorkingDirectory() + "/conf/sql/tpch/q" + test_id_str + ".sql";
    std::ifstream query_file(query_file_path);

    if (!query_file.is_open()) {
        throw std::runtime_error("Could not open query file: " + query_file_path);
    }

    std::stringstream query_buffer;
    query_buffer << query_file.rdbuf();
    std::string query = query_buffer.str();

    query_file.close();

    if(input_tuple_limit_ > 0) {
        query = truncated_tpch_queries[test_id];
        boost::replace_all(query, "$LIMIT", std::to_string(input_tuple_limit_));
        boost::replace_all(query, "$ALICE_DB", alice_db);
        boost::replace_all(query, "$BOB_DB", bob_db);
    }

    return query;
}

///////////////////////////////////////////////////////////////////////////////
// *** Card-Bound Tests ***
// ---------------------------------------------------------
// NOTE: From here, we are using keyed-relationship & card bound, so need to run every queries
// ---------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


TEST_F(GeneralizedComparisonTest, baseline_tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q2) {

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(0, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::ASCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};
    runTest(2, "q2", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q3) {
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                ColumnSort(1, SortDirection::DESCENDING),
                                ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q4) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(4, "q4", expected_sort, FLAGS_unioned_db, 1);
}


TEST_F(GeneralizedComparisonTest, baseline_tpch_q5) {
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q6) {
    SortDefinition expected_sort{};
    runTest(6, "q6", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q7) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(3);
    runTest(7, "q7", expected_sort, FLAGS_unioned_db, 1);
}


TEST_F(GeneralizedComparisonTest, baseline_tpch_q8) {
SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
runTest(8, "q8", expected_sort, FLAGS_unioned_db, 1);
}


TEST_F(GeneralizedComparisonTest, baseline_tpch_q9) {
SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
runTest(9, "q9", expected_sort, FLAGS_unioned_db, 1);

}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q10) {
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(2, SortDirection::DESCENDING)};
    runTest(10, "q10", expected_sort, FLAGS_unioned_db, 1);
}


TEST_F(GeneralizedComparisonTest, baseline_tpch_q11) {
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(11, "q11", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q12) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(12, "q12", expected_sort, FLAGS_unioned_db, 1);
}


TEST_F(GeneralizedComparisonTest, baseline_tpch_q13) {
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING), ColumnSort(0, SortDirection::DESCENDING)};
    runTest(13, "q13", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q14) {
    SortDefinition expected_sort{};
    runTest(14, "q14", expected_sort, FLAGS_unioned_db, 1);
}


TEST_F(GeneralizedComparisonTest, baseline_tpch_q15) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(15, "q15", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q16) {
    SortDefinition expected_sort{ColumnSort(3, SortDirection::DESCENDING),
                                   ColumnSort(0, SortDirection::ASCENDING),
                                    ColumnSort(1, SortDirection::ASCENDING),
                                    ColumnSort(2, SortDirection::ASCENDING)};
    runTest(16, "q16", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q17) {
    SortDefinition expected_sort{};
    runTest(17, "q17", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q18) {
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};
    runTest(18, "q18", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q19) {
    SortDefinition expected_sort{};
    runTest(19, "q19", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q20) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(20, "q20", expected_sort, FLAGS_unioned_db, 1);

}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q21) {
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(0, SortDirection::ASCENDING)};
    runTest(21, "q21", expected_sort, FLAGS_unioned_db, 1);
}

TEST_F(GeneralizedComparisonTest, baseline_tpch_q22) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(22, "q22", expected_sort, FLAGS_unioned_db, 1);
}



//// Cost-Optimized Tests
TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, FLAGS_unioned_db, 2);
}


TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q2) {
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(0, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::ASCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};
    runTest(2, "q2", expected_sort, FLAGS_unioned_db, 2);
}



TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q3) {
SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(1, SortDirection::DESCENDING),
                             ColumnSort(2, SortDirection::ASCENDING)};
runTest(3, "q3", expected_sort, FLAGS_unioned_db, 2);
}


TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q4) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(4, "q4", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q6) {
    SortDefinition expected_sort{};
    runTest(6, "q6", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q7) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(3);
    runTest(7, "q7", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, FLAGS_unioned_db, 2);
}


TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q9) {
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q10) {
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(2, SortDirection::DESCENDING)};
    runTest(10, "q10", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q11) {
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(11, "q11", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q12) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(12, "q12", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q13) {
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING), ColumnSort(0, SortDirection::DESCENDING)};
    runTest(13, "q13", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q14) {
    SortDefinition expected_sort{};
    runTest(14, "q14", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q15) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(15, "q15", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q16) {
    SortDefinition expected_sort{ColumnSort(3, SortDirection::DESCENDING),
                                    ColumnSort(0, SortDirection::ASCENDING),
                                    ColumnSort(1, SortDirection::ASCENDING),
                                    ColumnSort(2, SortDirection::ASCENDING)};
    runTest(16, "q16", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q17) {
    SortDefinition expected_sort{};
    runTest(17, "q17", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q18) {
// -1 ASC, $4 DESC, $3 ASC
SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                             ColumnSort(4, SortDirection::DESCENDING),
                             ColumnSort(3, SortDirection::ASCENDING)};
runTest(18, "q18", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q19) {
    SortDefinition expected_sort{};
    runTest(19, "q19", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q20) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(20, "q20", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q21) {
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(0, SortDirection::ASCENDING)};
    runTest(21, "q21", expected_sort, FLAGS_unioned_db, 2);
}

TEST_F(GeneralizedComparisonTest, cost_optimized_tpch_q22) {
    SortDefinition expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(22, "q22", expected_sort, FLAGS_unioned_db, 2);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
