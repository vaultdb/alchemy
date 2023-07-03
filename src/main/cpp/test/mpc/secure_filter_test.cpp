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

using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54325, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65454, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");


class SecureFilterTest : public EmpBaseTest {};




TEST_F(SecureFilterTest, test_table_scan) {

    std::string db_name_ =  FLAGS_party == emp::ALICE ? alice_db_ : bob_db_;

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem WHERE l_orderkey <= 100  ORDER BY (1), (2)";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);


    SecureSqlInput input(db_name_, sql, false, collation);
    auto scanned = input.run();
    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, sql, false);
        expected->setSortOrder(collation);

        PlainTable *revealed = scanned->reveal(emp::PUBLIC);
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;

    }

}




// Testing for selecting l_linenumber=1

TEST_F(SecureFilterTest, test_filter) {
    std::string db_name_ =  FLAGS_party == emp::ALICE ? alice_db_ : bob_db_;

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem   WHERE l_orderkey <= 100  ORDER BY (1), (2)";
    std::string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input WHERE l_linenumber = 1";
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(2);


    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);
    expected->setSortOrder(collation);

    SecureSqlInput *input = new SecureSqlInput(db_name_, sql, false, collation);

    // expression setup
    // filtering for l_linenumber = 1

    InputReference<emp::Bit> read_field(1, input->getOutputSchema());
    Field<emp::Bit> one(FieldType::SECURE_INT, emp::Integer(32, 1));
    LiteralNode<emp::Bit> constant_input(one);
    EqualNode<emp::Bit> equality_check((ExpressionNode<emp::Bit> *) &read_field, (ExpressionNode<emp::Bit> *) &constant_input);
    Expression<emp::Bit> *expression = new GenericExpression<emp::Bit>(&equality_check, "predicate", FieldType::SECURE_BOOL);


    Filter<emp::Bit> filter(input, expression);
    auto fiiltered = filter.run();
    if(FLAGS_validation) {
        PlainTable *revealed = fiiltered->reveal(emp::PUBLIC);
        ASSERT_EQ(*expected, *revealed);
        delete expected;
        delete revealed;
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

