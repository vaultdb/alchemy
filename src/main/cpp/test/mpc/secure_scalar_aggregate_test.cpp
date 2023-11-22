#include <test/mpc/emp_base_test.h>
#include <util/data_utilities.h>
#include <operators/operator.h>
#include <operators/support/aggregate_id.h>
#include <operators/secure_sql_input.h>
#include <operators/scalar_aggregate.h>
#include <opt/operator_cost_model.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54312, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65466, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");


using namespace vaultdb;

class SecureScalarAggregateTest : public EmpBaseTest {

protected:
  void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;
  void runDummiesTest(const string & expected_sql, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void SecureScalarAggregateTest::runTest(const string &expected_sql,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

  // produces 25 rows @ cutoff 10
  std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";



  // provide the aggregator with inputs:
  auto input = new SecureSqlInput(db_name_, query, false);
  ScalarAggregate aggregate(input, aggregators);
  auto aggregated = aggregate.run();

  if(FLAGS_validation) {
      PlainTable *observed = aggregated->reveal();
      PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
      ASSERT_EQ(*expected, *observed);

      delete observed;
      delete expected;

  }

}


void SecureScalarAggregateTest::runDummiesTest(const string &expected_sql,
                                        const vector<ScalarAggregateDefinition> & aggregators) const {

  // produces 25 rows
  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);

  // provide the aggregator with inputs:
  auto input = new SecureSqlInput(db_name_, query, true);


  ScalarAggregate aggregate(input, aggregators);
  
  auto aggregated = aggregate.run();

    size_t observed_gates = aggregate.getGateCount();
    size_t estimated_gates = OperatorCostModel::operatorCost((SecureOperator *) &aggregate);
//    cout << "Input schema: " << input->getOutputSchema() << endl;
//    cout << "Estimated cost: " << estimated_gates << " observed gates: " << observed_gates << endl;

    if(FLAGS_validation) {
        PlainTable *observed = aggregated->reveal();
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;

    }

    

}


TEST_F(SecureScalarAggregateTest, test_count) {
    // set up expected output
    std::string expected_sql = "SELECT COUNT(*)::BIGINT cnt FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expected_sql, aggregators);

}



TEST_F(SecureScalarAggregateTest, test_count_dummies) {
    // set up expected output
    // count should be 8
    std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);
    std::string expected_sql = "SELECT COUNT(*)::BIGINT cnt_dummy FROM (" + query + ") subquery WHERE  NOT dummy";


    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runDummiesTest(expected_sql, aggregators);

}



TEST_F(SecureScalarAggregateTest, test_min) {
    std::string expected_sql = "SELECT MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(SecureScalarAggregateTest, test_min_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);
    std::string expected_sql = "SELECT MIN(l_linenumber) min_dummy FROM (" + query + ") subquery WHERE  NOT dummy";


    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min")};
    runDummiesTest(expected_sql, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_max) {
    std::string expected_sql = "SELECT MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runTest(expected_sql, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_max_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);
    std::string expected_sql = "SELECT MAX(l_linenumber) min_dummy FROM (" + query + ") subquery WHERE  NOT dummy";


    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "maxus ")};
    runDummiesTest(expected_sql, aggregators);
}



TEST_F(SecureScalarAggregateTest, test_sum) {
    // set up expected outputs
    std::string expected_sql = "SELECT SUM(l_linenumber)::INTEGER sum_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};
    runTest(expected_sql, aggregators);

}


TEST_F(SecureScalarAggregateTest, test_sum_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);
    std::string expected_sql = "SELECT SUM(l_linenumber)::INTEGER sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runDummiesTest(expected_sql, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_avg) {
  std::string expected_sql = "SELECT FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};
  runTest(expected_sql, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_avg_dummies) {


  std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);
  std::string expected_sql = "SELECT FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};
  runDummiesTest(expected_sql, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_sum_baseprice_dummies) {


  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);
  std::string expected_sql = "SELECT SUM(l_extendedprice) sum_base_price FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, AggregateId::SUM, "sum_base_price")};

  runDummiesTest(expected_sql, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_sum_lineno_baseprice) {


  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=" + std::to_string(FLAGS_cutoff);
  std::string expected_sql = "SELECT SUM(l_linenumber)::INTEGER sum_lineno, SUM(l_extendedprice) sum_base_price FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(1, AggregateId::SUM, "sum_linemno"),
      ScalarAggregateDefinition(2, AggregateId::SUM, "sum_base_price")};

  runDummiesTest(expected_sql, aggregators);
}



