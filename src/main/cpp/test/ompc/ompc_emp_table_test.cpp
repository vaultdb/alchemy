#include <test/mpc/emp_base_test.h>
#include <util/data_utilities.h>
#include <operators/sort.h>
#include <operators/packed_table_scan.h>
#include <data/secret_shared_data/secret_share_and_pack_data_from_query.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54334, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_int32(ctrl_port, 65428, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "wire_packed", "storage model for columns (column, wire_packed or compressed)");

using namespace vaultdb;

class OMPCEmpTableTest : public EmpBaseTest {
protected:
    void secretShareAndValidate(const std::string & sql, const SortDefinition & sort = SortDefinition());
};





// test encrypting a query table with a single int in EMP
TEST_F(OMPCEmpTableTest, ompc_secret_share_table_one_column) {

    std::string sql =  "SELECT l_orderkey FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_orderkey, l_linenumber";

    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(1);

    secretShareAndValidate(sql, collation);



}


// test secret a query table with a single string in EMP
TEST_F(OMPCEmpTableTest, ompc_secret_share_table_varchar) {

    string sql = "SELECT l_orderkey, l_linenumber, l_comment FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_orderkey, l_linenumber";
    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(2);

    secretShareAndValidate(sql, collation);

}



// test more column types
TEST_F(OMPCEmpTableTest, ompc_secret_share_table) {

    std::string sql =  "SELECT l_orderkey, l_linenumber, l_comment, l_returnflag, l_discount, "
                       "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "  // handle timestamps by converting them to longs using SQL
                       "FROM lineitem "
                       "WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                                                                               "ORDER BY l_orderkey, l_linenumber ";
    secretShareAndValidate(sql, DataUtilities::getDefaultSortDefinition(2));

}


TEST_F(OMPCEmpTableTest, ompc_secret_share_table_dummy_tag) {


    std::string sql = "SELECT l_orderkey, l_linenumber, l_comment, l_returnflag, l_discount, "
                      "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL
                      "l_returnflag <> 'N' AS dummy "  // simulate a filter for l_returnflag = 'N' -- all of the ones that dont match are dummies
                      "FROM lineitem "
                      " WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                                                                               "ORDER BY l_orderkey, l_linenumber ";
    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(2);
    PlainTable *input = DataUtilities::getQueryResults(db_name_, sql, true);

    input->order_by_ = collation;
    SecureTable *secret_shared = input->secretShare();
    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);
        DataUtilities::removeDummies(revealed);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, true);
        expected->order_by_ = collation;
        DataUtilities::removeDummies(expected);
        ASSERT_EQ(*expected, *revealed);
    }

}


TEST_F(OMPCEmpTableTest, ompc_bit_packing_test) {


    std::string sql = "SELECT c_custkey, c_nationkey FROM customer WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY (1)";


    PlainTable *input = DataUtilities::getQueryResults(db_name_, sql, false);
    SortDefinition collation = DataUtilities::getDefaultSortDefinition(1);
    input->order_by_ = collation;

    SecureTable *secret_shared = input->secretShare();
    //    cout << "Packed wire size: " << sizeof(emp::OMPCPackedWire) << " bytes, bit size: " << sizeof(emp::Bit) << " bytes, configured: " << sizeof(emp::Bit) << endl;
    // Expect: Packed wire size: 48 bytes, bit size: 192 bytes, configured: 48

    // c_custkey has 150 distinct vals, should have 8 bits
    ASSERT_EQ(8, secret_shared->getSchema().getField(0).size());
    // c_nationkey has 25 distinct vals, should have 5 bits
    ASSERT_EQ(5, secret_shared->getSchema().getField(1).size());

    PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
    expected->order_by_ = collation;

    ASSERT_EQ(secret_shared->tuple_cnt_,  expected->tuple_cnt_);

    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);
        DataUtilities::removeDummies(revealed);
        ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";
        delete revealed;
    }

    delete input;
    delete expected;
    delete secret_shared;

}

TEST_F(OMPCEmpTableTest, ompc_test_packed_table_scan) {
    std::string src_path = Utilities::getCurrentWorkingDirectory();
    std::string packed_pages_path = src_path + "/packed_pages/";

    std::string table_name = "customer";

    PackedTableScan<Bit> packed_table_scan(FLAGS_unioned_db, table_name, packed_pages_path, FLAGS_party);
    PackedColumnTable *packed_table = (PackedColumnTable *) packed_table_scan.run();

    if(FLAGS_validation) {
        std::string table_sql = "SELECT * FROM " + table_name + " ORDER BY c_custkey";

        SecretShareAndPackDataFromQuery ssp(FLAGS_unioned_db, table_sql, table_name);
        PackedColumnTable *expected = ssp.getTable();

        PlainTable *expected_plain = expected->revealInsecure(emp::PUBLIC);
        PlainTable *observed_plain = packed_table->revealInsecure(emp::PUBLIC);

        ASSERT_EQ(*expected_plain, *observed_plain);
    }
}

void OMPCEmpTableTest::secretShareAndValidate(const std::string & sql, const SortDefinition & sort) {

    PlainTable *input = DataUtilities::getQueryResults(db_name_, sql, false);

    input->order_by_ = sort;
    SecureTable *secret_shared = input->secretShare();

    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);
        DataUtilities::removeDummies(revealed);
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, sql, false);
        expected->order_by_ = sort;

        ASSERT_EQ(*expected, *revealed);
        delete revealed;
        delete expected;
    }

    delete input;
    delete secret_shared;
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

