#include <data/PsqlDataProvider.h>
#include <test/support/EmpBaseTest.h>
#include <util/data_utilities.h>
#include <test/support/QueryTableTestQueries.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54324, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

using namespace vaultdb;

class EmpTableTest : public EmpBaseTest {};





// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_one_column) {

    PsqlDataProvider dataProvider;

    std::string inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::cout << "Querying " << dbName << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(dbName,
                                                                         inputQuery, false);


    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);




    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";


}





// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_varchar) {

    PsqlDataProvider dataProvider;

    std::string inputQuery =  "SELECT l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);


    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(dbName,
                                                                         inputQuery, false);

    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    std::cout << "Done encrypting table!" << std::endl;

    netio->flush();

    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);





    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

}



TEST_F(EmpTableTest, encrypt_table_two_cols) {

    PsqlDataProvider dataProvider;

    std::string inputQuery = "SELECT l_orderkey, l_comment "
                             "FROM lineitem "
                             "ORDER BY l_orderkey, l_linenumber "
                             "LIMIT 10";

    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(dbName,
                                                                         inputQuery, false);

    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}



// test more column types
TEST_F(EmpTableTest, encrypt_table) {

    PsqlDataProvider dataProvider;
    std::string inputQuery = QueryTableTestQueries::getInputQuery();
    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(dbName,
                                                                         inputQuery, false);

    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob",  inputQuery, false);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);


    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}

TEST_F(EmpTableTest, encrypt_table_dummy_tag) {

    PsqlDataProvider dataProvider;

    std::string inputQuery = QueryTableTestQueries::getInputQueryDummyTag();
    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(dbName,
                                                                         inputQuery, true);

    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery,
                                                                                 true);

    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    return RUN_ALL_TESTS();
}

