#include <test/ompc/ompc_base_test.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include <operators/table_scan.h>

#if __has_include("emp-rescu/emp-rescu.h")


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54334, "port for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65428, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column or wire_packed)");
DEFINE_string(empty_db, "tpch_empty", "empty db name for schemas");
DEFINE_string(wire_path, "wires", "local path to wire files");
DEFINE_int32(input_party, 10086, "party for input data");
DEFINE_int32(unpacked_page_size_bits, 2048, "unpacked page size in bits");
DEFINE_int32(page_cnt, 50, "number of pages in unpacked buffer pool");

using namespace vaultdb;

class OMPCStoredTableTest : public OmpcBaseTest {
protected:
    void scanAndValidate(const string & table_name, const std::string & sql, const SortDefinition & sort = SortDefinition());
};




void OMPCStoredTableTest::scanAndValidate(const string & table_name, const std::string & sql, const SortDefinition & sort) {

    TableScan<Bit> scan(table_name);
    auto observed = scan.run();

    if(FLAGS_validation) {
        PlainTable *revealed = observed->revealInsecure(emp::PUBLIC);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort;

        ASSERT_EQ(*expected, *revealed);
        delete revealed;
        delete expected;
    }

}





TEST_F(OMPCStoredTableTest, lineitem) {

    std::string sql =  "SELECT l_orderkey, l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING)};
    scanAndValidate("lineitem", sql, collation);

}

TEST_F(OMPCStoredTableTest, orders) {

    std::string sql =  "SELECT o_orderkey, o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, o_orderyear FROM orders ORDER BY o_orderkey";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING)};
    scanAndValidate("orders", sql, collation);

}

TEST_F(OMPCStoredTableTest, customer) {

    std::string sql =  "SELECT c_custkey, c_name, c_address, c_nationkey, c_phone, c_acctbal, c_mktsegment, c_comment FROM customer ORDER BY c_custkey";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING)};
    scanAndValidate("customer", sql, collation);

}

TEST_F(OMPCStoredTableTest, part) {

    std::string sql = "SELECT p_partkey, p_name, p_mfgr, p_brand, p_type, p_size, p_container, p_retailprice, p_comment FROM part ORDER BY p_partkey";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING)};
    scanAndValidate("part", sql, collation);

}

TEST_F(OMPCStoredTableTest, partsupp) {

    std::string sql =  "SELECT ps_partkey, ps_suppkey, ps_availqty, ps_supplycost, ps_comment FROM partsupp ORDER BY ps_partkey, ps_suppkey";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::ASCENDING)};
    scanAndValidate("part", sql, collation);

}

TEST_F(OMPCStoredTableTest, supplier) {

    std::string sql =  "SELECT  s_suppkey, s_name, s_address, s_nationkey, s_phone, s_acctbal, s_comment FROM supplier ORDER BY s_suppkey";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING)};
    scanAndValidate("supplier", sql, collation);

}

TEST_F(OMPCStoredTableTest, nation) {

    std::string sql =  "SELECT n_nationkey, n_name, n_regionkey, n_comment FROM nation ORDER BY n_nationkey";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING)};
    scanAndValidate("nation", sql, collation);

}

TEST_F(OMPCStoredTableTest, region) {

    std::string sql =  "SELECT r_regionkey, r_name, r_comment  FROM region ORDER BY r_regionkey";

    SortDefinition  collation = {ColumnSort(0, SortDirection::ASCENDING)};
    scanAndValidate("region", sql, collation);

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

//#else
//
//int main(int argc, char **argv) {
//   std::cout << "emp-rescu backend not found!" << std::endl;
//}

#endif
