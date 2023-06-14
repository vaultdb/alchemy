#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/support/aggregate_id.h>
#include <operators/nested_loop_aggregate.h>
#include "plain_base_test.h"
#include "operators/sort.h"

DEFINE_string(storage, "row", "storage model for tables (row or column)");col
class NestedLoopAggregateTest :  public PlainBaseTest  {


protected:
    void runTest(const std::string & expectedOutputQuery, const std::vector<ScalarAggregateDefinition> & aggregators) const;
    void runDummiesTest(const std::string & expectedOutputQuery, const std::vector<ScalarAggregateDefinition> & aggregators) const;
};

void NestedLoopAggregateTest::runTest(const string &expectedOutputQuery,
                                   const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY random()";
    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expectedOutputQuery, false, 1, storage_model_);


    std::vector<int32_t> groupByCols{0};

    SqlInput input(db_name_, query, false, storage_model_, SortDefinition());

    NestedLoopAggregate aggregate(&input, groupByCols, aggregators, 7);

    //Sort<bool> sort(&aggregate, DataUtilities::getDefaultSortDefinition(1));

    //PlainTable* aggregated = sort.run();

    PlainTable *observed = aggregate.run()->reveal();


    ASSERT_EQ(*expected, *observed);
    delete observed;
    delete expected;

}

void NestedLoopAggregateTest::runDummiesTest(const string &expectedOutputQuery,
                                          const vector<ScalarAggregateDefinition> &aggregators) const {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE "
                        "l_orderkey <=10 ORDER BY random()";
    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expectedOutputQuery, false, 1, storage_model_);


    std::vector<int32_t> groupByCols{0};

    SqlInput input(db_name_, query, true, storage_model_, SortDefinition());

    NestedLoopAggregate aggregate(&input, groupByCols, aggregators, 7);
    //Sort<bool> sort(&aggregate, DataUtilities::getDefaultSortDefinition(1));

    //std::shared_ptr<PlainTable> aggregated = sort.run();

    // need to delete dummies from observed output to compare it to expected
    PlainTable *observed = aggregate.run()->reveal();


    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;

}


TEST_F(NestedLoopAggregateTest, test_count) {
    std::string expectedOutputQuery = "SELECT l_orderkey, COUNT(*)::BIGINT FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};

    runTest(expectedOutputQuery, aggregators);
}

TEST_F(NestedLoopAggregateTest, test_count_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, COUNT(*) cnt FROM (" + query + ") "
                                     "subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};

    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(NestedLoopAggregateTest, test_sum) {
    std::string expectedOutputQuery = "SELECT l_orderkey, SUM(l_linenumber)::INT sum_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runTest(expectedOutputQuery, aggregators);
}

TEST_F(NestedLoopAggregateTest, test_sum_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, SUM(l_linenumber)::INT sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(NestedLoopAggregateTest, test_avg) {
    std::string expectedOutputQuery = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};

    runTest(expectedOutputQuery, aggregators);

}

TEST_F(NestedLoopAggregateTest, test_avg_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};

    runDummiesTest(expectedOutputQuery, aggregators);

}


TEST_F(NestedLoopAggregateTest, test_min) {
    std::string expectedOutputQuery = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};

    runTest(expectedOutputQuery, aggregators);

}

TEST_F(NestedLoopAggregateTest, test_min_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};

    runDummiesTest(expectedOutputQuery, aggregators);

}


TEST_F(NestedLoopAggregateTest, test_max) {
    std::string expectedOutputQuery = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};

    runTest(expectedOutputQuery, aggregators);

}

TEST_F(NestedLoopAggregateTest, test_max_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};

    runDummiesTest(expectedOutputQuery, aggregators);


}

// brings in about 200 tuples
TEST_F(NestedLoopAggregateTest, test_tpch_q1_sums) {


    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1.0 - l_discount) AS disc_price, l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194  ORDER BY l_orderkey, l_linenumber) "
                        "selection \n"
                        " ORDER BY  random()";

    string expectedOutputQuery = "SELECT  l_returnflag, l_linestatus, "
                                 "SUM(l_quantity) sum_qty, "
                                 "SUM(l_extendedprice) sum_base_price, "
                                 "SUM(disc_price) sum_disc_price, "
                                 "SUM(charge) sum_charge "
                                 "FROM (" + inputQuery + ") subquery WHERE NOT dummy "
                                                         "GROUP BY l_returnflag, l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expectedOutputQuery, false, 2, storage_model_);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};
    std::vector<int32_t> groupByCols{0, 1};


    SqlInput input(db_name_, inputQuery, true, storage_model_, SortDefinition());

    NestedLoopAggregate aggregate(&input, groupByCols, aggregators, 194);

    //Sort<bool> sort(&aggregate, DataUtilities::getDefaultSortDefinition(2));
    //std::shared_ptr<PlainTable> observed = sort.run();
    PlainTable *observed = aggregate.run()->reveal(PUBLIC);


ASSERT_EQ(*expected, *observed);

delete observed;
delete expected;;

}


TEST_F(NestedLoopAggregateTest, test_tpch_q1_avg_cnt) {

    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194  ORDER BY l_orderkey, l_linenumber) selection\n"
                        " ORDER BY random() ";

    string expectedOutputQuery =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + inputQuery + ") subq\n"
                                  " where NOT dummy\n"
                                  "group by \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus \n"
                                  " \n"
                                  "order by \n"
                                  "  l_returnflag, l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expectedOutputQuery, false, 2, storage_model_);

    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
        ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
        ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
        ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
        ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SqlInput input(db_name_, inputQuery, true, storage_model_, SortDefinition());

    NestedLoopAggregate aggregate(&input, groupByCols, aggregators, 194);

    //std::shared_ptr<PlainTable> aggregated = aggregate.run();

//    Sort<bool> sort(&aggregate, DataUtilities::getDefaultSortDefinition(2));
//    std::shared_ptr<PlainTable> observed = sort.run();
PlainTable *observed = aggregate.run()->reveal(PUBLIC);


ASSERT_EQ(*expected, *observed);
delete expected;
delete observed;

}

TEST_F(NestedLoopAggregateTest, tpch_q1) {

    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 194 ORDER BY l_orderkey, l_linenumber";
    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                        " ORDER BY random()";

    string expectedOutputQuery =  "select \n"
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

    PlainTable *expected = DataUtilities::getExpectedResults(db_name_, expectedOutputQuery, false, 2, storage_model_);

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

    SqlInput input(db_name_, inputQuery, true, storage_model_, SortDefinition());

    NestedLoopAggregate aggregate(&input, groupByCols, aggregators, 194);

//    Sort<bool> sort(&aggregate, DataUtilities::getDefaultSortDefinition(2));
//    std::shared_ptr<PlainTable> observed = sort.run();
PlainTable *observed = aggregate.run()->reveal(PUBLIC);


ASSERT_EQ(*expected, *observed);
delete expected;
delete observed;

}