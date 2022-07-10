#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/group_by_aggregate.h>
#include <test/zk/zk_base_test.h>
#include <operators/sort.h>

using namespace emp;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43445, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");


class ZkGroupByAggregateTest : public ZkTest {

protected:
    void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;
    void runDummiesTest(const string & expectedOutputQuery, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void ZkGroupByAggregateTest::runTest(const string &expectedOutputQuery,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

    // produces 25 rows
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    // set up expected output
    SortDefinition expected_order_by = DataUtilities::getDefaultSortDefinition(1);
    std::shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(alice_db, expectedOutputQuery, false);
    expected->setSortOrder(expected_order_by);

    // run secure query
    SortDefinition order_by = DataUtilities::getDefaultSortDefinition(1); // actually 2
    shared_ptr<SecureTable> input = ZkTest::secret_share_input(query, false);
    input->setSortOrder(order_by);

    std::vector<int32_t> groupByCols{0};
    GroupByAggregate aggregate(input, groupByCols, aggregators);

    std::shared_ptr<SecureTable> aggregated = aggregate.run();
    std::shared_ptr<PlainTable> aggregatedReveal = aggregated->reveal();


    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregatedReveal);

    ASSERT_EQ(*expected, *observed);


}

void ZkGroupByAggregateTest::runDummiesTest(const string &expectedOutputQuery,
                                                const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    std::shared_ptr<PlainTable> expected =  DataUtilities::getQueryResults(alice_db, expectedOutputQuery, false);
    expected->setSortOrder(DataUtilities::getDefaultSortDefinition(1));

    // configure and run test
    SortDefinition order_by = DataUtilities::getDefaultSortDefinition(1);
    shared_ptr<SecureTable> input = ZkTest::secret_share_input(query, true);
    input->setSortOrder(order_by);

    std::vector<int32_t> groupByCols{0};
    GroupByAggregate aggregate(input, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal();


    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);
    ASSERT_EQ(*expected, *observed);
}


TEST_F(ZkGroupByAggregateTest, test_count) {
    // set up expected output
    std::string expectedOutputQuery = "SELECT l_orderkey, COUNT(*)::BIGINT cnt FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expectedOutputQuery, aggregators);

}



TEST_F(ZkGroupByAggregateTest, test_sum) {
    // set up expected outputs
    std::string expectedOutputQuery = "SELECT l_orderkey, SUM(l_linenumber)::INTEGER sum_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));
    runTest(expectedOutputQuery, aggregators);

}

TEST_F(ZkGroupByAggregateTest, test_sum_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, SUM(l_linenumber)::INTEGER sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));

    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkGroupByAggregateTest, test_avg) {
    std::string expectedOutputQuery = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runTest(expectedOutputQuery, aggregators);
}

TEST_F(ZkGroupByAggregateTest, test_avg_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkGroupByAggregateTest, test_min) {
    std::string expectedOutputQuery = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkGroupByAggregateTest, test_min_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runDummiesTest(expectedOutputQuery, aggregators);
}

TEST_F(ZkGroupByAggregateTest, test_max) {
    std::string expectedOutputQuery = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runTest(expectedOutputQuery, aggregators);
}

TEST_F(ZkGroupByAggregateTest, test_max_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runDummiesTest(expectedOutputQuery, aggregators);
}

// // brings in about 200 tuples
TEST_F(ZkGroupByAggregateTest, test_tpch_q1_sums) {

    // TODO: if <= 194, then we start to get floating point drift.
    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100 ORDER BY l_orderkey, l_linenumber";
    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1.0 - l_discount) AS disc_price, l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                        " ORDER BY  l_returnflag, l_linestatus";


    string expectedOutputQuery = "SELECT  l_returnflag, l_linestatus, "
                                 "SUM(l_quantity)::BIGINT sum_qty, "
                                 "SUM(l_extendedprice)::BIGINT sum_base_price, "
                                 "SUM(disc_price) sum_disc_price, "
                                 "SUM(charge) sum_charge "
                                 "FROM (" + inputQuery + ") subquery WHERE NOT dummy "
                                                         "GROUP BY l_returnflag, l_linestatus "
                                                         "ORDER BY l_returnflag, l_linestatus";

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, expectedOutputQuery, false, 2);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};
    std::vector<int32_t> groupByCols{0, 1};


    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    shared_ptr<SecureTable> input = ZkTest::secret_share_input(inputQuery, true);

    // sort alice + bob inputs after union
    Sort sort(input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal(PUBLIC);

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);


    ASSERT_EQ(*expected, *observed);

}


TEST_F(ZkGroupByAggregateTest, test_tpch_q1_avg_cnt) {

    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194 ORDER BY l_orderkey, l_linenumber) selection\n"
                        " ORDER BY l_returnflag, l_linestatus ";

    string expectedOutputQuery =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  FLOOR(avg(l_quantity))::BIGINT as avg_qty, \n"
                                  "  FLOOR(avg(l_extendedprice))::BIGINT as avg_price, \n"
                                  "  FLOOR(avg(l_discount))::BIGINT as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + inputQuery + ") subq\n"
                                                          " where NOT dummy\n"
                                                          "group by \n"
                                                          "  l_returnflag, \n"
                                                          "  l_linestatus \n"
                                                          " \n"
                                                          "order by \n"
                                                          "  l_returnflag, l_linestatus";

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, expectedOutputQuery, false, 2);

    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    shared_ptr<SecureTable> input = ZkTest::secret_share_input(inputQuery, true);


    // sort alice + bob inputs after union -- TODO: this is deprecated
    Sort sort(input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal(PUBLIC);

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *observed);

}

TEST_F(ZkGroupByAggregateTest, tpch_q1) {

    // TODO: was <= 194, reduced owing to floating point drift
    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100  ORDER BY l_orderkey, l_linenumber";
    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                                                  " ORDER BY l_returnflag, l_linestatus";

    string expectedOutputQuery =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  sum(l_quantity)::BIGINT as sum_qty, \n"
                                  "  sum(l_extendedprice)::BIGINT as sum_base_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount))::BIGINT as sum_disc_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax))::BIGINT as sum_charge, \n"
                                  "  FLOOR(avg(l_quantity))::BIGINT as avg_qty, \n"
                                  "  FLOOR(avg(l_extendedprice))::BIGINT as avg_price, \n"
                                  "  FLOOR(avg(l_discount))::BIGINT as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + inputTuples + ") input "
                                                           " where  l_shipdate <= date '1998-08-03'  "
                                                           "group by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus \n"
                                                           " \n"
                                                           "order by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus";

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, expectedOutputQuery, false, 2);

    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    shared_ptr<SecureTable> input = ZkTest::secret_share_input(inputQuery, true);

    // sort alice + bob inputs after union -- TODO: this is deprecated
    Sort sort(input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal(PUBLIC);

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *observed);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





