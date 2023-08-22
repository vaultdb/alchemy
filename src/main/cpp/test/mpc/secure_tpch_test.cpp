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
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65478, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


class SecureTpcHTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest(const int &test_id, const string &test_name, const SortDefinition &expected_sort);
    string  generateExpectedOutputQuery(const int & test_id);

    int input_tuple_limit_ = -1;

};

void SecureTpcHTest::runTest(const int &test_id, const string &test_name, const SortDefinition &expected_sort) {

    string expected_sql = generateExpectedOutputQuery(test_id);
    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_sql, false, 0);
    expected->setSortOrder(expected_sort);

    ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + test_name + ".json";

    PlanParser<Bit> parser(db_name_, plan_file, input_tuple_limit_);
   SecureOperator *root = parser.getRoot();

    auto result = root->run();

    if(FLAGS_validation) {
        PlainTable *observed = result->reveal();
        ASSERT_EQ(*expected, *observed);
        delete observed;
        delete expected;
    }

}

string
SecureTpcHTest::generateExpectedOutputQuery(const int &test_id) {
    string query = tpch_queries[test_id];
    if (input_tuple_limit_ > 0) {
        query = (emp_mode_ == EmpMode::SH2PC) ? truncated_tpch_queries[test_id] : truncated_tpch_queries_ompc[test_id];
        boost::replace_all(query, "$LIMIT", std::to_string(input_tuple_limit_));
    }
    return query;
}

 // passes in ~17 secs on codd2
TEST_F(SecureTpcHTest, tpch_q01) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort);
}


 // passes in ~10 mins on codd2
TEST_F(SecureTpcHTest, tpch_q03) {

    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 500 : 1200;
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort);
}


 // passes on codd2 in about 2.5 mins
TEST_F(SecureTpcHTest, tpch_q05) {

    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 200 : 400;


    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort);

}

//  passes in 56 secs on codd12
TEST_F(SecureTpcHTest, tpch_q08) {
    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 400 : 800;
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort);
}

 // *passes in around ~42 secs on codd2
TEST_F(SecureTpcHTest, tpch_q09) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort);

}
// passes in ~2.5 mins
TEST_F(SecureTpcHTest, tpch_q18) {
    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 200 : 400;

    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};


    runTest(18, "q18", expected_sort);
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}

