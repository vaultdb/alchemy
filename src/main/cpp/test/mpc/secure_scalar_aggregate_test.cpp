#include <test/mpc/emp_base_test.h>
#include <util/data_utilities.h>
#include <operators/operator.h>
#include <operators/support/aggregate_id.h>
#include <operators/secure_sql_input.h>
#include <operators/scalar_aggregate.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54312, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");


using namespace vaultdb;

class SecureScalarAggregateTest : public EmpBaseTest {

protected:
  void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;
  void runDummiesTest(const string & expectedOutputQuery, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void SecureScalarAggregateTest::runTest(const string &expectedOutputQuery,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

  // produces 25 rows
  std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";


  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expectedOutputQuery, storage_model_, false);

  // provide the aggregator with inputs:
  auto input = new SecureSqlInput(db_name_, query, false, storage_model_, netio_, FLAGS_party);
  ScalarAggregate aggregate(input, aggregators);
  PlainTable *observed = aggregate.run()->reveal();


  ASSERT_EQ(*expected, *observed);

  delete observed;
  delete expected;

}


void SecureScalarAggregateTest::runDummiesTest(const string &expectedOutputQuery,
                                        const vector<ScalarAggregateDefinition> & aggregators) const {

  // produces 25 rows
  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expectedOutputQuery, storage_model_, false);

  // provide the aggregator with inputs:
  auto input = new SecureSqlInput(db_name_, query, true, storage_model_, netio_, FLAGS_party);


  ScalarAggregate aggregate(input, aggregators);

  PlainTable *aggregated = aggregate.run()->reveal();


  // need to delete dummies from observed output to compare it to expected
   DataUtilities::removeDummies(aggregated);


      ASSERT_EQ(*expected, *aggregated);

      delete expected;
      delete aggregated;


}


TEST_F(SecureScalarAggregateTest, test_count) {
    // set up expected output
    std::string expectedOutputQuery = "SELECT COUNT(*)::BIGINT cnt FROM lineitem WHERE l_orderkey <= 10";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expectedOutputQuery, aggregators);

}



TEST_F(SecureScalarAggregateTest, test_count_dummies) {
    // set up expected output
    // count should be 8
    std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
    std::string expectedOutputQuery = "SELECT COUNT(*)::BIGINT cnt_dummy FROM (" + query + ") subquery WHERE  NOT dummy";


    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runDummiesTest(expectedOutputQuery, aggregators);

}


TEST_F(SecureScalarAggregateTest, test_min) {
    std::string expectedOutputQuery = "SELECT MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expectedOutputQuery, aggregators);
}

TEST_F(SecureScalarAggregateTest, test_min_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
    std::string expectedOutputQuery = "SELECT MIN(l_linenumber) min_dummy FROM (" + query + ") subquery WHERE  NOT dummy";


    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min")};
    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_max) {
    std::string expectedOutputQuery = "SELECT MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_max_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
    std::string expectedOutputQuery = "SELECT MAX(l_linenumber) min_dummy FROM (" + query + ") subquery WHERE  NOT dummy";


    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "maxus ")};
    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_sum) {
    // set up expected outputs
    std::string expectedOutputQuery = "SELECT SUM(l_linenumber)::INTEGER sum_lineno FROM lineitem WHERE l_orderkey <= 10";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};
    runTest(expectedOutputQuery, aggregators);

}


TEST_F(SecureScalarAggregateTest, test_sum_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
    std::string expectedOutputQuery = "SELECT SUM(l_linenumber)::INTEGER sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno")};

    runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_avg) {
  std::string expectedOutputQuery = "SELECT FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= 10";

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};
  runTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_avg_dummies) {


  std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  std::string expectedOutputQuery = "SELECT FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno")};
  runDummiesTest(expectedOutputQuery, aggregators);
}




TEST_F(SecureScalarAggregateTest, test_sum_baseprice_dummies) {


  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  std::string expectedOutputQuery = "SELECT SUM(l_extendedprice) sum_base_price FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, AggregateId::SUM, "sum_base_price")};

  runDummiesTest(expectedOutputQuery, aggregators);
}


TEST_F(SecureScalarAggregateTest, test_sum_lineno_baseprice) {


  std::string query = "SELECT l_orderkey, l_linenumber, l_extendedprice, l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10";
  std::string expectedOutputQuery = "SELECT SUM(l_linenumber)::INTEGER sum_lineno, SUM(l_extendedprice) sum_base_price FROM (" + query + ") subquery WHERE  NOT dummy";

  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(1, AggregateId::SUM, "sum_linemno"),
      ScalarAggregateDefinition(2, AggregateId::SUM, "sum_base_price")};

  runDummiesTest(expectedOutputQuery, aggregators);
}



TEST_F(SecureScalarAggregateTest, test_tpch_q1_sums) {

    // was l_orderkey <= 194
  string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100";
  string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, "
                      "l_extendedprice * (1.0 - l_discount) AS disc_price, "
                      "l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (" + inputTuples + ") selection";


  string expectedOutputQuery = "SELECT SUM(l_quantity) sum_qty, "
                               "SUM(l_extendedprice) sum_base_price, "
                               "SUM(disc_price) sum_disc_price, "
                               "SUM(charge) sum_charge "
                               "FROM (" + inputQuery + ") subquery WHERE NOT dummy ";

  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expectedOutputQuery, storage_model_, false);

  std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                      ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                      ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                      ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};


  auto input = new SecureSqlInput(db_name_, inputQuery, true, storage_model_, netio_, FLAGS_party);


  ScalarAggregate aggregate(input, aggregators);

  PlainTable *aggregated = aggregate.run()->reveal(PUBLIC);

  // need to delete dummies from observed output to compare it to expected
  DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *aggregated);
    delete aggregated;
    delete expected;



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

  PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, storage_model_, false);

  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
      ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
      ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

  auto input = new SecureSqlInput(db_name_, sql, true, storage_model_, netio_, FLAGS_party);

  // sort alice + bob inputs after union
  ScalarAggregate aggregate(input, aggregators);

  PlainTable *aggregated = aggregate.run()->reveal(PUBLIC);

  // need to delete dummies from observed output to compare it to expected
  DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *aggregated);
    delete aggregated;
    delete expected;


}

TEST_F(SecureScalarAggregateTest, tpch_q1) {

  string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100";
  string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                      " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                      " FROM (" + inputTuples + ") selection \n";

  string expectedOutputQuery =  "select \n"
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

PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expectedOutputQuery, storage_model_, false);

  std::vector<ScalarAggregateDefinition> aggregators{
      ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
      ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
      ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
      ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
      ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
      ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
      ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    auto input = new SecureSqlInput(db_name_, inputQuery, true, storage_model_, netio_, FLAGS_party);



    ScalarAggregate aggregate(input, aggregators);


  PlainTable *aggregated = aggregate.run()->reveal(PUBLIC);

  // need to delete dummies from observed output to compare it to expected
  DataUtilities::removeDummies(aggregated);
  ASSERT_EQ(*expected, *aggregated);
  delete aggregated;
  delete expected;


}





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    return RUN_ALL_TESTS();
}

