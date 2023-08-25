#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/sql_input.h>
#include <operators/secure_sql_input.h>
#include <test/mpc/emp_base_test.h>
#include "opt/operator_cost_model.h"

using namespace emp;
using namespace vaultdb;

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54325, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class OperatorCostModelTest : public EmpBaseTest {
protected:
    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY c_custkey";


    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                    "FROM orders \n"
                                    "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                    " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= " + std::to_string(FLAGS_cutoff) + ")  \n"
                                                                                                                                          " ORDER BY l_orderkey, revenue ";
};

TEST_F(OperatorCostModelTest, test_table_scan) {
	std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);

    SecureSqlInput input(db_name_, sql, false, collation);

	auto scanned = input.run();
	
    auto cost = OperatorCostModel::operatorCost(&input);
    auto gate_cnt = input.getGateCount();

	std::cout << "Predicted cost: " << cost << ", observed cost: " << gate_cnt << "\n";

	if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->setSortOrder(collation);

        PlainTable *revealed = scanned->reveal(emp::PUBLIC);
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;

    }

}

TEST_F(OperatorCostModelTest, test_filter) {
		std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem   WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY (1), (2)";
    std::string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input WHERE l_linenumber = 1";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);


    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
    expected->setSortOrder(collation);

    SecureSqlInput *input = new SecureSqlInput(db_name_, sql, false, collation);

    // expression setup
    // filtering for l_linenumber = 1

    PackedInputReference<emp::Bit> read_field(1, input->getOutputSchema());
    Field<emp::Bit> one(FieldType::SECURE_INT, emp::Integer(4, 0));
    LiteralNode<emp::Bit> constant_input(one);
    EqualNode<emp::Bit> equality_check((ExpressionNode<emp::Bit> *) &read_field, (ExpressionNode<emp::Bit> *) &constant_input);
    Expression<emp::Bit> *expression = new GenericExpression<emp::Bit>(&equality_check, "predicate", FieldType::SECURE_BOOL);
	std::cout << input->getOutputSchema() << "\n";

    Filter<emp::Bit> filter(input, expression);

	size_t start_gates = manager_->andGateCount();
	auto filtered = filter.run();
	
    auto cost = OperatorCostModel::operatorCost(&filter);

	size_t end_gates = manager_->andGateCount();
    auto gate_cnt = filter.getGateCount();

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";
    
    if(FLAGS_validation) {
        PlainTable *revealed = filtered->reveal(emp::PUBLIC);
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;
    }
	//std::cout << SystemConfiguration::getInstance().bitPackingEnabled() << "\n";
}

TEST_F(OperatorCostModelTest, test_sort) { //from tpchQ01Sort

	string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_results_sql = "WITH input AS ("
                                  + sql
                                  + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";


    SortDefinition sort_def{
            ColumnSort(0, SortDirection::ASCENDING),
            ColumnSort(1, SortDirection::ASCENDING)
    };

    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sort_def);

	size_t start_gates = manager_->andGateCount();
    auto sorted = sort.run();

	auto cost = OperatorCostModel::operatorCost(&sort);

	size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";

    if(FLAGS_validation) {
        auto observed = sorted->reveal();
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_results_sql, false);
        expected->setSortOrder(sort_def);

        ASSERT_EQ(*expected, *observed);

        delete expected;
        delete observed;

    }	
}

TEST_F(OperatorCostModelTest, test_basic_join) { //from test_tpch_q3_customer_orders


        std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                             "orders_cte AS (" + orders_sql_ + ") "
                                                                                                             "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                                                                                             "FROM customer_cte, orders_cte "
                                                                                                             "WHERE NOT c_dummy AND NOT o_dummy AND c_custkey = o_custkey "
                                                                                                             "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql,false);

    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true);
    auto orders_input= new SecureSqlInput(db_name_, orders_sql_, true);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<emp::Bit> *predicate = FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                     customer_input, 4);

    auto join = new BasicJoin(orders_input, customer_input, predicate);

	size_t start_gates = manager_->andGateCount();

    auto join_res = join->run();

	auto cost = OperatorCostModel::operatorCost(join);

	size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";
    if(FLAGS_validation) {

        SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join->getOutputSchema().getFieldCount());
        Sort sort(join_res->reveal(), sort_def);

        PlainTable *observed = sort.run();
        expected->setSortOrder(sort_def);

        ASSERT_EQ(*expected, *observed);
        delete expected;
    }

    delete join;

}

