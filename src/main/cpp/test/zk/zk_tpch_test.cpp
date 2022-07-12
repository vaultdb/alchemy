#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <query_table/secure_tuple.h>
#include <data/psql_data_provider.h>
#include <test/support/zk_tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <operators/group_by_aggregate.h>
#include <parser/plan_parser.h>
#include <test/zk/zk_base_test.h>

using namespace emp;
using namespace vaultdb;

#define TRUNCATE_INPUTS 1

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54327, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");



class ZkTpcHTest : public ZkTest {


protected:

    // depends on truncate-tpch-set.sql
    int tuple_limit_ = 10000; // when TRUNCATE_DATA == 1, tune this to change the size of our test input data
    void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
ZkTpcHTest::runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    string query = zk_tpch_queries[test_id];
    if(TRUNCATE_INPUTS) {
        query = truncated_zk_tpch_queries[test_id];
        boost::replace_all(query, "$LIMIT", std::to_string(tuple_limit_));
    }

    // use alice DB since she's the prover
    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, query, false, 0);
    expected->setSortOrder(expected_sort);

    int limit = (TRUNCATE_INPUTS) ? tuple_limit_ : -1; // set to -1 for full test

    PlanParser<emp::Bit> parser(db_name, test_name, ios_, threads_, FLAGS_party, limit);

    shared_ptr<SecureOperator> root = parser.getRoot();

    shared_ptr<PlainTable> observed = root->run()->reveal();
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

}


TEST_F(ZkTpcHTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, "q1", expected_sort, db_name);
}


TEST_F(ZkTpcHTest, tpch_q3) {

    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};



    runTest(3, "q3", expected_sort, db_name);
}




TEST_F(ZkTpcHTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, "q5", expected_sort, db_name);

}

TEST_F(ZkTpcHTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, "q8", expected_sort, db_name);
}

TEST_F(ZkTpcHTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, "q9", expected_sort, db_name);

}



TEST_F(ZkTpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    string test_name = "q18";
    runTest(18, test_name, expected_sort, db_name);
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

