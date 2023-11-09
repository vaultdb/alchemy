#include <test/mpc/emp_base_test.h>
#include <util/data_utilities.h>
#include <operators/sort.h>


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

using namespace vaultdb;

class EmpTableTest : public EmpBaseTest {
protected:
    void secretShareAndValidate(const std::string & sql, const SortDefinition & sort = SortDefinition());
};





// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, secret_share_table_one_column) {

    std::string sql =  "SELECT l_orderkey FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_orderkey, l_linenumber";

    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(1);

    secretShareAndValidate(sql, collation);



}


// test secret a query table with a single string in EMP
// bitonic merge the inputs
TEST_F(EmpTableTest, secret_share_table_varchar) {

    string sql = "SELECT l_orderkey, l_linenumber, l_comment FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_orderkey, l_linenumber";
    SortDefinition  collation = DataUtilities::getDefaultSortDefinition(2);

    secretShareAndValidate(sql, collation);

}



// test more column types
TEST_F(EmpTableTest, secret_share_table) {

    std::string sql =  "SELECT l_orderkey, l_linenumber, l_comment, l_returnflag, l_discount, "
                               "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "  // handle timestamps by converting them to longs using SQL
                               "FROM lineitem "
                               "WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                               "ORDER BY l_orderkey, l_linenumber ";
    secretShareAndValidate(sql, DataUtilities::getDefaultSortDefinition(2));

}


TEST_F(EmpTableTest, secret_share_table_dummy_tag) {


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


TEST_F(EmpTableTest, bit_packing_test) {


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
    // tuple_cnt * (5+8+1 (for dummy tag) )*sizeof(emp::Bit)
    if(emp_mode_ == vaultdb::EmpMode::SH2PC)
        ASSERT_EQ(expected->tuple_cnt_ * 14 * sizeof(emp::Bit),  secret_shared->tuple_cnt_ * secret_shared->tuple_size_bytes_);
    // 3 packed wires per tuple
    if(emp_mode_ == vaultdb::EmpMode::OUTSOURCED && SystemConfiguration::getInstance().wire_packing_enabled_)
        ASSERT_EQ(expected->tuple_cnt_ * 3 * sizeof(emp::OMPCPackedWire),  secret_shared->tuple_cnt_ * secret_shared->tuple_size_bytes_);

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

void EmpTableTest::secretShareAndValidate(const std::string & sql, const SortDefinition & sort) {

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

