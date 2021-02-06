#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/secure_aggregate.h>
#include <operators/support/aggregate_id.h>
#include <data/PsqlDataProvider.h>

using namespace emp;
using namespace vaultdb::types;



class AggregateTest : public ::testing::Test {

protected:
  void SetUp() override{};
  void TearDown() override{};

  void runAggregateTest(const AggregateDef & aggregateDef, const std::string & sql, const std::string & expectedResult);
  std::unique_ptr<QueryTable> getExpectedResult(std::string sql, bool dummyTag);
};


TEST_F(AggregateTest, aggregateSingleCount) {
  std::string sql = "SELECT l_returnflag FROM lineitem LIMIT 10"; // AS q1count
  std::string expectedResultSql = "WITH input AS (SELECT l_returnflag FROM lineitem LIMIT 10) SELECT COUNT(*) AS q1count FROM input";
  std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

  AggregateDef aggregateDef;
  aggregateDef.scalarAggregates.emplace_back(ScalarAggregateDef(0, vaultdb::AggregateId::COUNT, "q1count"));

  runAggregateTest(aggregateDef, sql, expectedResult);
}


TEST_F(AggregateTest, singleGroupbyCount) {
  std::string sql = "SELECT n_regionkey FROM nation"; // AS gbCount
  std::string expectedResultSql = "SELECT COUNT(n_regionkey) AS gbCount FROM nation GROUP BY n_regionkey";
  std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

  AggregateDef aggregateDef;
  aggregateDef.scalarAggregates.emplace_back(ScalarAggregateDef(0, vaultdb::AggregateId::COUNT, "q1count"));
  aggregateDef.groupByOrdinals.emplace_back(0);

  runAggregateTest(aggregateDef, sql, expectedResult);
}


TEST_F(AggregateTest, multipleGroupbyCount) {
  std::string sql = "SELECT n_nationkey, n_regionkey FROM nation";
  std::string expectedResultSql = "SELECT COUNT(n_nationkey) AS nationCount, COUNT(n_regionkey) AS regionCount FROM nation GROUP BY n_regionkey";
  std::string expectedResult = getExpectedResult(expectedResultSql, false)->toString();

  AggregateDef aggregateDef;
  aggregateDef.scalarAggregates.emplace_back(ScalarAggregateDef(0, vaultdb::AggregateId::COUNT, "nationCount"));
  aggregateDef.scalarAggregates.emplace_back(ScalarAggregateDef(1, vaultdb::AggregateId::COUNT, "regionCount"));
  aggregateDef.groupByOrdinals.emplace_back(1);

  runAggregateTest(aggregateDef, sql, expectedResult);
}


void AggregateTest::runAggregateTest(const AggregateDef &aggregateDef, const std::string &sql, const std::string &expectedResult) {
  std::string dbName =  "tpch_alice";


  std::shared_ptr<Operator> input(new SqlInput(dbName, sql, false));
  Aggregate *aggregateOp = new Aggregate(input, aggregateDef); // heap allocate it
  std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

  std::shared_ptr<QueryTable> result = aggregate->run();

  std::cout << "Observed aggregate result: " << std::endl;
  std::cout << *result << std::endl;

  std::cout << "Expected result: " << std::endl
            << expectedResult << std::endl;

  ASSERT_EQ(result->toString(), expectedResult);

}


std::unique_ptr<QueryTable> AggregateTest::getExpectedResult(std::string sql, bool dummyTag) {

  PsqlDataProvider dataProvider;
  return dataProvider.getQueryTable("tpch_alice", sql, dummyTag);

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  return RUN_ALL_TESTS();
}