#include <gtest/gtest.h>
#include <stdexcept>
#include <gflags/gflags.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <test/mpc/emp_base_test.h>
#include <operators/keyed_sort_merge_join.h>
#include <operators/basic_join.h>  // Add this for NLJ
#include "util/field_utilities.h"
#include "opt/operator_cost_model.h"
#include "util/logger.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43455, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for generator");
DEFINE_string(unioned_db, "tpch_unioned_5000", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_5000_join_cost", "alice db name");
DEFINE_string(bob_db, "tpch_bob_5000_join_cost", "bob db name");
DEFINE_int32(ctrl_port, 65470, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");

using namespace vaultdb;
using namespace Logging;

class SecureJoinAggCostComparisonTest : public EmpBaseTest {
protected:
    int orders_cutoff = 10;
    int lineitem_cutoff = 10;

};

void execute_sort_merge_join(std::string db_name_, int orders_cutoff, int lineitem_cutoff){
    string orders_rows = "SELECT * FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(orders_cutoff);
    string lineitem_rows = "SELECT * FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(lineitem_cutoff);

    std::string orders_sql_ = "SELECT o_orderkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' AS o_dummy \n"
                              "FROM (" + orders_rows + ") selection \n"
                                                       "ORDER BY o_orderkey";

    std::string lineitem_sql_ = "SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue, l_shipdate <= date '1995-03-25' AS l_dummy \n"
                                "FROM (" + lineitem_rows + ") selection \n"
                                                           "ORDER BY l_orderkey";

    SortDefinition orders_sort_{ColumnSort(0, SortDirection::ASCENDING)};
    SortDefinition lineitem_sort_{ColumnSort(0, SortDirection::ASCENDING)};

    string unioned_orders_rows = "SELECT * FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(orders_cutoff*2);
    string unioned_lineitem_rows = "SELECT * FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(lineitem_cutoff*2);


    std::string expected_orders_sql_ = "SELECT o_orderkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' AS o_dummy \n"
                                       "FROM (" + unioned_orders_rows + ") selection \n"
                                                                        "ORDER BY o_orderkey";

    std::string expected_lineitem_sql_ = "SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue, l_shipdate <= date '1995-03-25' AS l_dummy \n"
                                         "FROM (" + unioned_lineitem_rows + ") selection \n"
                                                                            "ORDER BY l_orderkey";


    std::string expected_sql = "WITH orders_cte AS (" + expected_orders_sql_ + "), "
                                                                               "lineitem_cte AS (" + expected_lineitem_sql_ + ") "
                                                                                                                              "SELECT o_orderkey, o_orderdate, o_shippriority, l_orderkey, revenue "
                                                                                                                              "FROM orders_cte JOIN lineitem_cte ON o_orderkey = l_orderkey "
                                                                                                                              "WHERE NOT o_dummy AND NOT l_dummy "
                                                                                                                              "ORDER BY o_orderkey, revenue";

    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, orders_sort_);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);

    GenericExpression<emp::Bit>* predicate = (GenericExpression<Bit>*)FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 0, lineitem_input, 3);

    auto smj = new KeyedSortMergeJoin(orders_input, lineitem_input, 1, predicate);

    cout << "left child : " + std::to_string(smj->getChild(0)->getOutputCardinality()) << " , right child : " << std::to_string(smj->getChild(1)->getOutputCardinality()) << endl;
    auto smj_cost = OperatorCostModel::operatorCost((SecureOperator *) smj);
    cout << "SMJ Cost " << smj_cost << endl;

    auto expected_sort = SortDefinition{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING)};
    auto sort = new Sort(smj, expected_sort);

    auto joined = sort->run();

    Logger* log = get_log();
    log->write("Performing Sort Merge Join", Level::INFO);
    log->write("Observed gate count: " + std::to_string(smj->getGateCount()), Level::INFO);
    log->write("Runtime: " + std::to_string(smj->getRuntimeMs()), Level::INFO);

    if (FLAGS_validation) {
        SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING),};
        PlainTable* observed = joined->reveal();

        PlainTable* expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;
    }
}

