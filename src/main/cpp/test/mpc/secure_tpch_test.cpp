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



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54330, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");

class SecureTpcHTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int input_tuple_limit_ = 1000;

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
SecureTpcHTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {
    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);

    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    std::cout << "Querying " << db_name << " with query: \n" << expected_query << '\n';
    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    // tpch_alice_150 row counts:
    // SELECT l_returnflag, l_linestatus, COUNT(*) FROM (SELECT * FROM lineitem ORDER BY  l_orderkey, l_linenumber LIMIT 1000 ) l  WHERE l_shipdate <= date '1998-08-03' GROUP BY  l_returnflag, l_linestatus;
    //  l_returnflag | l_linestatus | count
    //--------------+--------------+-------
    // R            | F            |   222
    // A            | F            |   244
    // N            | F            |    10
    // N            | O            |   498

    // bob:

    std::cout << "Expected query answer: " << *expected << '\n';


    PlanParser<emp::Bit> parser(local_db, test_name, netio_, FLAGS_party, input_tuple_limit_);
    shared_ptr<SecureOperator> root = parser.getRoot();

    std::cout << "Query tree: \n" << root->printTree() << '\n';

    shared_ptr<PlainTable> observed = root->run()->reveal();
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

}

string
SecureTpcHTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
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

TEST_F(SecureTpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, unioned_db_);
}

TEST_F(SecureTpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest(3, "q3", expected_sort, unioned_db_);
}




//TEST_F(SecureTpcHTest, tpch_q5) {
//    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
//    // to get non-empty results, run with tpch_unioned_1000 - runs for ~40 mins in plaintext
//    // JMR: ran for 12+ hours in MPC before killing it.
//    // reduced the runtime by adding a where clause at the bottom of each SQL query
//    runTest(5, "q5", expected_sort, unioned_db_);
//
//}
//
//
//// runs for ~9 minutes
//TEST_F(SecureTpcHTest, tpch_q8) {
//    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
//    runTest(8, "q8", expected_sort, unioned_db_);
//}
//
//
//// passed, runs in 15 mins
//TEST_F(SecureTpcHTest, tpch_q9) {
//    // $0 ASC, $1 DESC
//    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
//    runTest(9, "q9", expected_sort, unioned_db_);
//
//}
//
//// passed, runs < 1 minute
//TEST_F(SecureTpcHTest, tpch_q18) {
//    // -1 ASC, $4 DESC, $3 ASC
//    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
//                                 ColumnSort(4, SortDirection::DESCENDING),
//                                 ColumnSort(3, SortDirection::ASCENDING)};
//
//
//    runTest(18, "q18", expected_sort, unioned_db_);
//}
//
//


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

