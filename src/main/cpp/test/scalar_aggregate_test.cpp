#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/support/aggregate_id.h>
#include <operators/scalar_aggregate.h>


using namespace emp;
using namespace vaultdb::types;



class ScalarAggregateTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
    const std::string dbName = "tpch_unioned";
};


// should just count to 50
TEST_F(ScalarAggregateTest, test_count) {
    std::string query = "SELECT l_extendedprice FROM lineitem ORDER BY (1)  LIMIT 50";

    std::shared_ptr<Operator> input(new SqlInput(dbName, query, false));

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));

    ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    std::shared_ptr<QueryTable> output = aggregate->run();

    QueryTuple firstTuple = output->getTuple(0);
    types::Value firstValue = firstTuple.getFieldPtr(0)->getValue();
    types::Value expectedValue((int64_t) 50);

    ASSERT_TRUE((expectedValue == firstValue).getBool());

}


TEST_F(ScalarAggregateTest, test_count_dummies) {
    std::string query = "SELECT l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem ORDER BY (1)  LIMIT 54";

    // set up the expected results:
    std::string expectedOutputQuery = "SELECT COUNT(*) cnt FROM (" + query + ") selection WHERE NOT dummy";
    std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
    types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

    // provide the aggregator with inputs:
    std::shared_ptr<Operator> input(new SqlInput(dbName, query, true));

    // define the aggregate
    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));

    // place aggregate definition in an Operator
    ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    // run it
    std::shared_ptr<QueryTable> output = aggregate->run();


    ASSERT_EQ(*expectedOutput, *output);

}


// TODO: add unit tests for sum, min, max, avg
// TODO: try this with multiple aggregates in a single SELECT

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}







