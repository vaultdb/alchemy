#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/scalar_aggregate.h>


using namespace emp;
using namespace vaultdb;
using namespace vaultdb::types;



class ScalarAggregateTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");
    };

    void TearDown() override{
        finalize_plain_prot();
    };

    const std::string dbName = "tpch_unioned";
};


// should just count to 50
TEST_F(ScalarAggregateTest, test_count) {
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem ORDER BY (1)  LIMIT 50";

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
    std::string query = "SELECT l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem ORDER BY l_orderkey, l_linenumber  LIMIT 54";

    // set up the expected results:
    std::string expectedOutputQuery = "SELECT COUNT(*) cnt FROM (" + query + ") selection WHERE NOT dummy";
    std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
    types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

    // provide the aggregator with inputs:
    std::shared_ptr<Operator> input(new SqlInput(dbName, query, true));

    // define the aggregate:
    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));

    // place aggregate definition in an Operator
    ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    // run it
    std::shared_ptr<QueryTable> output = aggregate->run();


    ASSERT_EQ(*expectedOutput, *output);

}


TEST_F(ScalarAggregateTest, test_min) {
    std::string query =  "SELECT l_quantity FROM lineitem WHERE l_orderkey <= 50";

    // set up the expected results:
    std::string expectedOutputQuery = "WITH input AS (" + query + ") SELECT MIN(l_quantity) min_quantity FROM input";
    std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
    types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

    // provide the aggregator with inputs:
    std::shared_ptr<Operator> input(new SqlInput(dbName, query, false));

    // define the aggregate:
    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::MIN, "min_quantity"));

    // place aggregate definition in an Operator:
    ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    // run it:
    std::shared_ptr<QueryTable> output = aggregate->run();

    QueryTuple firstTuple = output->getTuple(0);
    types::Value firstValue = firstTuple.getFieldPtr(0)->getValue();

    ASSERT_TRUE((firstValue == expectedValue).getBool());

}


TEST_F(ScalarAggregateTest, test_max) {
    std::string query =  "SELECT l_tax FROM lineitem WHERE l_orderkey <= 50";

    // set up the expected results:
    std::string expectedOutputQuery = "WITH input AS (" + query + ") SELECT MAX(l_tax) max_tax FROM input";
    std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
    types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

    // provide the aggregator with inputs:
    std::shared_ptr<Operator> input(new SqlInput(dbName, query, false));

    // define the aggregate:
    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::MAX, "max_tax"));

    // place aggregate definition in an Operator:
    ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    // run it:
    std::shared_ptr<QueryTable> output = aggregate->run();

    QueryTuple firstTuple = output->getTuple(0);
    types::Value firstValue = firstTuple.getFieldPtr(0)->getValue();

    ASSERT_TRUE((firstValue == expectedValue).getBool());

}


TEST_F(ScalarAggregateTest, test_sum) {
  std::string query =  "SELECT l_quantity FROM lineitem WHERE l_orderkey <= 50";

  // set up the expected results:
  std::string expectedOutputQuery = "WITH input AS (" + query + ") SELECT SUM(l_quantity) sum_qty FROM input";
  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SqlInput(dbName, query, false));

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::SUM, "sum_qty"));

  // place aggregate definition in an Operator:
  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  // run it:
  std::shared_ptr<QueryTable> output = aggregate->run();

  QueryTuple firstTuple = output->getTuple(0);
  types::Value firstValue = firstTuple.getFieldPtr(0)->getValue();

  ASSERT_TRUE((firstValue == expectedValue).getBool());

}

TEST_F(ScalarAggregateTest, test_sum_dummies) {
  std::string query =  "SELECT l_extendedprice, l_shipinstruct <> 'NONE' AS dummy FROM lineitem WHERE l_orderkey <= 100 ";

  // set up the expected results:
  std::string expectedOutputQuery = "SELECT SUM(l_extendedprice) sum_base_price FROM (" + query + ") selection WHERE NOT dummy";
  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SqlInput(dbName, query, true));

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::SUM, "sum_base_price"));

  // place aggregate definition in an Operator:
  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  // run it:
  std::shared_ptr<QueryTable> output = aggregate->run();

  ASSERT_EQ(*expectedOutput, *output);

}


TEST_F(ScalarAggregateTest, test_avg) {
  std::string query =  "SELECT l_extendedprice FROM lineitem WHERE l_orderkey <= 50";

  // set up the expected results:
  std::string expectedOutputQuery = "WITH input AS (" + query + ") SELECT AVG(l_extendedprice) avg_price FROM input";
  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SqlInput(dbName, query, false));

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg_price"));

  // place aggregate definition in an Operator:
  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  // run it:
  std::shared_ptr<QueryTable> output = aggregate->run();

  QueryTuple firstTuple = output->getTuple(0);
  types::Value firstValue = firstTuple.getFieldPtr(0)->getValue();

  ASSERT_TRUE((firstValue == expectedValue).getBool());

}


TEST_F(ScalarAggregateTest, test_avg_dummies) {
  std::string query =  "SELECT l_discount, l_shipinstruct <> 'NONE' AS dummy FROM lineitem WHERE l_orderkey <= 100 ";

  // set up the expected results:
  std::string expectedOutputQuery = "SELECT AVG(l_discount) avg_disc FROM (" + query + ") selection WHERE NOT dummy";
  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SqlInput(dbName, query, true));

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg_disc"));

  // place aggregate definition in an Operator:
  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  // run it:
  std::shared_ptr<QueryTable> output = aggregate->run();

  ASSERT_EQ(*expectedOutput, *output);

}


