#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/group_by_aggregate.h>
#include "plain_base_test.h"

DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class GroupByAggregateTest :  public PlainBaseTest  {


protected:
    void runTest(const std::string & expected_sql, const std::vector<ScalarAggregateDefinition> & aggregators) const;
    void runDummiesTest(const std::string & expected_sql, const std::vector<ScalarAggregateDefinition> & aggregators) const;
};

void GroupByAggregateTest::runTest(const string &expected_sql,
                                   const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 1);


    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    std::vector<int32_t> groupByCols{0};

    auto input = new SqlInput(db_name_, query, false, sort_def);

    GroupByAggregate aggregate(input, groupByCols, aggregators);

    PlainTable *aggregated = aggregate.run();


    // need to delete dummies from observed output to compare it to expected
    DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *aggregated);

    delete expected;

}

void GroupByAggregateTest::runDummiesTest(const string &expected_sql,
                                          const vector<ScalarAggregateDefinition> &aggregators) const {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 1);


    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    std::vector<int32_t> group_bys{0};

    auto input = new SqlInput(db_name_, query, true, sort_def);

    GroupByAggregate aggregate(input, group_bys, aggregators);

    PlainTable *aggregated = aggregate.run();

    // need to delete dummies from observed output to compare it to expected
    DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *aggregated);
    delete expected;

}


TEST_F(GroupByAggregateTest, test_count) {
    std::string expected_sql = "SELECT l_orderkey, COUNT(*)::BIGINT FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};

    runTest(expected_sql, aggregators);
}

TEST_F(GroupByAggregateTest, test_count_dummies) {

    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey,COUNT(*)::BIGINT cnt  FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runDummiesTest(expected_sql, aggregators);
}



TEST_F(GroupByAggregateTest, test_sum) {
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INT sum_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runTest(expected_sql, aggregators);
}

TEST_F(GroupByAggregateTest, test_sum_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INT sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runDummiesTest(expected_sql, aggregators);
}


TEST_F(GroupByAggregateTest, test_avg) {
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};

    runTest(expected_sql, aggregators);

}

TEST_F(GroupByAggregateTest, test_avg_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};

    runDummiesTest(expected_sql, aggregators);

}


TEST_F(GroupByAggregateTest, test_min) {
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};

    runTest(expected_sql, aggregators);

}

TEST_F(GroupByAggregateTest, test_min_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};

    runDummiesTest(expected_sql, aggregators);

}


TEST_F(GroupByAggregateTest, test_max) {
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};

    runTest(expected_sql, aggregators);

}

TEST_F(GroupByAggregateTest, test_max_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};

    runDummiesTest(expected_sql, aggregators);


}

// brings in about 200 tuples
TEST_F(GroupByAggregateTest, test_tpch_q1_sums) {


    string input_sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1.0 - l_discount) AS disc_price, l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194  ORDER BY l_orderkey, l_linenumber) selection \n"
                        " ORDER BY  l_returnflag, l_linestatus";

    string expected_sql = "SELECT  l_returnflag, l_linestatus, "
                                 "SUM(l_quantity) sum_qty, "
                                 "SUM(l_extendedprice) sum_base_price, "
                                 "SUM(disc_price) sum_disc_price, "
                                 "SUM(charge) sum_charge "
                                 "FROM (" + input_sql + ") subquery WHERE NOT dummy "
                                                         "GROUP BY l_returnflag, l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 2);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};
    std::vector<int32_t> groupByCols{0, 1};


    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SqlInput(db_name_, input_sql, true, sort_def);

    GroupByAggregate aggregate(input, groupByCols, aggregators);

    PlainTable *aggregated = aggregate.run();

    // need to delete dummies from observed output to compare it to expected
     DataUtilities::removeDummies(aggregated);


    ASSERT_EQ(*expected, *aggregated);

    delete expected;

}


TEST_F(GroupByAggregateTest, test_tpch_q1_avg_cnt) {

    string input_sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194  ORDER BY l_orderkey, l_linenumber) selection\n"
                        " ORDER BY l_returnflag, l_linestatus ";

    string expected_sql =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + input_sql + ") subq\n"
                                  " where NOT dummy\n"
                                  "group by \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus \n"
                                  " \n"
                                  "order by \n"
                                  "  l_returnflag, l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 2);

    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
        ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
        ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
        ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
        ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SqlInput(db_name_, input_sql, true, sort_def);

    GroupByAggregate aggregate(input, groupByCols, aggregators);

    PlainTable *aggregated = aggregate.run();

    // need to delete dummies from observed output to compare it to expected
    DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *aggregated);
    delete expected;


}

TEST_F(GroupByAggregateTest, tpch_q1) {

    string input_rows = "SELECT * FROM lineitem WHERE l_orderkey <= 194 ORDER BY l_orderkey, l_linenumber";
    string input_query = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + input_rows + ") selection \n"
                        " ORDER BY l_returnflag, l_linestatus";

    string expected_sql = "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  sum(l_quantity) as sum_qty, \n"
                                  "  sum(l_extendedprice) as sum_base_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + input_rows + ") input "
                                  " where  l_shipdate <= date '1998-08-03'  "
                                  "group by \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus \n"
                                  " \n"
                                  "order by \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 2);

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
    auto input = new SqlInput(db_name_, input_query, true, sort_def);

    GroupByAggregate aggregate(input, groupByCols, aggregators);

    PlainTable *aggregated = aggregate.run();

    // need to delete dummies from observed output to compare it to expected
    DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *aggregated);

    delete expected;

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