void execute_nested_loop_join(std::string db_name_, int orders_cutoff, int lineitem_cutoff){

    string orders_rows = "SELECT * FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(orders_cutoff);
    string lineitem_rows = "SELECT * FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(lineitem_cutoff);

    std::string orders_sql_ = "SELECT o_orderkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' AS o_dummy \n"
                              "FROM (" + orders_rows + ") selection \n"
                                                       "ORDER BY o_orderkey";

    std::string lineitem_sql_ = "SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue, l_shipdate <= date '1995-03-25' AS l_dummy \n"
                                "FROM (" + lineitem_rows + ") selection \n"
                                                           "ORDER BY l_orderkey";

    SortDefinition orders_sort_{ColumnSort(0, SortDirection::ASCENDING)};
    SortDefinition lineitem_sort_{ColumnSort(0, SortDirection::ASCENDING)};

    string unioned_orders_rows = "SELECT * FROM orders ORDER BY o_orderkey LIMIT " + std::to_string(orders_cutoff*2);
    string unioned_lineitem_rows = "SELECT * FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(lineitem_cutoff*2);


    std::string expected_orders_sql_ = "SELECT o_orderkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' AS o_dummy \n"
                                       "FROM (" + unioned_orders_rows + ") selection \n"
                                                                        "ORDER BY o_orderkey";

    std::string expected_lineitem_sql_ = "SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue, l_shipdate <= date '1995-03-25' AS l_dummy \n"
                                "FROM (" + unioned_lineitem_rows + ") selection \n"
                                                           "ORDER BY l_orderkey";


    std::string expected_sql = "WITH orders_cte AS (" + expected_orders_sql_ + "), "
                                                                      "lineitem_cte AS (" + expected_lineitem_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_orderdate, o_shippriority, l_orderkey, revenue "
                                                                                                            "FROM orders_cte JOIN lineitem_cte ON o_orderkey = l_orderkey "
                                                                                                                     "WHERE NOT o_dummy AND NOT l_dummy "
                                                                                                            "ORDER BY o_orderkey, revenue";

    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, orders_sort_);
    auto lineitem_input = new SecureSqlInput(db_name_, lineitem_sql_, true, lineitem_sort_);

    GenericExpression<emp::Bit>* predicate = (GenericExpression<Bit>*)FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 0, lineitem_input, 3);

    auto nlj = new KeyedJoin(orders_input, lineitem_input, predicate);
    nlj->setForeignKeyInput(1);
    cout << "left child : " + std::to_string(nlj->getChild(0)->getOutputCardinality()) << " , right child : " << std::to_string(nlj->getChild(1)->getOutputCardinality()) << endl;

    auto nlj_cost = OperatorCostModel::operatorCost((SecureOperator *) nlj);
    cout << "NLJ Cost " << nlj_cost << endl;

    auto expected_sort = SortDefinition{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING)};
    auto sort = new Sort(nlj, expected_sort);

    auto joined = sort->run();

    Logger* log = get_log();
    log->write("Performing Nested Loop Join", Level::INFO);
    log->write("Observed gate count: " + std::to_string(nlj->getGateCount()), Level::INFO);
    log->write("Runtime: " + std::to_string(nlj->getRuntimeMs()), Level::INFO);

    if (FLAGS_validation) {
        SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING),};
        PlainTable* observed = joined->reveal();

        PlainTable* expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;
    }

}


