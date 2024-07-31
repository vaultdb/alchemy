#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/filter.h>
#include <operators/stored_table_scan.h>
#include <test/ompc/ompc_base_test.h>
#include <expression/comparator_expression_nodes.h>
#include "expression/generic_expression.h"
#include "opt/operator_cost_model.h"

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
DEFINE_string(wire_path, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");
DEFINE_int32(unpacked_page_size_bits, 2048, "unpacked page size in bits");
DEFINE_int32(page_cnt, 50, "number of pages in buffer pool");

// depends on
// bash wire_pack_tpch_instance.sh tpch_unioned_150
// or
// bash ./bin/xor_secret_share_tpch_instance tpch_unioned_150 shares/tpch_unioned_150
class OMPCFilterTest : public OmpcBaseTest {};




TEST_F(OMPCFilterTest, ompc_test_table_scan) {

    std::string limit_sql = "SELECT  l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(FLAGS_cutoff);
    std::string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input ORDER BY l_orderkey, l_linenumber";

    SortDefinition collation{ColumnSort(0, SortDirection::ASCENDING),
                            ColumnSort(3, SortDirection::ASCENDING)};

    Operator<Bit> *input =  new  StoredTableScan<Bit>("lineitem", FLAGS_cutoff);
    input->setOperatorId(-2);

    SecureTable *scanned = input->run();
    cout << "Finished table scan!\n";
    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = collation;
        PlainTable *revealed = scanned->revealInsecure();

        cout << "Revealed: " << revealed->toString(2, true) << endl;
        cout << "Expected: " << expected->toString(2, true) << endl;
        cout << "First bytes of expected: " << (int) (expected->column_data_[0].data())[0] << ", " << (int)(expected->column_data_[0].data())[1] << '\n';
        ASSERT_EQ(*expected, *revealed);

    }

}

// Testing for selecting l_linenumber=1

TEST_F(OMPCFilterTest, ompc_test_filter) {

    std::string limit_sql = "SELECT  l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT " + std::to_string(FLAGS_cutoff);
    std::string sql = "WITH input AS (" + limit_sql + ") SELECT * FROM input WHERE l_linenumber = 1 ORDER BY l_orderkey, l_linenumber";
    SortDefinition collation{ColumnSort(0, SortDirection::ASCENDING),
                             ColumnSort(3, SortDirection::ASCENDING)};

    Operator<Bit> *input =  new  StoredTableScan<Bit>("lineitem", FLAGS_cutoff);
    input->setOperatorId(-2);

    // filtering for l_linenumber = 1
    PackedInputReference<emp::Bit> read_linenumber_field(3, input->getOutputSchema());
    Field<emp::Bit> one(FieldType::SECURE_INT, emp::Integer(4, 0));
    LiteralNode<emp::Bit> constant_input(one);
    EqualNode<emp::Bit> equality_check((ExpressionNode<emp::Bit> *) &read_linenumber_field, (ExpressionNode<emp::Bit> *) &constant_input);
    Expression<emp::Bit> *expression = new GenericExpression<emp::Bit>(&equality_check, "predicate", FieldType::SECURE_BOOL);

    Filter<emp::Bit> *filter = new Filter(input, expression);
    filter->setOperatorId(-2);
    auto filtered = filter->run();

    if(FLAGS_validation) {
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = collation;

        PlainTable *revealed = filtered->revealInsecure();
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

#else
int main(int argc, char **argv) {
    std::cout << "emp-rescu backend not found!" << std::endl;
}

#endif
