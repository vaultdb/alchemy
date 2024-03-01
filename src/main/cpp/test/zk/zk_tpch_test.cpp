#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <operators/sort_merge_aggregate.h>
#include <parser/plan_parser.h>
#include <test/zk/zk_base_test.h>

using namespace emp;
using namespace vaultdb;

#define TRUNCATE_INPUTS 1

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54327, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(empty_db, "tpch_empty", "empty db name");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


class ZkTpcHTest : public ZkTest {


protected:

    // depends on truncate-tpch-set.sql
    int input_tuple_limit_ = 1000; // when TRUNCATE_INPUTS == 1, tune this to change the size of our test input data
    void runTest(const int &test_id, const SortDefinition &expected_sort);

};


void ZkTpcHTest::runTest(const int &test_id, const SortDefinition &expected_sort) {

    int limit = (TRUNCATE_INPUTS) ? input_tuple_limit_ : -1; // set to -1 for full test

    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/zk-q" + std::to_string(test_id) + ".json";
    PlanParser<Bit> parser(db_name_, plan_file, limit);

    SecureOperator *root = parser.getRoot();
    SecureTable *observed = root->run();

    if(FLAGS_validation) {
        string expected_sql = tpch_queries[test_id];
        if (limit > 0) {
            expected_sql =  zk_tpch_queries[test_id];
            boost::replace_all(expected_sql, "$LIMIT", std::to_string(input_tuple_limit_));
        }

        // use alice DB since she's the prover
        PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);
        if(TRUNCATE_INPUTS) assert(!expected->empty()); // if we're just testing get non-empty inputs

        expected->order_by_ = expected_sort;
        PlainTable *revealed = observed->reveal();
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;

    }
    delete root;
}


TEST_F(ZkTpcHTest, tpch_q01) {
    input_tuple_limit_ = 150;
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, expected_sort);
}


TEST_F(ZkTpcHTest, tpch_q03) {
    input_tuple_limit_ = 1200;
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};


    runTest(3, expected_sort);
}




TEST_F(ZkTpcHTest, tpch_q05) {
    input_tuple_limit_ = 400;
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, expected_sort);

}

TEST_F(ZkTpcHTest, tpch_q08) {
    input_tuple_limit_ = 800;
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    runTest(8, expected_sort);
}

TEST_F(ZkTpcHTest, tpch_q09) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, expected_sort);

}



TEST_F(ZkTpcHTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    input_tuple_limit_ =  400;

    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest(18, expected_sort);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}


