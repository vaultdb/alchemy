#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <query_table/secure_tuple.h>
#include <data/psql_data_provider.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <operators/group_by_aggregate.h>
#include <operators/nested_loop_aggregate.h>
#include <parser/plan_parser.h>
#include <util/field_utilities.h>



using namespace emp;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54330, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(query, 1, "query number");
DEFINE_string(bitpacking, "packed", "bit packed or non-bit packed");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class SecureBitPackingTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int input_tuple_limit_ = FLAGS_cutoff;
};

void
SecureBitPackingTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());


    PlainTable *expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_1/" + "baseline/baseline-" + test_name + ".json";

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    SecureTable *observed = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    PlainTable *observed_plain = observed->reveal();
    DataUtilities::removeDummies(observed_plain);

    ASSERT_EQ(*expected, *observed_plain);
    delete observed_plain;
    delete observed;
    delete expected;

}

string
SecureBitPackingTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
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


TEST_F(SecureBitPackingTest, tpch_q1) {
    this->initializeBitPacking(FLAGS_unioned_db);
    cout << "Q1 Bit Packed " << endl;
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q1_non_bitpacked) {
    this->disableBitPacking();
    cout << "Q1 non Bit Packed " << endl;
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q3) {
    this->initializeBitPacking(FLAGS_unioned_db);
    cout << "Q3 Bit Packed " << endl;
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q3_non_bitpacked) {
    this->disableBitPacking();
    cout << "Q3 non Bit Packed " << endl;
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q5) {
    this->initializeBitPacking(FLAGS_unioned_db);
    cout << "Q5 Bit Packed " << endl;
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q5_non_bitpacked) {
    this->disableBitPacking();
    cout << "Q5 non Bit Packed " << endl;
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, FLAGS_unioned_db);
}


TEST_F(SecureBitPackingTest, tpch_q8) {
    this->initializeBitPacking(FLAGS_unioned_db);
    cout << "Q8 Bit Packed " << endl;
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q8_non_bitpacked) {
    this->disableBitPacking();
    cout << "Q8 non Bit Packed " << endl;
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, FLAGS_unioned_db);
}


// passed, runs in 15 mins
TEST_F(SecureBitPackingTest, tpch_q9) {
    this->initializeBitPacking(FLAGS_unioned_db);
    cout << "Q9 Bit Packed " << endl;
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q9_non_bitpacked) {
    this->disableBitPacking();
    cout << "Q9 non Bit Packed " << endl;
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, FLAGS_unioned_db);
}


TEST_F(SecureBitPackingTest, tpch_q18) {
    this->initializeBitPacking(FLAGS_unioned_db);
    cout << "Q18 Bit Packed " << endl;
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest(18, "q18", expected_sort, FLAGS_unioned_db);
}

TEST_F(SecureBitPackingTest, tpch_q18_non_bitpacked) {
    this->disableBitPacking();
    cout << "Q18 Bit Packed " << endl;
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest(18, "q18", expected_sort, FLAGS_unioned_db);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;

    /*
    if(FLAGS_bitpacking == "packed") {
        switch(FLAGS_query) {
            case 1:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q1";
                break;
            case 2:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q3";
                break;
            case 3:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q5";
                break;
            case 4:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q8";
                break;
            case 5:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q9";
                break;
            case 6:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q18";
                break;
            default:
                // Handle unknown type
                break;
        }
    } else {
        switch(FLAGS_query) {
            case 1:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q1_non_bitpacked";
                break;
            case 2:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q3_non_bitpacked";
                break;
            case 3:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q5_non_bitpacked";
                break;
            case 4:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q8_non_bitpacked";
                break;
            case 5:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q9_non_bitpacked";
                break;
            case 6:
                ::testing::GTEST_FLAG(filter) = "SecureBitPackingTest.tpch_q18_non_bitpacked";
                break;
            default:
                // Handle unknown type
                break;
        }
    }
    */
    return RUN_ALL_TESTS();
}
