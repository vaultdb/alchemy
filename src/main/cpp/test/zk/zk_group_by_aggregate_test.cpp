#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/group_by_aggregate.h>
#include <test/zk/zk_base_test.h>
#include <operators/sort.h>
#include <operators/zk_sql_input.h>

using namespace emp;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43445, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");


class ZkGroupByAggregateTest : public ZkTest {

protected:
    void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;
    void runDummiesTest(const string & expected_sql, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void ZkGroupByAggregateTest::runTest(const string &expected_sql,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

    // produces 25 rows
    std::string sql = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    // set up expected output
    SortDefinition expected_order_by = DataUtilities::getDefaultSortDefinition(1);
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);
    expected->setSortOrder(expected_order_by);

    // run secure query
    SortDefinition order_by = DataUtilities::getDefaultSortDefinition(1); // actually 2
    auto input = new ZkSqlInput(db_name_, sql, false);
    input->setSortOrder(order_by);

    std::vector<int32_t> groupByCols{0};
    GroupByAggregate aggregate(input, groupByCols, aggregators);

    SecureTable * aggregated = aggregate.run();
    PlainTable *observed = aggregated->reveal();


    ASSERT_EQ(*expected, *observed);
    
    delete observed;
    delete expected;


}

void ZkGroupByAggregateTest::runDummiesTest(const string &expected_sql,
                                                const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string sql = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    PlainTable *expected =  DataUtilities::getQueryResults(unioned_db_, expected_sql, false);
    expected->setSortOrder(DataUtilities::getDefaultSortDefinition(1));

    // configure and run test
    SortDefinition order_by = DataUtilities::getDefaultSortDefinition(1);
    auto input = new ZkSqlInput(db_name_, sql, true);
    input->setSortOrder(order_by);

    std::vector<int32_t> groupByCols{0};
    GroupByAggregate aggregate(input, groupByCols, aggregators);

    PlainTable *observed = aggregate.run()->reveal();


    ASSERT_EQ(*expected, *observed);
    
    delete observed;
    delete expected;
}


TEST_F(ZkGroupByAggregateTest, test_count) {
    // set up expected output
    std::string expected_sql = "SELECT l_orderkey, COUNT(*)::BIGINT cnt FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expected_sql, aggregators);

}



TEST_F(ZkGroupByAggregateTest, test_sum) {
    // set up expected outputs
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INTEGER sum_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));
    runTest(expected_sql, aggregators);

}

TEST_F(ZkGroupByAggregateTest, test_sum_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INTEGER sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));

    runDummiesTest(expected_sql, aggregators);
}


TEST_F(ZkGroupByAggregateTest, test_avg) {
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runTest(expected_sql, aggregators);
}

TEST_F(ZkGroupByAggregateTest, test_avg_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runDummiesTest(expected_sql, aggregators);
}


TEST_F(ZkGroupByAggregateTest, test_min) {
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expected_sql, aggregators);
}


TEST_F(ZkGroupByAggregateTest, test_min_dummies) {


    std::string sql = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM (" + sql + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runDummiesTest(expected_sql, aggregators);
}

TEST_F(ZkGroupByAggregateTest, test_max) {
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(ZkGroupByAggregateTest, test_max_dummies) {
    std::string sql = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM (" + sql + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runDummiesTest(expected_sql, aggregators);
}

// // brings in about 200 tuples
TEST_F(ZkGroupByAggregateTest, test_tpch_q1_sums) {

    // TODO: if <= 194, then we start to get floating point drift.
    string input_tuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100 ORDER BY l_orderkey, l_linenumber";
    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1.0 - l_discount) AS disc_price, l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + input_tuples + ") selection \n"
                        " ORDER BY  l_returnflag, l_linestatus";


    string expected_sql = "SELECT  l_returnflag, l_linestatus, "
                                 "SUM(l_quantity)::BIGINT sum_qty, "
                                 "SUM(l_extendedprice)::BIGINT sum_base_price, "
                                 "SUM(disc_price) sum_disc_price, "
                                 "SUM(charge) sum_charge "
                                 "FROM (" + sql + ") subquery WHERE NOT dummy "
                                                         "GROUP BY l_returnflag, l_linestatus "
                                                         "ORDER BY l_returnflag, l_linestatus";

    PlainTable * expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 2);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};
    std::vector<int32_t> groupByCols{0, 1};


    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    auto input = new ZkSqlInput(db_name_, sql, true);


    // sort alice + bob inputs after union
    Sort sort(input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    PlainTable * observed = aggregate.run()->reveal(PUBLIC);



    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;
}


TEST_F(ZkGroupByAggregateTest, test_tpch_q1_avg_cnt) {

    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194 ORDER BY l_orderkey, l_linenumber) selection\n"
                        " ORDER BY l_returnflag, l_linestatus ";

    string expected_sql =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  FLOOR(avg(l_quantity))::BIGINT as avg_qty, \n"
                                  "  FLOOR(avg(l_extendedprice))::BIGINT as avg_price, \n"
                                  "  FLOOR(avg(l_discount))::BIGINT as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + sql + ") subq\n"
                                                          " where NOT dummy\n"
                                                          "group by \n"
                                                          "  l_returnflag, \n"
                                                          "  l_linestatus \n"
                                                          " \n"
                                                          "order by \n"
                                                          "  l_returnflag, l_linestatus";

    PlainTable * expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 2);

    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    auto input = new ZkSqlInput(db_name_, sql, true);



    // sort alice + bob inputs after union -- TODO: this is deprecated
    Sort sort(input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    PlainTable * observed = aggregate.run()->reveal(PUBLIC);

    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;
}

TEST_F(ZkGroupByAggregateTest, tpch_q1) {

    // TODO: was <= 194, reduced owing to floating point drift
    string input_tuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100  ORDER BY l_orderkey, l_linenumber";
    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + input_tuples + ") selection \n"
                                                  " ORDER BY l_returnflag, l_linestatus";

    string expected_sql =  "select \n"
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
                                  "from (" + input_tuples + ") input "
                                                           " where  l_shipdate <= date '1998-08-03'  "
                                                           "group by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus \n"
                                                           " \n"
                                                           "order by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus";

    PlainTable * expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 2);

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

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);

    auto input = new ZkSqlInput(db_name_, sql, true, sort_def);
//
//    SecureTable * input = ZkTest::secret_share_input(inputQuery, true);

    // sort alice + bob inputs after union -- TODO: this is deprecated
    Sort sort(input, sort_def);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    PlainTable *observed = aggregate.run()->reveal(PUBLIC);


    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete observed;

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





