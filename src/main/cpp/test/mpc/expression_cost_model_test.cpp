#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/secure_sql_input.h>
#include <test/mpc/emp_base_test.h>
#include "expression/generic_expression.h"
#include "expression/comparator_expression_nodes.h"
#include "opt/expression_cost_model.h"

using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54325, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65454, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class ExpressionCostModelTest : public EmpBaseTest {};


TEST_F(ExpressionCostModelTest, test_equality_int32) {
    this->disableBitPacking();
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem   WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY (1), (2)";
    SecureSqlInput *input = new SecureSqlInput(db_name_, sql, false);

    // filtering for l_linenumber = 1
    InputReference<Bit> read_field(1, input->getOutputSchema());
    Field<Bit> one(FieldType::SECURE_INT, emp::Integer(32, 1));
    LiteralNode<Bit> constant_input(one);
    EqualNode equality_check((ExpressionNode<Bit> *) &read_field, (ExpressionNode<Bit> *) &constant_input);

    ExpressionCostModel<Bit> model(&equality_check, input->getOutputSchema());
    auto cost = model.cost();

    size_t start_gates = manager_->andGateCount();

    Integer i(32, 1, PUBLIC);
    Integer one_i(32, 1, PUBLIC);

    Bit res = (i == one_i);

    size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;
    ASSERT_EQ(cost, gate_cnt);

}



TEST_F(ExpressionCostModelTest, test_equality_literal_packed) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem   WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY (1), (2)";
    std::string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input WHERE l_linenumber = 1";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);


    SecureSqlInput *input = new SecureSqlInput(db_name_, sql, false, collation);

    // filtering for l_linenumber = 1
    InputReference<Bit> read_field(1, input->getOutputSchema());
    Field<Bit> one(FieldType::SECURE_INT, emp::Integer(3, 1));
    LiteralNode<Bit> constant_input(one);
    EqualNode equality_check((ExpressionNode<Bit> *) &read_field, (ExpressionNode<Bit> *) &constant_input);

    ExpressionCostModel<Bit> model(&equality_check, input->getOutputSchema());
    auto cost = model.cost();

    size_t start_gates = manager_->andGateCount();

    Integer i(3, 1, PUBLIC);
    Integer one_i(3, 1, PUBLIC);

    Bit res = (i == one_i);

    size_t end_gates = manager_->andGateCount();
    auto gate_cnt = end_gates - start_gates;
    ASSERT_EQ(cost, gate_cnt);


}






int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

