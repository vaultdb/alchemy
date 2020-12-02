#include <test/support/EmpBaseTest.h>
#include <util/data_utilities.h>
#include <operators/operator.h>
#include <operators/support/aggregate_id.h>
#include <operators/secure_sql_input.h>
#include <operators/scalar_aggregate.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54324, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


using namespace vaultdb;

class SecureScalarAggregateTest : public EmpBaseTest {

protected:
  void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;
//  void runDummiesTest(const string & expectedOutputQuery, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void SecureScalarAggregateTest::runTest(const string &expectedOutputQuery,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

  // produces 25 rows
  std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10"; //ORDER BY (1), (2)";


  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults("tpch_unioned", expectedOutputQuery, false);
  //types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SecureSqlInput(dbName, query, false, netio, FLAGS_party));


  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  std::shared_ptr<QueryTable> aggregated = aggregate->run();
  std::shared_ptr<QueryTable> aggregatedReveal = aggregated->reveal();


  // need to delete dummies from observed output to compare it to expected
  std::shared_ptr<QueryTable> observed = DataUtilities::removeDummies(aggregatedReveal);
  std::cout << "Observed output: " << observed->toString(true) << std::endl;

  ASSERT_EQ(*expectedOutput, *observed);

//  // define the aggregate
//  std::vector<ScalarAggregateDefinition> aggregators;
//  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg"));
//
//  // place aggregate definition in an Operator
//  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
//  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();
//
//  // run it
//  std::shared_ptr<QueryTable> output = aggregate->run();
//  std::shared_ptr<QueryTable> observed = output->reveal();
//  observed->setSchema(expectedOutput->getSchema());
//
//
//  ASSERT_EQ(*expectedOutput, *observed);


}


TEST_F(SecureScalarAggregateTest, test_avg) {
    // predicates in WHERE clause instead of limit to line up selections in tpch_unioned verifier
    std::string query = "SELECT l_extendedprice FROM lineitem  WHERE l_extendedprice <= 911.0";


    // set up the expected results:
    std::string expectedOutputQuery = "SELECT AVG(l_extendedprice) avg FROM (" + query + ") selection";

    std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults("tpch_unioned", expectedOutputQuery, false);
    types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

    // provide the aggregator with inputs:
    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, query, false, netio, FLAGS_party));

    // define the aggregate
    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg"));

    // place aggregate definition in an Operator
    ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    // run it
    std::shared_ptr<QueryTable> output = aggregate->run();
    std::shared_ptr<QueryTable> observed = output->reveal();
    observed->setSchema(expectedOutput->getSchema());


    ASSERT_EQ(*expectedOutput, *observed);

}


TEST_F(SecureScalarAggregateTest, test_avg_new) {
  std::string expectedOutputQuery = "SELECT AVG(l_linenumber) avg_lineno FROM lineitem WHERE l_orderkey <= 10";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
  runTest(expectedOutputQuery, aggregators);
}



TEST_F(SecureScalarAggregateTest, test_sum) {
  // set up expected outputs
  std::string expectedOutputQuery = "SELECT SUM(l_linenumber) sum_lineno FROM lineitem WHERE l_orderkey <= 10";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));
  runTest(expectedOutputQuery, aggregators);

}

//TEST_F(SecureScalarAggregateTest, test_min) {
//  std::string expectedOutputQuery = "SELECT MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10";
//  std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
//  runTest(expectedOutputQuery, aggregators);
//}
//
//
//TEST_F(SecureScalarAggregateTest, test_max) {
//  std::string expectedOutputQuery = "SELECT MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10";
//  std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
//  runTest(expectedOutputQuery, aggregators);
//}

TEST_F(SecureScalarAggregateTest, test_count) {
    // set up expected output
    std::string expectedOutputQuery = "SELECT COUNT(*) cnt FROM lineitem WHERE l_orderkey <= 10";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::COUNT, "cnt"));
    runTest(expectedOutputQuery, aggregators);

}


//TEST_F(SecureScalarAggregateTest, test_count) {
//  // predicates in WHERE clause instead of limit to line up selections in tpch_unioned verifier
//  std::string query = "SELECT l_extendedprice FROM lineitem  WHERE l_extendedprice <= 911.0";
//
//
//  // set up the expected results:
//  std::string expectedOutputQuery = "SELECT COUNT(*) cnt FROM (" + query + ") selection";
//
//  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults("tpch_unioned", expectedOutputQuery, false);
//  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();
//
//  // provide the aggregator with inputs:
//  std::shared_ptr<Operator> input(new SecureSqlInput(dbName, query, false, netio, FLAGS_party));
//
//  // define the aggregate
//  std::vector<ScalarAggregateDefinition> aggregators;
//  aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));
//
//  // place aggregate definition in an Operator
//  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
//  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();
//
//  // run it
//  std::shared_ptr<QueryTable> output = aggregate->run();
//  std::shared_ptr<QueryTable> observed = output->reveal();
//  observed->setSchema(expectedOutput->getSchema());
//
//
//  ASSERT_EQ(*expectedOutput, *observed);
//
//}


//TEST_F(SecureScalarAggregateTest, test_sum) {
//  // predicates in WHERE clause instead of limit to line up selections in tpch_unioned verifier
//  std::string query = "SELECT l_extendedprice FROM lineitem  WHERE l_extendedprice <= 911.0";
//
//
//  // set up the expected results:
//  std::string expectedOutputQuery = "SELECT SUM(l_extendedprice) avg FROM (" + query + ") selection";
//
//  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults("tpch_unioned", expectedOutputQuery, false);
//  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();
//
//  // provide the aggregator with inputs:
//  std::shared_ptr<Operator> input(new SecureSqlInput(dbName, query, false, netio, FLAGS_party));
//
//  // define the aggregate
//  std::vector<ScalarAggregateDefinition> aggregators;
//  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg"));
//
//  // place aggregate definition in an Operator
//  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
//  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();
//
//  // run it
//  std::shared_ptr<QueryTable> output = aggregate->run();
//  std::shared_ptr<QueryTable> observed = output->reveal();
//  observed->setSchema(expectedOutput->getSchema());
//
//
//  ASSERT_EQ(*expectedOutput, *observed);
//
//}


// TODO: add tests for min/max/sum/count
// TODO: run this with multiple aggregates, e.g., SELECT AVG(x), SUM(y) FROM z;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    return RUN_ALL_TESTS();
}