TEST_F(ScalarAggregateTest, test_all_sum_dummies) {
  std::string query =  "SELECT l_quantity, l_extendedprice, "
                      "l_extendedprice * (1 - l_discount) l_discprice, "
                      "l_extendedprice * (1 - l_discount) * (1 + l_tax) l_charge, "
                      "l_shipinstruct <> 'NONE' AS dummy "
                      "FROM lineitem WHERE l_orderkey <= 100 ";

  // Ordinals:
  // l_quantity, #0
  // l_extendedprice, #1
  // l_extendedprice * (1 - l_discount) AS disc_price, #2
  // l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge #3

  // set up the expected results:
  std::string expectedOutputQuery = "SELECT SUM(l_quantity) sum_qty, "
                                    "SUM(l_extendedprice) sum_base_price, "
                                    "SUM(l_discprice) sum_disc_price, "
                                    "SUM(l_charge) sum_charge "
                                    "FROM (" + query + ") selection WHERE NOT dummy";
  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SqlInput(dbName, query, true));

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::SUM, "sum_qty"));
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_base_price"));
  aggregators.push_back(ScalarAggregateDefinition(2, AggregateId::SUM, "sum_disc_price"));
  aggregators.push_back(ScalarAggregateDefinition(3, AggregateId::SUM, "sum_charge"));


  // place aggregate definition in an Operator:
  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  // run it:
  std::shared_ptr<QueryTable> output = aggregate->run();

  ASSERT_EQ(*expectedOutput, *output);

}


TEST_F(ScalarAggregateTest, test_all_avg_dummies) {
  std::string query =  "SELECT l_quantity, l_extendedprice, l_discount, "
                       "l_shipinstruct <> 'NONE' AS dummy "
                       "FROM lineitem WHERE l_orderkey <= 100 ";

  // set up the expected results:
  std::string expectedOutputQuery = "SELECT AVG(l_quantity) avg_qty, "
                                    "AVG(l_extendedprice) avg_price, "
                                    "AVG(l_discount) avg_disc "
                                    "FROM (" + query + ") selection WHERE NOT dummy";
  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SqlInput(dbName, query, true));

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg_qty"));
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_price"));
  aggregators.push_back(ScalarAggregateDefinition(2, AggregateId::AVG, "avg_disc"));


  // place aggregate definition in an Operator:
  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  // run it:
  std::shared_ptr<QueryTable> output = aggregate->run();

  ASSERT_EQ(*expectedOutput, *output);

}


TEST_F(ScalarAggregateTest, test_tpch_q1_avg_cnt) {

    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 100) selection";

    string expectedOutputQuery =  "select \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*) as count_order \n"
                                  "from (" + inputQuery + ") subq\n"
                                                          " where NOT dummy";

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);

    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    std::shared_ptr<Operator> input(new SqlInput(dbName, inputQuery, true));

    ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    std::shared_ptr<QueryTable> aggregated = aggregate->run();

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<QueryTable> observed = DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *observed);

}


TEST_F(ScalarAggregateTest, test_all_aggs_tpch_q1) {
  std::string query =  "SELECT l_quantity, l_extendedprice, l_discount, "
                       "l_extendedprice * (1 - l_discount) l_discprice, "
                       "l_extendedprice * (1 - l_discount) * (1 + l_tax) l_charge, "
                       "l_shipinstruct <> 'NONE' AS dummy "
                       "FROM lineitem WHERE l_orderkey <= 100 ";

  // Ordinals:
  // l_quantity, #0
  // l_extendedprice, #1
  // l_discount #2
  // l_extendedprice * (1 - l_discount) AS disc_price, #3
  // l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge #4

  // set up the expected results:
  std::string expectedOutputQuery = "SELECT SUM(l_quantity) sum_qty, "
                                    "SUM(l_extendedprice) sum_base_price, "
                                    "SUM(l_discprice) sum_disc_price, "
                                    "SUM(l_charge) sum_charge, "
                                    "AVG(l_quantity) avg_qty, "
                                    "AVG(l_extendedprice) avg_price, "
                                    "AVG(l_discount) avg_disc, "
                                    "COUNT(*) count_order "
                                    "FROM (" + query + ") selection WHERE NOT dummy";
  std::shared_ptr<QueryTable> expectedOutput = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
  types::Value expectedValue = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
  std::shared_ptr<Operator> input(new SqlInput(dbName, query, true));

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::SUM, "sum_qty"));
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_base_price"));
  aggregators.push_back(ScalarAggregateDefinition(3, AggregateId::SUM, "sum_disc_price"));
  aggregators.push_back(ScalarAggregateDefinition(4, AggregateId::SUM, "sum_charge"));
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg_qty"));
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_price"));
  aggregators.push_back(ScalarAggregateDefinition(2, AggregateId::AVG, "avg_disc"));
  aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "count_order"));


  // place aggregate definition in an Operator:
  ScalarAggregate *aggregateOp = new ScalarAggregate(input, aggregators);
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  // run it:
  std::shared_ptr<QueryTable> output = aggregate->run();

  ASSERT_EQ(*expectedOutput, *output);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}







