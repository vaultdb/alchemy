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

// DIAGNOSE = 1 --> all tests produce non-empty result
#define DIAGNOSE 0


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");

class SecureTpcHTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
SecureTpcHTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {
    string query = tpch_queries[test_id];

    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db_name = db_name;

    boost::replace_first(local_db_name, "unioned", party_name.c_str());

    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, query, false, 0);
    expected->setSortOrder(expected_sort);
    if(DIAGNOSE == 1) {
        ASSERT_GT(expected->getTupleCount(),  0);
    }

    PlanParser<emp::Bit> parser(local_db_name, test_name, netio_, FLAGS_party, 0);
    shared_ptr<SecureOperator> root = parser.getRoot();
    cout << "Have plan: " << *root << endl;

    shared_ptr<PlainTable> observed = root->run()->reveal();
    observed = DataUtilities::removeDummies(observed);



    ASSERT_EQ(*expected, *observed);

}
/*
// passes
TEST_F(SecureTpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, "tpch_unioned_50");
}


TEST_F(SecureTpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, "tpch_unioned_50");
}


TEST_F(SecureTpcHTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    // to get non-empty results, run with tpch_unioned_1000 - runs for ~40 mins
    string db_name = (DIAGNOSE == 1) ? "tpch_unioned_1000" : "tpch_unioned_50";
    runTest(5, "q5", expected_sort, db_name);
}


TEST_F(SecureTpcHTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, "tpch_unioned_1000");
}
*/

TEST_F(SecureTpcHTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, "tpch_unioned_250");

}


/*

TEST_F(SecureTpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    string db_name = (DIAGNOSE == 1) ? "tpch_unioned" : "tpch_unioned_50";
    string test_name = (DIAGNOSE == 1) ? "q18-truncated" : "q18";

    runTest(18, test_name, expected_sort, db_name);
}



*/

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

