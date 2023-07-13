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
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65455, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class OperatorCostModelTest : public EmpBaseTest {};

TEST_F(OperatorCostModelTest, test_table_scan) {
	std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem WHERE l_orderkey <= 100  ORDER BY (1), (2)";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);

    SecureSqlInput input(db_name_, sql, false, collation); 	

	size_t start_gates = manager_->andGateCount();
	auto scanned = input.run();
	
    auto cost = OperatorCostModel::operatorCost(&input);

	size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;

	std::cout << "Predicted cost: " << cost << "\n";
	std::cout << "Observed cost: " << gate_cnt << "\n";

	if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, sql, false);
        expected->setSortOrder(collation);

        PlainTable *revealed = scanned->reveal(emp::PUBLIC);
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;

    }

}
