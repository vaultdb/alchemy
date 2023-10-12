#include "plain_base_test.h"
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/scalar_aggregate.h>

DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class ScalarAggregateTest : public PlainBaseTest { };


// should just count to 50
TEST_F(ScalarAggregateTest, test_count) {
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem ORDER BY (1)  LIMIT " + std::to_string(FLAGS_cutoff);
    std::string expected_sql = "SELECT COUNT(*)::BIGINT FROM (" + query + ") q";

    auto input = new SqlInput(db_name_, query, false);

    std::vector<ScalarAggregateDefinition> aggregators  = {ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};

    ScalarAggregate<bool> aggregate(input, aggregators);


    PlainTable *output = aggregate.run();
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);
    
    ASSERT_EQ(*expected, *output);

    delete expected;
    
}


TEST_F(ScalarAggregateTest, test_count_dummies) {
    std::string query = "SELECT l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem ORDER BY l_orderkey, l_linenumber  LIMIT " + std::to_string(FLAGS_cutoff);

    // set up the expected results:
    std::string expected_sql = "SELECT COUNT(*)::BIGINT cnt FROM (" + query + ") selection WHERE NOT dummy";
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    
    // provide the aggregator with inputs:
    auto input = new SqlInput(db_name_, query, true);

    // define the aggregate:
    std::vector<ScalarAggregateDefinition> aggregators = {ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};

    // place aggregate definition in an Operator
    ScalarAggregate aggregate(input, aggregators);

    // run it
    PlainTable *output = aggregate.run();
    
    ASSERT_EQ(*expected, *output);

    delete expected;

}


TEST_F(ScalarAggregateTest, test_min) {
    std::string query =  "SELECT l_quantity FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

    // set up the expected results:
    std::string expected_sql = "WITH input AS (" + query + ") SELECT MIN(l_quantity) min_quantity FROM input";
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    // provide the aggregator with inputs:
    auto input = new SqlInput(db_name_, query, false);

    // define the aggregate:
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(0, AggregateId::MIN, "min_quantity")};

    // place aggregate definition in an Operator:
    ScalarAggregate aggregate(input, aggregators);

    // run it:
    PlainTable *output = aggregate.run();
    ASSERT_EQ(*output, *expected);
    delete expected;
}


TEST_F(ScalarAggregateTest, test_max) {
    std::string query =  "SELECT l_tax FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

    // set up the expected results:
    std::string expected_sql = "WITH input AS (" + query + ") SELECT MAX(l_tax) max_tax FROM input";
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    // provide the aggregator with inputs:
    auto input = new SqlInput(db_name_, query, false);

    // define the aggregate:
    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::MAX, "max_tax"));

    // place aggregate definition in an Operator:
    ScalarAggregate aggregate(input, aggregators);
    PlainTable *output = aggregate.run();

    ASSERT_EQ(*expected, *output);
    delete expected;

}


TEST_F(ScalarAggregateTest, test_sum) {
  std::string query =  "SELECT l_quantity FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

  // set up the expected results:
  std::string expected_sql = "WITH input AS (" + query + ") SELECT SUM(l_quantity) sum_qty FROM input";
  PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

  // provide the aggregator with inputs:
  auto input = new SqlInput(db_name_, query, false);

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::SUM, "sum_qty"));

  // place aggregate definition in an Operator:
  ScalarAggregate aggregate(input, aggregators);

  // run it:
  PlainTable *output = aggregate.run();
  ASSERT_EQ(*expected, *output);
    delete expected;

}

TEST_F(ScalarAggregateTest, test_sum_dummies) {
  std::string query =  "SELECT l_extendedprice, l_shipinstruct <> 'NONE' AS dummy FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

  // set up the expected results:
  std::string expected_sql = "SELECT SUM(l_extendedprice) sum_base_price FROM (" + query + ") selection WHERE NOT dummy";
  PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

  // provide the aggregator with inputs:
  auto input = new SqlInput(db_name_, query, true);


  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::SUM, "sum_base_price"));

  ScalarAggregate aggregate(input, aggregators);
  PlainTable *output = aggregate.run();

  ASSERT_EQ(*expected, *output);
    delete expected;

}


TEST_F(ScalarAggregateTest, test_avg) {

    std::string query =  "SELECT l_linenumber FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY (1)";

  // set up the expected results:
    std::string expected_sql = "SELECT FLOOR(AVG(l_linenumber))::INT avg_lno  FROM (" + query + ") q";

  PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

  // provide the aggregator with inputs:
  auto input = new SqlInput(db_name_, query, false);

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg_price"));
  ScalarAggregate aggregate(input, aggregators);
  PlainTable *output = aggregate.run();

  ASSERT_EQ(*expected, *output);
    delete expected;

}


TEST_F(ScalarAggregateTest, test_avg_dummies) {
    std::string query = "SELECT l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";


  // set up the expected results:
  std::string expected_sql = "SELECT FLOOR(AVG(l_linenumber))::INTEGER avg_disc FROM (" + query + ") selection WHERE NOT dummy";
  PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

  auto input = new SqlInput(db_name_, query, false);
  std::vector<ScalarAggregateDefinition> aggregators;
  aggregators.push_back(ScalarAggregateDefinition(0, AggregateId::AVG, "avg_disc"));

  ScalarAggregate aggregate(input, aggregators);
  PlainTable *output = aggregate.run();

  ASSERT_EQ(*expected, *output);
    delete expected;

}


