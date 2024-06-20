#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/secure_sql_input.h>
#include <test/ompc/ompc_base_test.h>
#include <query_table/secure_tuple.h>
#include <expression/comparator_expression_nodes.h>
#include "expression/generic_expression.h"
#include "opt/operator_cost_model.h"
#include "operators/packed_table_scan.h"

#if __has_include("emp-rescu/emp-rescu.h")

using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54335, "port for EMP execution");
DEFINE_int32(ctrl_port, 65455, "port for managing EMP control flow by passing public values");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(cutoff, 1000, "limit clause for queries");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wires, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");


class OMPCFilterTest : public OmpcBaseTest {};




TEST_F(OMPCFilterTest, ompc_test_table_scan) {

    std::string limit_sql = "SELECT * FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(FLAGS_cutoff);
    std::string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY l_orderkey, l_linenumber";
    SortDefinition collation{ColumnSort(0, SortDirection::ASCENDING),
                            ColumnSort(3, SortDirection::ASCENDING)};

    Operator<Bit> *input;

    if(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE) {
        std::string src_path = Utilities::getCurrentWorkingDirectory();
        std::string packed_pages_path = src_path + "/packed_pages/";

        input = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path,
                                                                 FLAGS_party, FLAGS_cutoff);
    }
    else {
        input = new SecureSqlInput(db_name_, limit_sql, false);
    }

    input->setOperatorId(-2);

    SecureTable *scanned = input->run();

    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = collation;

        PlainTable *revealed = scanned->revealInsecure();
        DataUtilities::removeDummies(revealed);

        // Order by l_orderkey, l_linenumber
        Sort<bool> sort(revealed, collation);
        sort.setOperatorId(-2);
        revealed = sort.run();

        ASSERT_EQ(*expected, *revealed);

    }

}

// Testing for selecting l_linenumber=1

TEST_F(OMPCFilterTest, ompc_test_filter) {

    std::string limit_sql = "SELECT * FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(FLAGS_cutoff);
    std::string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input WHERE l_linenumber = 1 ORDER BY l_orderkey, l_linenumber";
    SortDefinition collation{ColumnSort(0, SortDirection::ASCENDING),
                             ColumnSort(3, SortDirection::ASCENDING)};

    Operator<Bit> *input;

    if(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE) {
        std::string src_path = Utilities::getCurrentWorkingDirectory();
        std::string packed_pages_path = src_path + "/packed_pages/";

        input = new PackedTableScan<Bit>(FLAGS_unioned_db, "lineitem", packed_pages_path,
                                                                 FLAGS_party, FLAGS_cutoff);
    }
    else {
        input = new SecureSqlInput(db_name_, limit_sql, false);
    }
    input->setOperatorId(-2);

    // filtering for l_linenumber = 1
    PackedInputReference<emp::Bit> read_linenumber_field(3, input->getOutputSchema());
    Field<emp::Bit> one(FieldType::SECURE_INT, emp::Integer(4, 0));
    LiteralNode<emp::Bit> constant_input(one);
    EqualNode<emp::Bit> equality_check((ExpressionNode<emp::Bit> *) &read_linenumber_field, (ExpressionNode<emp::Bit> *) &constant_input);
    Expression<emp::Bit> *expression = new GenericExpression<emp::Bit>(&equality_check, "predicate", FieldType::SECURE_BOOL);

    Filter<emp::Bit> *filter = new Filter(input, expression);
    filter->setOperatorId(-2);
    auto fiiltered = filter->run();

    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = collation;

        PlainTable *revealed = fiiltered->revealInsecure();
        DataUtilities::removeDummies(revealed);

        // Order by l_orderkey, l_linenumber
        Sort<bool> sort(revealed, collation);
        sort.setOperatorId(-2);
        revealed = sort.run();

        ASSERT_EQ(*expected, *revealed);
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


#endif