TEST_F(SecureScalarAggregateTest, test_tpch_q1_sums) {

    // was l_orderkey <= 194
  string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100";
  string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, "
                      "l_extendedprice * (1.0 - l_discount) AS disc_price, "
                      "l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (" + inputTuples + ") selection";


  string expected_sql = "SELECT SUM(l_quantity) sum_qty, "
                               "SUM(l_extendedprice) sum_base_price, "
                               "SUM(disc_price) sum_disc_price, "
                               "SUM(charge) sum_charge "
                               "FROM (" + inputQuery + ") subquery WHERE NOT dummy ";
  
  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                      ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                      ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                      ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};


  auto input = new SecureSqlInput(db_name_, inputQuery, true);


  ScalarAggregate aggregate(input, aggregators);

  auto aggregated = aggregate.run();

    size_t observed_gates = aggregate.getGateCount();
    size_t estimated_gates = OperatorCostModel::operatorCost((SecureOperator *) &aggregate);
//    cout << "Input schema: " << input->getOutputSchema() << endl;
//    cout << "Estimated cost: " << estimated_gates << " observed gates: " << observed_gates << endl;

  if(FLAGS_validation) {
      auto observed = aggregated->reveal();
      PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);

      ASSERT_EQ(*expected, *observed);
      delete observed;
      delete expected;
  }
  
}




TEST_F(SecureScalarAggregateTest, test_tpch_q1_avg_cnt) {

  string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194) selection";

  string expected_sql =  "select \n"
                                "  avg(l_quantity) as avg_qty, \n"
                                "  avg(l_extendedprice) as avg_price, \n"
                                "  avg(l_discount) as avg_disc, \n"
                                "  count(*)::BIGINT as count_order \n"
                                "from (" + sql + ") subq\n"
                                                        " where NOT dummy\n";

  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
      ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
      ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

  auto input = new SecureSqlInput(db_name_, sql, true);

  // sort alice + bob inputs after union
  ScalarAggregate aggregate(input, aggregators);
   auto aggregated = aggregate.run();

    size_t observed_gates = aggregate.getGateCount();
    size_t estimated_gates = OperatorCostModel::operatorCost((SecureOperator *) &aggregate);
//    cout << "Input schema: " << input->getOutputSchema() << endl;
//    cout << "Estimated cost: " << estimated_gates << " observed gates: " << observed_gates << endl;
    if(FLAGS_validation) {
        auto observed = aggregated->reveal();
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);

        ASSERT_EQ(*expected, *observed);
        delete observed;
        delete expected;
    }

}

TEST_F(SecureScalarAggregateTest, tpch_q1) {

  string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100";
  string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (" + inputTuples + ") selection \n";

  string expected_sql =  "select \n"
                                "  sum(l_quantity) as sum_qty, \n"
                                "  sum(l_extendedprice) as sum_base_price, \n"
                                "  sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \n"
                                "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \n"
                                "  avg(l_quantity) as avg_qty, \n"
                                "  avg(l_extendedprice) as avg_price, \n"
                                "  avg(l_discount) as avg_disc, \n"
                                "  count(*)::BIGINT as count_order \n"
                                "from (" + inputTuples + ") input "
                                                         " where  l_shipdate <= date '1998-08-03'";


  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
      ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
      ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
      ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
      ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
      ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    auto input = new SecureSqlInput(db_name_, inputQuery, true);



    ScalarAggregate aggregate(input, aggregators);

    auto aggregated = aggregate.run();

    size_t observed_gates = aggregate.getGateCount();
    size_t estimated_gates = OperatorCostModel::operatorCost((SecureOperator *) &aggregate);
//    cout << "Input schema: " << input->getOutputSchema() << endl;
//    cout << "Estimated cost: " << estimated_gates << " observed gates: " << observed_gates << endl;
    if(FLAGS_validation) {
        auto observed = aggregated->reveal();
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);

        ASSERT_EQ(*expected, *observed);
        delete observed;
        delete expected;
    }


}





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