void execute_nested_loop_agg(std::string db_name_, int lineitem_cutoff, int output_card){

    string input_rows = "SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(lineitem_cutoff);

    string input_query = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                         " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                         " FROM (" + input_rows + ") selection \n"
                                                  " ORDER BY l_returnflag, l_linestatus";

    string unioned_lineitem_rows = "SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(lineitem_cutoff*2);


    std::string expected_lineitem_sql_ = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                                         " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                                         " FROM (" + unioned_lineitem_rows + ") selection \n"
                                                                            "ORDER BY l_orderkey";


    std::string expected_sql = "WITH lineitem_cte AS (" + expected_lineitem_sql_ + ") "
                                                                                   "SELECT l_returnflag, l_linestatus, SUM(l_quantity) as sum_qty, COUNT(*)::BIGINT as count_order "
                                                                                   "FROM lineitem_cte "
                                                                                   "WHERE NOT dummy "
                                                                                   "GROUP BY  l_returnflag, l_linestatus "
                                                                                   "ORDER BY l_returnflag, l_linestatus";

    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
//            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
//            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
//            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
//            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
//            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
//            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    auto input = new SecureSqlInput(db_name_, input_query, true);

    auto nla = new NestedLoopAggregate (input, groupByCols, aggregators, SortDefinition(), output_card);
    auto sort = new Sort(nla, SortDefinition{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::ASCENDING)});

    auto agg = sort->run();

    cout << "Input cardinality: " << input->getOutputCardinality() << ", output card: "<< output_card  << endl;

    Logger* log = get_log();
    log->write("Performing Nested Loop Agg", Level::INFO);
    log->write("Observed gate count: " + std::to_string(nla->getGateCount()), Level::INFO);
    log->write("Runtime: " + std::to_string(nla->getRuntimeMs()), Level::INFO);

    if (FLAGS_validation) {
        SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::ASCENDING)};
        PlainTable* observed = agg->reveal();

        PlainTable* expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;
    }

}

void execute_sort_merge_agg(std::string db_name_, int lineitem_cutoff, int output_card){

    string input_rows = "SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(lineitem_cutoff);

    string input_query = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                         " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                         " FROM (" + input_rows + ") selection \n"
                                                  " ORDER BY l_returnflag, l_linestatus";

    string unioned_lineitem_rows = "SELECT * FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(lineitem_cutoff*2);


    std::string expected_lineitem_sql_ = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                                         " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                                         " FROM (" + unioned_lineitem_rows + ") selection \n"
                                                                             "ORDER BY l_orderkey";


    std::string expected_sql = "WITH lineitem_cte AS (" + expected_lineitem_sql_ + ") "
                                                                                   "SELECT l_returnflag, l_linestatus, SUM(l_quantity) as sum_qty, COUNT(*)::BIGINT as count_order "
                                                                                   "FROM lineitem_cte "
                                                                                   "WHERE NOT dummy "
                                                                                   "GROUP BY  l_returnflag, l_linestatus "
                                                                                   "ORDER BY l_returnflag, l_linestatus";





    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
//            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
//            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
//            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
//            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
//            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
//            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};


    auto input = new SecureSqlInput(db_name_, input_query, true);

    auto sort = new Sort(input, SortDefinition{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::ASCENDING)});
    auto sma = new SortMergeAggregate(sort, groupByCols, aggregators);
    auto agg = sma->run();

    cout << "Input cardinality: " << input->getOutputCardinality() << ", output card: "<< output_card << endl;

    Logger* log = get_log();
    log->write("Performing Sort Merge Agg", Level::INFO);
    log->write("Observed gate count: " + std::to_string(sort->getGateCount() + sma->getGateCount()), Level::INFO);
    log->write("Runtime: " + std::to_string(sma->getRuntimeMs() + sort->getRuntimeMs()), Level::INFO);

    if (FLAGS_validation) {
        SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::ASCENDING)};
        PlainTable* observed = agg->reveal();

        PlainTable* expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->order_by_ = sort_def;

        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;
    }

}

// AGG TESTS

