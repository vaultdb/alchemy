#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/nested_loop_aggregate.h>
#include <test/mpc/emp_base_test.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>

using namespace emp;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43440, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65462, "port for managing EMP control flow by passing public values");


class SecureNestedLoopAggregateTest : public EmpBaseTest {

protected:
    void runTest(const string & expectedResultsQuery, const vector<ScalarAggregateDefinition> & aggregators) const;
    void runDummiesTest(const string & expected_sql, const vector<ScalarAggregateDefinition> & aggregators) const;

};

void SecureNestedLoopAggregateTest::runTest(const string &expected_sql,
                                         const vector<ScalarAggregateDefinition> & aggregators) const {

    // produces 25 rows
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    // set up expected output
    PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 1);

    // run secure query
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1); // actually 2
    auto input = new SecureSqlInput(db_name_, query, false, sort_def);


    std::vector<int32_t> group_bys{0};
    NestedLoopAggregate aggregate(input, group_bys, aggregators, 10);

    PlainTable *aggregated = aggregate.run()->reveal();

    Sort sort(aggregated, SortDefinition{ColumnSort {0, SortDirection::ASCENDING}});
    auto observed = sort.run();


    ASSERT_EQ(*expected, *observed);
    delete expected;



}

void SecureNestedLoopAggregateTest::runDummiesTest(const string &expected_sql,
                                                const vector<ScalarAggregateDefinition> &aggregators) const {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 1);

    // configure and run test
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SecureSqlInput(db_name_, query, true, sort_def);


    std::vector<int32_t> group_bys;
    group_bys.push_back(0);

    NestedLoopAggregate aggregate(input, group_bys, aggregators, 10);

    PlainTable *aggregated = aggregate.run()->reveal();

    Sort sort(aggregated, SortDefinition{ColumnSort {0, SortDirection::ASCENDING}});
    auto observed = sort.run();


        ASSERT_EQ(*expected, *observed);

        delete expected;

}


TEST_F(SecureNestedLoopAggregateTest, test_count) {
    // set up expected output
    std::string expected_sql = "SELECT l_orderkey, COUNT(*) cnt FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1) ";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expected_sql, aggregators);

}

TEST_F(SecureNestedLoopAggregateTest, test_count_no_bit_packing) {
    this->disableBitPacking();
    std::string expected_sql = "SELECT l_orderkey, COUNT(*) cnt FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runTest(expected_sql, aggregators);

}

TEST_F(SecureNestedLoopAggregateTest, test_count_dummies) {

    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey,COUNT(*) cnt  FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    runDummiesTest(expected_sql, aggregators);
}


TEST_F(SecureNestedLoopAggregateTest, test_sum) {
    // set up expected outputs
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INTEGER sum_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));
    runTest(expected_sql, aggregators);

}

TEST_F(SecureNestedLoopAggregateTest, test_sum_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, SUM(l_linenumber)::INTEGER sum_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::SUM, "sum_lineno"));

    runDummiesTest(expected_sql, aggregators);
}


TEST_F(SecureNestedLoopAggregateTest, test_avg) {
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runTest(expected_sql, aggregators);
}

TEST_F(SecureNestedLoopAggregateTest, test_avg_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, FLOOR(AVG(l_linenumber))::INTEGER avg_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(1, AggregateId::AVG, "avg_lineno"));
    runDummiesTest(expected_sql, aggregators);
}


TEST_F(SecureNestedLoopAggregateTest, test_min) {
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(SecureNestedLoopAggregateTest, test_min_no_bit_packing) {
    this->disableBitPacking();
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(SecureNestedLoopAggregateTest, test_min_dummies) {


    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MIN(l_linenumber) min_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MIN, "min_lineno")};
    runDummiesTest(expected_sql, aggregators);
}



TEST_F(SecureNestedLoopAggregateTest, test_max) {
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runTest(expected_sql, aggregators);
}

TEST_F(SecureNestedLoopAggregateTest, test_max_dummies) {
    std::string query = "SELECT l_orderkey, l_linenumber,  l_shipinstruct <> 'NONE' AS dummy  FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expected_sql = "SELECT l_orderkey, MAX(l_linenumber) max_lineno FROM (" + query + ") subquery WHERE  NOT dummy GROUP BY l_orderkey ORDER BY (1)";
    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(1, AggregateId::MAX, "max_lineno")};
    runDummiesTest(expected_sql, aggregators);
}

// // brings in about 200 tuples
TEST_F(SecureNestedLoopAggregateTest, test_tpch_q1_sums) {

    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100 ORDER BY l_orderkey, l_linenumber";
    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1.0 - l_discount) AS disc_price, l_extendedprice * (1.0 - l_discount) * (1.0 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                        " ORDER BY  l_returnflag, l_linestatus";


    string expected_sql = "SELECT  l_returnflag, l_linestatus, "
                                 "SUM(l_quantity) sum_qty, "
                                 "SUM(l_extendedprice) sum_base_price, "
                                 "SUM(disc_price) sum_disc_price, "
                                 "SUM(charge) sum_charge "
                                 "FROM (" + sql + ") subquery WHERE NOT dummy "
                                                         "GROUP BY l_returnflag, l_linestatus "
                                                         "ORDER BY l_returnflag, l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 0);

    std::vector<ScalarAggregateDefinition> aggregators {ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
                                                        ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
                                                        ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
                                                        ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge")};
    std::vector<int32_t> group_bys{0, 1};


    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SecureSqlInput(db_name_, sql, true, sort_def);


    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, 6);

    PlainTable *observed = aggregate->run()->reveal(PUBLIC);


    ASSERT_EQ(*expected, *observed);

    delete observed;
    delete expected;
    delete aggregate;



}

TEST_F(SecureNestedLoopAggregateTest, test_tpch_q1_avg_cnt) {

    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (SELECT * FROM lineitem WHERE l_orderkey <= 194 ORDER BY l_orderkey, l_linenumber) selection\n"
                        " ORDER BY l_returnflag, l_linestatus ";

    string expected_sql =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + sql + ") subq\n"
                                                          " where NOT dummy\n"
                                                          "group by \n"
                                                          "  l_returnflag, \n"
                                                          "  l_linestatus \n"
                                                          " \n"
                                                          "order by \n"
                                                          "  l_returnflag, l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 0);

    std::vector<int32_t> group_bys{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SecureSqlInput(db_name_, sql, true, sort_def);

    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, 6);

    PlainTable *observed = aggregate->run()->reveal(PUBLIC);


        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;
        delete aggregate;

}

TEST_F(SecureNestedLoopAggregateTest, tpch_q1) {

    // TODO: was <= 194, reduced owing to floating point drift
    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 100  ORDER BY l_orderkey, l_linenumber";
    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                                                  " ORDER BY l_returnflag, l_linestatus";

    string expected_sql =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  sum(l_quantity) as sum_qty, \n"
                                  "  sum(l_extendedprice) as sum_base_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*)::BIGINT as count_order \n"
                                  "from (" + inputTuples + ") input "
                                                           " where  l_shipdate <= date '1998-08-03'  "
                                                           "group by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus \n"
                                                           " \n"
                                                           "order by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus";

    PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_, expected_sql, false, 0);

    std::vector<int32_t> group_bys{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SecureSqlInput(db_name_, sql, true, sort_def);


    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, 6);

    PlainTable *observed = aggregate->run()->reveal(PUBLIC);

    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete observed;
    delete aggregate;



}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





