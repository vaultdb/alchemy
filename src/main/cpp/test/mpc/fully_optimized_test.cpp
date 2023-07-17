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


using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(dbname, "tpch_unioned_150", "db name for baseline comparison test");
DEFINE_string(filter, "*", "run only the tests passing this filter");


class FullyOptimizedTest : public EmpBaseTest {
protected:

    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int input_tuple_limit_ = -1;

};


void
FullyOptimizedTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {


    cout << "Expected DB : " << FLAGS_dbname;
    this->initializeBitPacking(FLAGS_dbname);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_dbname);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = FLAGS_dbname;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    cout << " Observed DB : "<< local_db << " - Bit Packed" << endl;

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_dbname, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_5/Fully_Optimized/fully_optimized-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_5/Fully_Optimized/fully_optimized-"  + test_name + ".json";

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std::cout << root->printTree() << endl;

    SecureTable *result = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();
        DataUtilities::removeDummies(observed);

        ASSERT_EQ(*expected, *observed);
        ASSERT_TRUE(!observed->empty()); // want all tests to produce output

        delete observed;
        delete expected;
    }
}

string
FullyOptimizedTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
    string alice_db = db_name;
    string bob_db = db_name;
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
    runTest(1, "q1", expected_sort, unioned_db_);
}

TEST_F(FullyOptimizedTest, tpch_q3) {

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, unioned_db_);
}


TEST_F(FullyOptimizedTest, tpch_q5) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, unioned_db_);
}


TEST_F(FullyOptimizedTest, tpch_q8) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, unioned_db_);
}



TEST_F(FullyOptimizedTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, unioned_db_);

}


TEST_F(FullyOptimizedTest, tpch_q18) {
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