// INPUT CARD : 10
TEST_F(SecureJoinAggCostComparisonTest, test_10_2_sma) {
    execute_sort_merge_agg(db_name_, 5, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10_2_nla) {
    execute_nested_loop_agg(db_name_, 5, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10_4_sma) {
    execute_sort_merge_agg(db_name_, 5, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10_4_nla) {
    execute_nested_loop_agg(db_name_, 5, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10_8_sma) {
    execute_sort_merge_agg(db_name_, 5, 8);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10_8_nla) {
    execute_nested_loop_agg(db_name_, 5, 8);
}

// INPUT CARD : 100
TEST_F(SecureJoinAggCostComparisonTest, test_100_2_sma) {
    execute_sort_merge_agg(db_name_, 50, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100_2_nla) {
    execute_nested_loop_agg(db_name_, 50, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100_4_sma) {
    execute_sort_merge_agg(db_name_, 50, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100_4_nla) {
    execute_nested_loop_agg(db_name_, 50, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100_8_sma) {
    execute_sort_merge_agg(db_name_, 50, 8);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100_8_nla) {
    execute_nested_loop_agg(db_name_, 50, 8);
}

// INPUT CARD : 1000
TEST_F(SecureJoinAggCostComparisonTest, test_1000_2_sma) {
    execute_sort_merge_agg(db_name_, 500, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000_2_nla) {
    execute_nested_loop_agg(db_name_, 500, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000_4_sma) {
    execute_sort_merge_agg(db_name_, 500, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000_4_nla) {
    execute_nested_loop_agg(db_name_, 500, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000_8_sma) {
    execute_sort_merge_agg(db_name_, 500, 8);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000_8_nla) {
    execute_nested_loop_agg(db_name_, 500, 8);
}

// INPUT CARD : 10000
TEST_F(SecureJoinAggCostComparisonTest, test_10000_2_sma) {
    execute_sort_merge_agg(db_name_, 5000, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10000_2_nla) {
    execute_nested_loop_agg(db_name_, 5000, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10000_4_sma) {
    execute_sort_merge_agg(db_name_, 5000, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10000_4_nla) {
    execute_nested_loop_agg(db_name_, 5000, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10000_8_sma) {
    execute_sort_merge_agg(db_name_, 5000, 8);
}

TEST_F(SecureJoinAggCostComparisonTest, test_10000_8_nla) {
    execute_nested_loop_agg(db_name_, 5000, 8);
}

// INPUT CARD : 100000
TEST_F(SecureJoinAggCostComparisonTest, test_100000_2_sma) {
    execute_sort_merge_agg(db_name_, 50000, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100000_2_nla) {
    execute_nested_loop_agg(db_name_, 50000, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100000_4_sma) {
    execute_sort_merge_agg(db_name_, 50000, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100000_4_nla) {
    execute_nested_loop_agg(db_name_, 50000, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100000_8_sma) {
    execute_sort_merge_agg(db_name_, 50000, 8);
}

TEST_F(SecureJoinAggCostComparisonTest, test_100000_8_nla) {
    execute_nested_loop_agg(db_name_, 50000, 8);
}

// INPUT CARD : 1000000
TEST_F(SecureJoinAggCostComparisonTest, test_1000000_2_sma) {
    execute_sort_merge_agg(db_name_, 500000, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000000_2_nla) {
    execute_nested_loop_agg(db_name_, 500000, 2);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000000_4_sma) {
    execute_sort_merge_agg(db_name_, 500000, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000000_4_nla) {
    execute_nested_loop_agg(db_name_, 500000, 4);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000000_8_sma) {
    execute_sort_merge_agg(db_name_, 500000, 8);
}

TEST_F(SecureJoinAggCostComparisonTest, test_1000000_8_nla) {
    execute_nested_loop_agg(db_name_, 500000, 8);
}


/*
// JOIN TESTS
TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_100_1_sort_merge_join) {
    execute_sort_merge_join(db_name_, 50, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_100_1_nested_loop_join) {
    execute_nested_loop_join(db_name_, 50, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_50_1_sort_merge_join) {
    execute_sort_merge_join(db_name_, 100,5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_50_1_nested_loop_join) {
    execute_nested_loop_join(db_name_, 100, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_10_1_sort_merge_join) {
    execute_sort_merge_join(db_name_, 500, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_10_1_nested_loop_join) {
    execute_nested_loop_join(db_name_, 500, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_5_1_sort_merge_join) {
    execute_sort_merge_join(db_name_, 1000, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_5_1_nested_loop_join) {
    execute_nested_loop_join(db_name_, 1000, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_1_1_nested_loop_join) {
    execute_nested_loop_join(db_name_, 5000, 5000);
}

TEST_F(SecureJoinAggCostComparisonTest, test_tpch_q3_orders_lineitem_1_1_sort_merge_join) {
    execute_sort_merge_join(db_name_, 5000, 5000);
}
*/


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter) = FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;
}