TEST_F(ScalarAggregateTest, test_all_sum_dummies) {
  std::string query =  "SELECT l_quantity, l_extendedprice, "
                      "l_extendedprice * (1 - l_discount) l_discprice, "
                      "l_extendedprice * (1 - l_discount) * (1 + l_tax) l_charge, "
                      "l_shipinstruct <> 'NONE' AS dummy "
                      "FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

  // Ordinals:
  // l_quantity, #0
  // l_extendedprice, #1
  // l_extendedprice * (1 - l_discount) AS disc_price, #2
  // l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge #3

  // set up the expected results:
  std::string expected_sql = "SELECT SUM(l_quantity) sum_qty, "
                                    "SUM(l_extendedprice) sum_base_price, "
                                    "SUM(l_discprice) sum_disc_price, "
                                    "SUM(l_charge) sum_charge "
                                    "FROM (" + query + ") selection WHERE NOT dummy";
  PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

  // provide the aggregator with inputs:
  auto input = new SqlInput(db_name_, query, true);

  // define the aggregate:
  std::vector<ScalarAggregateDefinition> aggregators = {
          ScalarAggregateDefinition(0, AggregateId::SUM, "sum_qty"),
          ScalarAggregateDefinition(1, AggregateId::SUM, "sum_base_price"),
          ScalarAggregateDefinition(2, AggregateId::SUM, "sum_disc_price"),
          ScalarAggregateDefinition(3, AggregateId::SUM, "sum_charge")
  };


  // place aggregate definition in an Operator:
 ScalarAggregate aggregate(input, aggregators);

  // run it:
  PlainTable *output = aggregate.run();

  ASSERT_EQ(*expected, *output);
    delete expected;

}


TEST_F(ScalarAggregateTest, test_all_avg_dummies) {
  std::string query =  "SELECT l_quantity, l_extendedprice, l_discount, "
                       "l_shipinstruct <> 'NONE' AS dummy "
                       "FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

  // set up the expected results:
  std::string expected_sql = "SELECT AVG(l_quantity) avg_qty, "
                                    "AVG(l_extendedprice) avg_price, "
                                    "AVG(l_discount) avg_disc "
                                    "FROM (" + query + ") selection WHERE NOT dummy";
  PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    std::vector<ScalarAggregateDefinition> aggregators  = {
          ScalarAggregateDefinition(0, AggregateId::AVG, "avg_qty"),
          ScalarAggregateDefinition(1, AggregateId::AVG, "avg_price"),
          ScalarAggregateDefinition(2, AggregateId::AVG, "avg_disc")};

    auto input = new SqlInput(db_name_, query, true);
    ScalarAggregate aggregate(input, aggregators);
    PlainTable *output = aggregate.run();

  ASSERT_EQ(*expected, *output);
    delete expected;

}


TEST_F(ScalarAggregateTest, test_tpch_q1_avg_cnt) {

    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(FLAGS_cutoff) + ") selection";

    string expected_sql =  "select \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice)  as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + sql + ") subq\n"
                                                          " where NOT dummy";

    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    // provide the aggregator with inputs:
    auto input = new SqlInput(db_name_, sql, true);
    ScalarAggregate aggregate(input, aggregators);
    PlainTable *aggregated = aggregate.run();


    ASSERT_EQ(*expected, *aggregated);
    delete expected;

}


TEST_F(ScalarAggregateTest, test_all_aggs_tpch_q1) {
  std::string query =  "SELECT l_quantity, l_extendedprice, l_discount, "
                       "l_extendedprice * (1 - l_discount) l_discprice, "
                       "l_extendedprice * (1 - l_discount) * (1 + l_tax) l_charge, "
                       "l_shipinstruct <> 'NONE' AS dummy "
                       "FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

  // Ordinals:
  // l_quantity, #0
  // l_extendedprice, #1
  // l_discount #2
  // l_extendedprice * (1 - l_discount) AS disc_price, #3
  // l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge #4

  // set up the expected results:
  std::string expected_sql = "SELECT SUM(l_quantity) sum_qty, "
                                    "SUM(l_extendedprice) sum_base_price, "
                                    "SUM(l_discprice) sum_disc_price, "
                                    "SUM(l_charge) sum_charge, "
                                    "AVG(l_quantity) avg_qty, "
                                    "AVG(l_extendedprice) avg_price, "
                                    "AVG(l_discount) avg_disc, "
                                    "COUNT(*)::BIGINT count_order "
                                    "FROM (" + query + ") selection WHERE NOT dummy";
  PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);


  std::vector<ScalarAggregateDefinition> aggregators = {
    ScalarAggregateDefinition(0, AggregateId::SUM, "sum_qty"),
  ScalarAggregateDefinition(1, AggregateId::SUM, "sum_base_price"),
  ScalarAggregateDefinition(3, AggregateId::SUM, "sum_disc_price"),
  ScalarAggregateDefinition(4, AggregateId::SUM, "sum_charge"),
  ScalarAggregateDefinition(0, AggregateId::AVG, "avg_qty"),
  ScalarAggregateDefinition(1, AggregateId::AVG, "avg_price"),
  ScalarAggregateDefinition(2, AggregateId::AVG, "avg_disc"),
  ScalarAggregateDefinition(-1, AggregateId::COUNT, "count_order")};


    auto input = new SqlInput(db_name_, query, true);
  ScalarAggregate aggregate(input, aggregators);
  PlainTable *output = aggregate.run();

  ASSERT_EQ(*expected, *output);
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







