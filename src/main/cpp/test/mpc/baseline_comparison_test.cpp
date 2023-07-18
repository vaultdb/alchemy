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
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(dbname, "tpch_unioned_150", "db name for baseline comparison test");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(bitpacking, "packed", "bit packed or non-bit packed");



class BaselineComparisonTest : public EmpBaseTest {


protected:

    void controlBitPacking(const string &db_name);
    void runTest_baseline(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    void runTest_handcode(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int input_tuple_limit_ = FLAGS_cutoff;

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.

void
BaselineComparisonTest::controlBitPacking(const string &db_name) {
    if(FLAGS_bitpacking == "packed") {
        this->initializeBitPacking(db_name);
        std::cout << "Bit Packed - Expected DB : " << db_name << "\n";
    }
    else {
        this->disableBitPacking();
        std::cout << "Non Bit Packed - Expected DB : " << db_name << "\n";
    }
}

void
BaselineComparisonTest::runTest_baseline(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    controlBitPacking(FLAGS_dbname);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_dbname);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = FLAGS_dbname;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    cout << " Observed DB : "<< local_db << endl;

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_dbname, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".json";

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    SecureTable *result = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "Baseline : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";
    

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();
        DataUtilities::removeDummies(observed);

        ASSERT_EQ(*expected, *observed);
        //ASSERT_TRUE(!observed->empty()); // want all tests to produce output

        delete observed;
        delete expected;
    }
}

void
BaselineComparisonTest::runTest_handcode(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    controlBitPacking(FLAGS_dbname);

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_dbname);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = FLAGS_dbname;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    cout << " Observed DB : "<< local_db << endl;

    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_dbname, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/MPC_minimization/queries-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/MPC_minimization/mpc-"  + test_name + ".json";

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    SecureTable *result = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "Handcode : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";


    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();
        DataUtilities::removeDummies(observed);

        ASSERT_EQ(*expected, *observed);
        //ASSERT_TRUE(!observed->empty()); // want all tests to produce output

        delete observed;
        delete expected;
    }
}

string
BaselineComparisonTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
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

TEST_F(BaselineComparisonTest, tpch_q1_handcode) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_handcode(1, "q1", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q1_baseline) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_baseline(1, "q1", expected_sort, unioned_db_);
}

TEST_F(BaselineComparisonTest, tpch_q3_handcode) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_handcode(3, "q3", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q3_baseline) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_baseline(3, "q3", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q5_handcode) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(5, "q5", expected_sort, unioned_db_);
}

TEST_F(BaselineComparisonTest, tpch_q5_baseline) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(5, "q5", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q8_handcode) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_handcode(8, "q8", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q8_baseline) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_baseline(8, "q8", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q9_handcode) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(9, "q9", expected_sort, unioned_db_);

}

TEST_F(BaselineComparisonTest, tpch_q9_baseline) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(9, "q9", expected_sort, unioned_db_);
}

TEST_F(BaselineComparisonTest, tpch_q18_handcode) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_handcode(18, "q18", expected_sort, unioned_db_);
}

TEST_F(BaselineComparisonTest, tpch_q18_baseline) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_baseline(18, "q18", expected_sort, unioned_db_);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
