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
DEFINE_string(bitpacking, "packed", "bit packed or non-bit packed");


class SecureAggregateComparisonTest : public EmpBaseTest {

protected:

    // depends on truncate-tpch-set.sql
    void runTest_SMA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    void runTest_NLA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int cutoff_ = 100;
    int input_tuple_limit_ = -1;

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.

void
SecureAggregateComparisonTest::runTest_SMA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    if(FLAGS_bitpacking == "packed")
        this->initializeBitPacking(unioned_db_);
    else
        this->disableBitPacking();

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    std::cout << "Setting : " ;
    if(FLAGS_bitpacking == "packed")
        std::cout << " Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;
    else
        std::cout<< " Non Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;


    PlainTable *expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/SMA/queries-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/SMA/mpc-"  + test_name + ".json";

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    SecureTable *observed = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "SMA : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    PlainTable *observed_plain = observed->reveal();
    DataUtilities::removeDummies(observed_plain);

    ASSERT_EQ(*expected, *observed_plain);

    ASSERT_TRUE(!observed_plain->empty()); // want all tests to produce output
    delete observed_plain;
    delete observed;
    delete expected;
}

void
SecureAggregateComparisonTest::runTest_NLA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    if(FLAGS_bitpacking == "packed")
        this->initializeBitPacking(unioned_db_);
    else
        this->disableBitPacking();

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    std::cout << "Setting : " ;
    if(FLAGS_bitpacking == "packed")
        std::cout << " Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;
    else
        std::cout<< " Non Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;

    PlainTable *expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/NLA/queries-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/NLA/mpc-"  + test_name + ".json";

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    SecureTable *observed = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "NLA : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    PlainTable *observed_plain = observed->reveal();
    DataUtilities::removeDummies(observed_plain);

    ASSERT_EQ(*expected, *observed_plain);

    ASSERT_TRUE(!observed_plain->empty()); // want all tests to produce output
    delete observed_plain;
    delete observed;
    delete expected;
}

string
SecureAggregateComparisonTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
    string alice_db = db_name;
    string bob_db = db_name;
    string query = aggregate_tpch_queries[test_id];
    boost::replace_all(query, "$cutoff", std::to_string(cutoff_));

    boost::replace_first(alice_db, "unioned", "alice");
    boost::replace_first(bob_db, "unioned", "bob");

    return query;
}



TEST_F(SecureAggregateComparisonTest, tpch_q1_SMA) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_SMA(1, "q1", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q1_NLA) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_NLA(1, "q1", expected_sort, unioned_db_);
}



TEST_F(SecureAggregateComparisonTest, tpch_q3_SMA) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_SMA(3, "q3", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q3_NLA) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_NLA(3, "q3", expected_sort, unioned_db_);
}


// passes on codd2 in about 2.5 mins
TEST_F(SecureAggregateComparisonTest, tpch_q5_SMA) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_SMA(5, "q5", expected_sort, unioned_db_);
}


// passes on codd2 in about 2.5 mins
TEST_F(SecureAggregateComparisonTest, tpch_q5_NLA) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_NLA(5, "q5", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q8_SMA) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_SMA(8, "q8", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q8_NLA) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_NLA(8, "q8", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q9_SMA) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_SMA(9, "q9", expected_sort, unioned_db_);

}

TEST_F(SecureAggregateComparisonTest, tpch_q9_NLA) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_NLA(9, "q9", expected_sort, unioned_db_);

}

TEST_F(SecureAggregateComparisonTest, tpch_q18_SMA) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_SMA(18, "q18", expected_sort, unioned_db_);
}

TEST_F(SecureAggregateComparisonTest, tpch_q18_NLA) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_NLA(18, "q18", expected_sort, unioned_db_);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}
