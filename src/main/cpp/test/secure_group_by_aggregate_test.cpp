#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/group_by_aggregate.h>
#include <test/support/EmpBaseTest.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>

using namespace emp;
using namespace vaultdb::types;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");


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
    std::shared_ptr<QueryTable> expected = EmpBaseTest::getExpectedOutput(expectedOutputQuery, 1);

    // run secure query
    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, query, false, sortDefinition, netio, FLAGS_party));

    // sort alice + bob inputs after union
    Sort *sortOp = new Sort(sortDefinition, input);
    std::shared_ptr<Operator> sort = sortOp->getPtr();


    std::vector<int32_t> groupByCols;
    groupByCols.push_back(0);

    GroupByAggregate *aggregateOp = new GroupByAggregate(sort, groupByCols, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    std::shared_ptr<QueryTable> aggregated = aggregate->run();
    std::shared_ptr<QueryTable> aggregatedReveal = aggregated->reveal();


    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<QueryTable> observed = DataUtilities::removeDummies(aggregatedReveal);
    std::cout << "Observed output: " << observed->toString(true) << std::endl;

    ASSERT_EQ(*expected, *observed);


}

void SecureGroupByAggregateTest::runDummiesTest(const string &expectedOutputQuery,
                                                const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    std::shared_ptr<QueryTable> expected = EmpBaseTest::getExpectedOutput(expectedOutputQuery, 1);

    // configure and run test
    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, query, true, sortDefinition, netio, FLAGS_party));

    // sort alice + bob inputs after union
    Sort *sortOp = new Sort(sortDefinition, input);
    std::shared_ptr<Operator> sort = sortOp->getPtr();

    std::vector<int32_t> groupByCols;
    groupByCols.push_back(0);

    GroupByAggregate *aggregateOp = new GroupByAggregate(sort, groupByCols, aggregators);

    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();
    std::shared_ptr<QueryTable> aggregated = aggregate->run()->reveal();


    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<QueryTable> observed = DataUtilities::removeDummies(aggregated);
    ASSERT_EQ(*expected, *observed);
}


TEST_F(SecureGroupByAggregateTest, test_count) {
    // set up expected output
    std::string expectedOutputQuery = "SELECT l_orderkey, COUNT(*) cnt FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));
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



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





