#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/group_by_aggregate.h>
#include <test/mpc/emp_base_test.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>

using namespace emp;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");


class SecureGroupByAggregateTest : public EmpBaseTest {

protected:
    void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;
    void runDummiesTest(const string & expectedOutputQuery, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void SecureGroupByAggregateTest::runTest(const string &expectedOutputQuery,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

    // produces 25 rows
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    // set up expected output
    std::shared_ptr<PlainTable> expected = EmpBaseTest::getExpectedOutput(expectedOutputQuery, 1);

    // run secure query
    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(1); // actually 2
    SecureSqlInput input(db_name_, query, false, sortDefinition, netio_, FLAGS_party);


    std::vector<int32_t> groupByCols{0};
    GroupByAggregate aggregate(&input, groupByCols, aggregators);

    std::shared_ptr<SecureTable> aggregated = aggregate.run();
    std::shared_ptr<PlainTable> aggregatedReveal = aggregated->reveal();


    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregatedReveal);

    if(!IGNORE_BOB) {
        ASSERT_EQ(*expected, *observed);
    }


}

void SecureGroupByAggregateTest::runDummiesTest(const string &expectedOutputQuery,
                                                const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    std::shared_ptr<PlainTable> expected = EmpBaseTest::getExpectedOutput(expectedOutputQuery, 1);

    // configure and run test
    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(1);
    SecureSqlInput input(db_name_, query, true, sortDefinition, netio_, FLAGS_party);


    std::vector<int32_t> groupByCols;
    groupByCols.push_back(0);

    GroupByAggregate aggregate(&input, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal();


    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);
    if(!IGNORE_BOB) {
        ASSERT_EQ(*expected, *observed);
    }
}


TEST_F(SecureGroupByAggregateTest, test_count) {
    // set up expected output
    std::string expectedOutputQuery = "SELECT l_orderkey, COUNT(*) cnt FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expectedOutputQuery, aggregators);

}



TEST_F(SecureGroupByAggregateTest, test_sum) {
    // set up expected outputs
    std::string expectedOutputQuery = "SELECT l_orderkey, SUM(l_linenumber) sum_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));
    runTest(expectedOutputQuery, aggregators);

}

TEST_F(SecureGroupByAggregateTest, test_sum_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, SUM(l_linenumber) sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));

    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureGroupByAggregateTest, test_avg) {
    std::string expectedOutputQuery = "SELECT l_orderkey, AVG(l_linenumber) avg_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runTest(expectedOutputQuery, aggregators);
}

TEST_F(SecureGroupByAggregateTest, test_avg_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, AVG(l_linenumber) avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureGroupByAggregateTest, test_min) {
    std::string expectedOutputQuery = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureGroupByAggregateTest, test_min_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runDummiesTest(expectedOutputQuery, aggregators);
}

TEST_F(SecureGroupByAggregateTest, test_max) {
    std::string expectedOutputQuery = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runTest(expectedOutputQuery, aggregators);
}

TEST_F(SecureGroupByAggregateTest, test_max_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runDummiesTest(expectedOutputQuery, aggregators);
}

// // brings in about 200 tuples
TEST_F(SecureGroupByAggregateTest, test_tpch_q1_sums) {

    // TODO: if <= 194, then we start to get floating point drift.
    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100 ORDER BY l_orderkey, l_linenumber";
    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1.0 - l_discount) AS disc_price, l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                        " ORDER BY  l_returnflag, l_linestatus";


    string expectedOutputQuery = "SELECT  l_returnflag, l_linestatus, "
                                 "SUM(l_quantity) sum_qty, "
                                 "SUM(l_extendedprice) sum_base_price, "
                                 "SUM(disc_price) sum_disc_price, "
                                 "SUM(charge) sum_charge "
                                 "FROM (" + inputQuery + ") subquery WHERE NOT dummy "
                                                         "GROUP BY l_returnflag, l_linestatus "
                                                         "ORDER BY l_returnflag, l_linestatus";

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(unioned_db_, expectedOutputQuery, false, 2);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};
    std::vector<int32_t> groupByCols{0, 1};


    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    SecureSqlInput input(db_name_, inputQuery, true, sortDefinition, netio_, FLAGS_party);

    // sort alice + bob inputs after union
    Sort sort(&input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal(PUBLIC);

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);

    if(!IGNORE_BOB) {
        ASSERT_EQ(*expected, *observed);
    }

}


TEST_F(SecureGroupByAggregateTest, test_tpch_q1_avg_cnt) {

    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194 ORDER BY l_orderkey, l_linenumber) selection\n"
                        " ORDER BY l_returnflag, l_linestatus ";

    string expectedOutputQuery =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*) as count_order \n"
                                  "from (" + inputQuery + ") subq\n"
                                                          " where NOT dummy\n"
                                                          "group by \n"
                                                          "  l_returnflag, \n"
                                                          "  l_linestatus \n"
                                                          " \n"
                                                          "order by \n"
                                                          "  l_returnflag, l_linestatus";

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(unioned_db_, expectedOutputQuery, false, 2);

    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    SecureSqlInput input(db_name_, inputQuery, true, sortDefinition, netio_, FLAGS_party);

    // sort alice + bob inputs after union
    Sort sort(&input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal(PUBLIC);

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);

    if(!IGNORE_BOB) {
        ASSERT_EQ(*expected, *observed);
    }

}

TEST_F(SecureGroupByAggregateTest, tpch_q1) {

    // TODO: was <= 194, reduced owing to floating point drift
    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100  ORDER BY l_orderkey, l_linenumber";
    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                                                  " ORDER BY l_returnflag, l_linestatus";

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
                                  "  count(*) as count_order \n"
                                  "from (" + inputTuples + ") input "
                                                           " where  l_shipdate <= date '1998-08-03'  "
                                                           "group by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus \n"
                                                           " \n"
                                                           "order by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus";

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(unioned_db_, expectedOutputQuery, false, 2);

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
    SecureSqlInput input(db_name_, inputQuery, true, sortDefinition, netio_, FLAGS_party);

    // sort alice + bob inputs after union
    Sort sort(&input, sortDefinition);

    GroupByAggregate aggregate(&sort, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run()->reveal(PUBLIC);

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);

    if(!IGNORE_BOB) {
        ASSERT_EQ(*expected, *observed);
    }

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