TEST_F(OperatorCostModelTest, test_keyed_join) {
    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                     "orders_cte AS (" + orders_sql_ + ") "
                                "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                "WHERE NOT o_dummy AND NOT c_dummy "
                                "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    SortDefinition  customer_sort = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition  orders_sort = DataUtilities::getDefaultSortDefinition(4);
    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true, customer_sort);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true, orders_sort);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                                                       customer_input, 4);


    KeyedJoin join(orders_input, customer_input, predicate);

	size_t start_gates = manager_->andGateCount();

    auto joined = join.run();

	auto cost = OperatorCostModel::operatorCost(&join);

	size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";

    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql,  false);
        PlainTable *observed = joined->reveal();
        expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);
        delete expected;
        delete observed;


    }

}

TEST_F(OperatorCostModelTest, test_sort_merge_join) {
	this->disableBitPacking();

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                          "orders_cte AS (" + orders_sql_ + ") "
                                                                                                            "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                                                                                            "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                            "WHERE NOT o_dummy AND NOT c_dummy "
                                                                                                            "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";



    auto customer_input = new SecureSqlInput(db_name_, customer_sql_, true);
    auto orders_input = new SecureSqlInput(db_name_, orders_sql_, true);


    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    GenericExpression<emp::Bit> *predicate = (GenericExpression<Bit> *) FieldUtilities::getEqualityPredicate<emp::Bit>(orders_input, 1,
                                                                                                                       customer_input, 4);

    SortMergeJoin join(orders_input, customer_input, predicate);
    // Join output schema: (#0 encrypted-int32(13) orders.o_orderkey, #1 encrypted-int32(8) orders.o_custkey, #2 encrypted-int64(28) orders.o_orderdate, #3 encrypted-int32(1) orders.o_shippriority, #4 encrypted-int32(8) customer.c_custkey)

	size_t start_gates = manager_->andGateCount();
	
    auto joined = join.run();

	
	auto cost = OperatorCostModel::operatorCost(&join);

	size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";

    if(FLAGS_validation) {
        PlainTable *observed = joined->reveal();
        Sort sorter(observed, DataUtilities::getDefaultSortDefinition(5));
        observed = sorter.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);
        delete expected;
    }

}

TEST_F(OperatorCostModelTest, test_scalar_aggregate) { //from test_count
    // set up expected output
    std::string expected_sql = "SELECT COUNT(*)::BIGINT cnt FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff);

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";



	// provide the aggregator with inputs:
	auto input = new SecureSqlInput(db_name_, query, false);
	ScalarAggregate aggregate(input, aggregators);

	size_t start_gates = manager_->andGateCount();

	auto aggregated = aggregate.run();

	auto cost = OperatorCostModel::operatorCost(&aggregate);

	size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";

	if(FLAGS_validation) {
		PlainTable *observed = aggregated->reveal();
		PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
		ASSERT_EQ(*expected, *observed);

		delete observed;
		delete expected;

	}

}

TEST_F(OperatorCostModelTest, test_group_by_aggregate) {
    // set up expected output
    std::string expected_sql = "SELECT l_orderkey, COUNT(*) cnt FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) +" GROUP BY l_orderkey ORDER BY (1)";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";


    // run secure query
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1); // actually 2
    auto input = new SecureSqlInput(db_name_, query, false, sort_def);


    std::vector<int32_t> group_bys{0};
    SortMergeAggregate aggregate(input, group_bys, aggregators);

	size_t start_gates = manager_->andGateCount();

    auto aggregated = aggregate.run();

	auto cost = OperatorCostModel::operatorCost(&aggregate);

    size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";

    if(FLAGS_validation) {
        // set up expected output
        const PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_sql, false, 1);

        PlainTable *revealed = aggregated->reveal();
        ASSERT_EQ(*expected, *revealed);

        delete expected;
        delete revealed;

    }


}

TEST_F(OperatorCostModelTest, test_nested_loop_aggregate) {
    // set up expected output
    std::string expected_sql = "SELECT l_orderkey, COUNT(*) cnt FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " GROUP BY l_orderkey ORDER BY (1) ";

    std::vector<ScalarAggregateDefinition> aggregators{ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt")};
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY (1), (2)";


    // run secure query
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1); // actually 2
    auto input = new SecureSqlInput(db_name_, query, false, sort_def);


    std::vector<int32_t> group_bys{0};
    NestedLoopAggregate aggregate(input, group_bys, aggregators, SortDefinition(), FLAGS_cutoff);

	size_t start_gates = manager_->andGateCount();

    auto aggregated= aggregate.run();

	auto cost = OperatorCostModel::operatorCost(&aggregate);

    size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";

    if(FLAGS_validation) {
        Sort sort(aggregated->reveal(), SortDefinition{ColumnSort {0, SortDirection::ASCENDING}});
        auto observed = sort.run();
        PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_sql, false, 1);

        ASSERT_EQ(*expected, *observed);
        delete expected;
    }


}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
