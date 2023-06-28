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
DEFINE_string(test, "baseline", "testname baseline or handcode");\
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");



class BaselineComparisonTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest_baseline(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    void runTest_handcode(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int input_tuple_limit_ = 150;

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.

void
BaselineComparisonTest::runTest_baseline(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    this->disableBitPacking();

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());


    PlainTable *expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<emp::Bit> parser(local_db, test_name, input_tuple_limit_, "experiment_1", true);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    SecureTable *observed = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "Baseline : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    PlainTable *observed_plain = observed->reveal();
    DataUtilities::removeDummies(observed_plain);

    ASSERT_EQ(*expected, *observed_plain);
    delete observed_plain;
    delete observed;
    delete expected;
}

void
BaselineComparisonTest::runTest_handcode(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    this->disableBitPacking();

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());


    PlainTable *expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    PlanParser<emp::Bit> parser(local_db, test_name, input_tuple_limit_, "experiment_1", false);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    SecureTable *observed = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "Handcode : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    PlainTable *observed_plain = observed->reveal();
    DataUtilities::removeDummies(observed_plain);

    ASSERT_EQ(*expected, *observed_plain);
    delete observed_plain;
    delete observed;
    delete expected;
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


TEST_F(BaselineComparisonTest, tpch_q1_baseline) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_baseline(1, "q1", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q1_handcode) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_handcode(1, "q1", expected_sort, unioned_db_);
}



TEST_F(BaselineComparisonTest, tpch_q3_baseline) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_baseline(3, "q3", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q3_handcode) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_handcode(3, "q3", expected_sort, unioned_db_);
}

/*
// passes on codd2 in about 2.5 mins
TEST_F(BaselineComparisonTest, tpch_q5_baseline) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(5, "q5", expected_sort, unioned_db_);
}


// passes on codd2 in about 2.5 mins
TEST_F(BaselineComparisonTest, tpch_q5_handcode) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(5, "q5", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q8_baseline) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_baseline(8, "q8", expected_sort, unioned_db_);
}

TEST_F(BaselineComparisonTest, tpch_q8_handcode) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_handcode(8, "q8", expected_sort, unioned_db_);
}


TEST_F(BaselineComparisonTest, tpch_q9_baseline) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_baseline(9, "q9", expected_sort, unioned_db_);

}

TEST_F(BaselineComparisonTest, tpch_q9_handcode) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_handcode(9, "q9", expected_sort, unioned_db_);

}

TEST_F(BaselineComparisonTest, tpch_q18_baseline) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_baseline(18, "q18", expected_sort, unioned_db_);
}

TEST_F(BaselineComparisonTest, tpch_q18_handcode) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_handcode(18, "q18", expected_sort, unioned_db_);
}
*/


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}
