#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/support/aggregate_id.h>
#include <operators/nested_loop_aggregate.h>
#include "plain_base_test.h"
#include "operators/sort.h"

DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column or compressed)");


class NestedLoopAggregateTest :  public PlainBaseTest  {


protected:
    void runTest(const std::string & expected_sql, const std::vector<ScalarAggregateDefinition> & aggregators) const;
    void runDummiesTest(const std::string & expected_sql, const std::vector<ScalarAggregateDefinition> & aggregators) const;
};

void NestedLoopAggregateTest::runTest(const string &expected_sql,
                                   const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY random()";
    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 1);


    std::vector<int32_t> group_bys{0};

    auto input = new SqlInput(db_name_, query, false, SortDefinition());

    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, SortDefinition (), FLAGS_cutoff);

    auto sort = new Sort(aggregate, DataUtilities::getDefaultSortDefinition(1));

    PlainTable* observed = sort->run();
    DataUtilities::removeDummies(observed);


    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete sort;

}

void NestedLoopAggregateTest::runDummiesTest(const string &expected_sql,
                                          const vector<ScalarAggregateDefinition> &aggregators) const {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE "
                        "l_orderkey <=" + std::to_string(FLAGS_cutoff) + " ORDER BY random()";
    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 1);


    std::vector<int32_t> group_bys{0};

    auto input = new SqlInput(db_name_, query, true, SortDefinition());

    auto  aggregate = new NestedLoopAggregate(input, group_bys, aggregators, SortDefinition (), FLAGS_cutoff);
    auto sort = new Sort(aggregate, DataUtilities::getDefaultSortDefinition(1));

    PlainTable* observed = sort->run();
    DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete sort;

}


TEST_F(NestedLoopAggregateTest, test_count) {
    std::string expected_sql = "SELECT l_orderkey, COUNT(*)::BIGINT FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};

    runTest(expected_sql, aggregators);
}

TEST_F(NestedLoopAggregateTest, test_count_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, COUNT(*) cnt FROM (" + query + ") "
                                     "subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};

    runDummiesTest(expected_sql, aggregators);
}


TEST_F(NestedLoopAggregateTest, test_sum) {
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INT sum_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runTest(expected_sql, aggregators);
}

TEST_F(NestedLoopAggregateTest, test_sum_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INT sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runDummiesTest(expected_sql, aggregators);
}


TEST_F(NestedLoopAggregateTest, test_avg) {
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};

    runTest(expected_sql, aggregators);

}

TEST_F(NestedLoopAggregateTest, test_avg_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};

    runDummiesTest(expected_sql, aggregators);

}


TEST_F(NestedLoopAggregateTest, test_min) {
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};

    runTest(expected_sql, aggregators);

}

TEST_F(NestedLoopAggregateTest, test_min_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};

    runDummiesTest(expected_sql, aggregators);

}


TEST_F(NestedLoopAggregateTest, test_max) {
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};

    runTest(expected_sql, aggregators);

}

TEST_F(NestedLoopAggregateTest, test_max_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};

    runDummiesTest(expected_sql, aggregators);


}

// brings in about 200 tuples
TEST_F(NestedLoopAggregateTest, test_tpch_q1_sums) {


    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1.0 - l_discount) AS disc_price, l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + ") "
                        "selection \n"
                        " ORDER BY  random()";

    string expected_sql = "SELECT  l_returnflag, l_linestatus, "
                                 "SUM(l_quantity) sum_qty, "
                                 "SUM(l_extendedprice) sum_base_price, "
                                 "SUM(disc_price) sum_disc_price, "
                                 "SUM(charge) sum_charge "
                                 "FROM (" + sql + ") subquery WHERE NOT dummy "
                                                         "GROUP BY l_returnflag, l_linestatus";


    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 2);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};
    std::vector<int32_t> group_bys{0, 1};


    auto input = new SqlInput(db_name_, sql, true, SortDefinition());
    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators,SortDefinition (),  6);

    Sort<bool> sort(aggregate, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed = sort.run();
    DataUtilities::removeDummies(observed);

ASSERT_EQ(*expected, *observed);

delete expected;

}


TEST_F(NestedLoopAggregateTest, test_tpch_q1_avg_cnt) {

    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY l_orderkey, l_linenumber) selection\n"
                        " ORDER BY random() ";

    string expected_sql =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + sql + ") subq\n"
                                  " where NOT dummy\n"
                                  "group by \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus \n"
                                  " \n"
                                  "order by \n"
                                  "  l_returnflag, l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 2);

    std::vector<int32_t> group_bys{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
        ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
        ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
        ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
        ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};


    auto input = new SqlInput(db_name_, sql, true, SortDefinition());
    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, SortDefinition (), 6);

    Sort<bool> sort(aggregate, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed  = sort.run();
    DataUtilities::removeDummies(observed);


    ASSERT_EQ(*expected, *observed);
    delete expected;

}

TEST_F(NestedLoopAggregateTest, tpch_q1) {

    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_orderkey, l_linenumber";
    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                        " ORDER BY random()";

    string expected_sql =  "select \n"
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
                                  "from (" + inputTuples + ") input "
                                  " where  l_shipdate <= date '1998-08-03'  "
                                  "group by \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus \n"
                                  " \n"
                                  "order by \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expected_sql, false, 2);

    std::vector<int32_t> group_bys{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
        ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
        ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
        ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
        ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
        ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    auto input = new SqlInput(db_name_, sql, true, SortDefinition());
    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, SortDefinition (), 6);


    Sort<bool> sort(aggregate, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed  = sort.run();
    DataUtilities::removeDummies(observed);

ASSERT_EQ(*expected, *observed);
delete expected;

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

