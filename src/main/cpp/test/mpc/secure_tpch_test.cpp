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
#include <parser/plan_parser.h>


using namespace emp;
using namespace vaultdb;

// DIAGNOSE = 1 --> all tests produce non-empty result
// JMR: This is db dependendant
// TODO: either add DBs to repo to make this reproducible or find a better way to test

#define DIAGNOSE 1
#define TRUNCATE 0


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54330, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");

class SecureTpcHTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    int input_tuple_limit_ = 1000;

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
SecureTpcHTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {
    string query = tpch_queries[test_id];
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db_name = db_name;
    boost::replace_first(local_db_name, "unioned", party_name.c_str());

    if(TRUNCATE) {
        query = truncated_tpch_queries[test_id];
        boost::replace_all(query, "$LIMIT", std::to_string(input_tuple_limit_));
    }

    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, query, false, 0);
    expected->setSortOrder(expected_sort);

//    // JMR, this Q5 + Q18 exception is a temporary hack.
//    TODO: fix this with better input data and adding the input DBs to the repo
//    if(DIAGNOSE == 1) {
//        ASSERT_GT(expected->getTupleCount(),  0);
//    }


    int limit = (TRUNCATE) ? input_tuple_limit_ : -1;
    PlanParser<emp::Bit> parser(local_db_name, test_name, netio_, FLAGS_party, limit);
    shared_ptr<SecureOperator> root = parser.getRoot();

    shared_ptr<PlainTable> observed = root->run()->reveal();
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

}


// Passes, 1.5 mins @ DIAGNOSE
TEST_F(SecureTpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, "tpch_unioned_50");
}

// Passes, 4+ mins @ DIAGNOSE
TEST_F(SecureTpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, "tpch_unioned_50");
}




// runs for 2 hours in MPC on tpch_unioned_250
 // now faster since we are hand-picking the keys to admit
TEST_F(SecureTpcHTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    // to get non-empty results, run with tpch_unioned_1000 - runs for ~40 mins in plaintext
    // JMR: ran for 12+ hours in MPC before killing it.
    // reduced the runtime by adding a where clause at the bottom of each SQL query
    //string db_name = (DIAGNOSE == 1) ? "tpch_unioned_250" : "tpch_unioned_50";
    string db_name = "tpch_unioned_50";
    runTest(5, "q5", expected_sort, db_name);

}


// runs for ~9 minutes
TEST_F(SecureTpcHTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, "tpch_unioned_1000");
}


// passed, runs in 15 mins
TEST_F(SecureTpcHTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, "tpch_unioned_100");

}

// passed, runs < 1 minute
TEST_F(SecureTpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    string db_name = (DIAGNOSE == 1) ? "tpch_unioned" : "tpch_unioned_50";
    string test_name = (DIAGNOSE == 1) ? "q18-truncated" : "q18";

    runTest(18, test_name, expected_sort, db_name);
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

