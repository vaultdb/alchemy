#include <data/PsqlDataProvider.h>
#include <test/mpc/emp_base_test.h>
#include <util/data_utilities.h>
#include <test/support/QueryTableTestQueries.h>
#include <sort.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54324, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

using namespace vaultdb;

class EmpTableTest : public EmpBaseTest {};





// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_one_column) {

    PsqlDataProvider dataProvider;

    std::string input_query =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::shared_ptr<PlainTable>  input_table = dataProvider.getQueryTable(db_name_,
                                                                          input_query, false);

    SortDefinition  sort_definition = DataUtilities::getDefaultSortDefinition(1);
    input_table->setSortOrder(sort_definition);

    std::shared_ptr<SecureTable> encrypted = PlainTable::secretShare(*input_table, netio_, FLAGS_party);

    netio_->flush();

    std::shared_ptr<PlainTable> expected = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", input_query, false);
    Sort sorter(expected, sort_definition);
    expected = sorter.run();

    std::unique_ptr<PlainTable> decrypted = encrypted->reveal(emp::PUBLIC);
    expected->setSortOrder(sort_definition);



    ASSERT_EQ(*expected, *decrypted) << "Query table was not processed correctly.";


}




// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_varchar) {

    PsqlDataProvider dataProvider;

    std::string input_query =  "SELECT l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";

    std::shared_ptr<PlainTable> expected = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", input_query, false);


    std::shared_ptr<PlainTable>  input_table = dataProvider.getQueryTable(db_name_,
                                                                          input_query, false);

    std::shared_ptr<SecureTable> encrypted = PlainTable::secretShare(*input_table, netio_, FLAGS_party);

    netio_->flush();

    std::unique_ptr<PlainTable> decrypted_table = encrypted->reveal(emp::PUBLIC);





    ASSERT_EQ(*expected, *decrypted_table) << "Query table was not processed correctly.";

}



TEST_F(EmpTableTest, encrypt_table_two_cols) {

    PsqlDataProvider dataProvider;

    std::string input_query = "SELECT l_orderkey, l_comment "
                             "FROM lineitem "
                             "ORDER BY l_orderkey, l_linenumber "
                             "LIMIT 10";

    std::shared_ptr<PlainTable>  input_table = dataProvider.getQueryTable(db_name_,
                                                                          input_query, false);

    std::shared_ptr<SecureTable> encrypted = PlainTable::secretShare(*input_table, netio_, FLAGS_party);
    netio_->flush();

    std::shared_ptr<PlainTable> expected = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", input_query, false);
    std::unique_ptr<PlainTable> decrypted = encrypted->reveal(emp::PUBLIC);

    ASSERT_EQ(*expected, *decrypted) << "Query table was not processed correctly.";



}



// test more column types
TEST_F(EmpTableTest, encrypt_table) {

    PsqlDataProvider dataProvider;
    std::string input_query = QueryTableTestQueries::getInputQuery();
    std::shared_ptr<PlainTable>  input_table = dataProvider.getQueryTable(db_name_,
                                                                          input_query, false);

    std::shared_ptr<SecureTable> encrypted = PlainTable::secretShare(*input_table, netio_, FLAGS_party);

    netio_->flush();

    std::shared_ptr<PlainTable> expected = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob",  input_query, false);
    std::unique_ptr<PlainTable> decrypted = encrypted->reveal(emp::PUBLIC);


    ASSERT_EQ(*expected, *decrypted) << "Query table was not processed correctly.";



}


TEST_F(EmpTableTest, encrypt_table_dummy_tag) {

    PsqlDataProvider dataProvider;

    std::string input_query = QueryTableTestQueries::getInputQueryDummyTag();
    std::shared_ptr<PlainTable>  input_table = dataProvider.getQueryTable(db_name_,
                                                                          input_query, true);

    std::shared_ptr<SecureTable> encrypted = PlainTable::secretShare(*input_table, netio_, FLAGS_party);

    netio_->flush();

    std::shared_ptr<PlainTable> expected = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", input_query,
                                                                                 true);

    std::unique_ptr<PlainTable> decrypted = encrypted->reveal(emp::PUBLIC);

    ASSERT_EQ(*expected, *decrypted) << "Query table was not processed correctly.";



}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    return RUN_ALL_TESTS();
}

