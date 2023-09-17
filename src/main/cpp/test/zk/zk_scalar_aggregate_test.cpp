#include <util/data_utilities.h>
#include <operators/operator.h>
#include <operators/support/aggregate_id.h>
#include <operators/scalar_aggregate.h>
#include <test/zk/zk_base_test.h>
#include "operators/zk_sql_input.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 65431, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(empty_db, "tpch_empty", "empty db name");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


using namespace vaultdb;

class ZkScalarAggregateTest : public ZkTest {

protected:
  void runTest(const string & expected_sql, const vector<ScalarAggregateDefinition> & aggregators) const;
  void runDummiesTest(const string & expected_sql, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void ZkScalarAggregateTest::runTest(const string &expected_sql,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

  // produces 25 rows
  std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10"; //ORDER BY (1), (2)";


  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);

  // provide the aggregator with inputs:
  auto input = new ZkSqlInput(db_name_, query, false);
  ScalarAggregate aggregate(input, aggregators);
  PlainTable *observed = aggregate.run()->reveal();

  ASSERT_EQ(*expected, *observed);
    delete expected;
    delete observed;
}


void ZkScalarAggregateTest::runDummiesTest(const string &expected_sql,
                                        const vector<ScalarAggregateDefinition> & aggregators) const {

  // produces 25 rows
  std::string sql = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";

  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);
  //Field expectedField = expectedOutput->getTuplePtr(0)->getFieldPtr(0)->getValue();

  // provide the aggregator with inputs:
    auto input = new ZkSqlInput(db_name_, sql, true);

    ScalarAggregate aggregate(input, aggregators);

  SecureTable *aggregated = aggregate.run();
  PlainTable *observed = aggregated->reveal();




  ASSERT_EQ(*expected, *observed);

  delete expected;
  delete observed;

}



TEST_F(ZkScalarAggregateTest, test_count) {
    // set up expected output
    std::string expectedOutputQuery = "SELECT COUNT(*)::BIGINT cnt FROM lineitem WHERE l_orderkey <= 10";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));
    runTest(expectedOutputQuery, aggregators);

}



TEST_F(ZkScalarAggregateTest, test_count_dummies) {
    // set up expected output
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
    std::string expectedOutputQuery = "SELECT COUNT(*)::BIGINT cnt_dummy FROM (" + query + ") subquery WHERE  NOT dummy";


    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));
    runDummiesTest(expectedOutputQuery, aggregators);

}





TEST_F(ZkScalarAggregateTest, test_avg) {
  std::string expectedOutputQuery = "SELECT FLOOR(AVG(l_linenumber))::INT avg_lineno FROM lineitem WHERE l_orderkey <= 10";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
  runTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkScalarAggregateTest, test_avg_dummies) {


  std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  std::string expectedOutputQuery = "SELECT FLOOR(AVG(l_linenumber))::INT avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
  runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkScalarAggregateTest, test_sum) {
  // set up expected outputs
  std::string expectedOutputQuery = "SELECT SUM(l_linenumber)::INTEGER sum_lineno FROM lineitem WHERE l_orderkey <= 10";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));
  runTest(expectedOutputQuery, aggregators);

}


TEST_F(ZkScalarAggregateTest, test_sum_dummies) {


  std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  std::string expectedOutputQuery = "SELECT SUM(l_linenumber)::INTEGER sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));

  runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkScalarAggregateTest, test_sum_baseprice_dummies) {


  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  std::string expectedOutputQuery = "SELECT SUM(l_extendedprice)::BIGINT sum_base_price FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(2, AggregateId::SUM, "sum_base_price"));

  runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkScalarAggregateTest, test_sum_lineno_baseprice) {


  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  std::string expectedOutputQuery = "SELECT SUM(l_linenumber)::INTEGER sum_lineno, SUM(l_extendedprice)::BIGINT sum_base_price FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_linemno"));
  aggregators.push_back(ScalarAggregateDefinition(2, AggregateId::SUM, "sum_base_price"));

  runDummiesTest(expectedOutputQuery, aggregators);
}

TEST_F(ZkScalarAggregateTest, test_min) {
  std::string expectedOutputQuery = "SELECT MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10";
  std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
   runTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkScalarAggregateTest, test_max) {
  std::string expectedOutputQuery = "SELECT MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10";
  std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
  runTest(expectedOutputQuery, aggregators);
}


TEST_F(ZkScalarAggregateTest, test_tpch_q1_sums) {

    // was l_orderkey <= 194
  string input_tuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100";
  string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, "
                      "l_extendedprice * (1.0 - l_discount) AS disc_price, "
                      "l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (" + input_tuples + ") selection";


  string expected_output_query = "SELECT SUM(l_quantity)::BIGINT sum_qty, "
                               "SUM(l_extendedprice)::BIGINT sum_base_price, "
                               "SUM(disc_price) sum_disc_price, "
                               "SUM(charge) sum_charge "
                               "FROM (" + sql + ") subquery WHERE NOT dummy ";

  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_output_query, false);

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                      ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                      ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                      ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};


  auto input = new ZkSqlInput(db_name_, sql, true);
  ScalarAggregate aggregate(input, aggregators);

  PlainTable *observed = aggregate.run()->reveal(PUBLIC);


  ASSERT_EQ(*expected, *observed);
    delete expected;
    delete observed;

}




TEST_F(ZkScalarAggregateTest, test_tpch_q1_avg_cnt) {

  string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194) selection";

  string expected_output_query =  "select \n"
                                "  FLOOR(avg(l_quantity))::BIGINT as avg_qty, \n"
                                "  FLOOR(avg(l_extendedprice))::BIGINT  as avg_price, \n"
                                "  FLOOR(avg(l_discount))::BIGINT  as avg_disc, \n"
                                "  count(*)::BIGINT as count_order \n"
                                "from (" + sql + ") subq\n"
                                                        " WHERE NOT dummy\n";

  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_output_query, false);

  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
      ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
      ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    auto input = new ZkSqlInput(db_name_, sql, true);


    // sort alice + bob inputs after union
  ScalarAggregate aggregate(input, aggregators);

  PlainTable *observed = aggregate.run()->reveal(PUBLIC);


  ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;

}

TEST_F(ZkScalarAggregateTest, tpch_q1) {

  string input_tuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100";
  string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (" + input_tuples + ") selection \n";

  string expected_output_query = "select \n"
                                "  sum(l_quantity)::BIGINT as sum_qty, \n"
                                "  sum(l_extendedprice)::BIGINT as sum_base_price, \n"
                                "  sum(l_extendedprice * (1 - l_discount))::BIGINT as sum_disc_price, \n"
                                "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax))::BIGINT as sum_charge, \n"
                                "  FLOOR(avg(l_quantity))::BIGINT as avg_qty, \n"
                                "  FLOOR(avg(l_extendedprice))::BIGINT as avg_price, \n"
                                "  FLOOR(avg(l_discount))::BIGINT as avg_disc, \n"
                                "  count(*)::BIGINT as count_order \n"
                                "from (" + input_tuples + ") input "
                                                         " where  l_shipdate <= date '1998-08-03'";


PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_output_query, false);

  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
      ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
      ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
      ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
      ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
      ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    auto input = new ZkSqlInput(db_name_, sql, true);

    ScalarAggregate aggregate(input, aggregators);


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

