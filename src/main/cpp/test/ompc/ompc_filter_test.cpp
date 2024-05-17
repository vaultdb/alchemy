#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/secure_sql_input.h>
#include <test/mpc/emp_base_test.h>
#include <query_table/secure_tuple.h>
#include <expression/comparator_expression_nodes.h>
#include "expression/generic_expression.h"
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
DEFINE_int32(ctrl_port, 65454, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column, wire_packed or compressed)");


class OMPCFilterTest : public EmpBaseTest {};




TEST_F(OMPCFilterTest, ompc_test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY (1), (2)";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);


    SecureSqlInput input(db_name_, sql, false, collation);
    auto scanned = input.run();
    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = collation;

        PlainTable *revealed = scanned->revealInsecure(emp::PUBLIC);
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;

    }

}

// Testing for selecting l_linenumber=1

TEST_F(OMPCFilterTest, ompc_test_filter) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem   WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY (1), (2)";
    std::string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input WHERE l_linenumber = 1";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);


    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
    expected->order_by_ = collation;

    SecureSqlInput *input = new SecureSqlInput(db_name_, sql, false, collation);

    // expression setup
    // filtering for l_linenumber = 1
    PackedInputReference<emp::Bit> read_field(1, input->getOutputSchema());
    Field<emp::Bit> one(FieldType::SECURE_INT, emp::Integer(4, 0));
    LiteralNode<emp::Bit> constant_input(one);
    EqualNode<emp::Bit> equality_check((ExpressionNode<emp::Bit> *) &read_field, (ExpressionNode<emp::Bit> *) &constant_input);
    Expression<emp::Bit> *expression = new GenericExpression<emp::Bit>(&equality_check, "predicate", FieldType::SECURE_BOOL);


    Filter<emp::Bit> filter(input, expression);
    auto fiiltered = filter.run();

    if(FLAGS_validation) {
        PlainTable *revealed = fiiltered->revealInsecure(emp::PUBLIC);
        DataUtilities::removeDummies(revealed);
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;
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

