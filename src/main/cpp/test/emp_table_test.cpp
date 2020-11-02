#include <data/PsqlDataProvider.h>
#include <test/support/EmpBaseTest.h>
#include <util/data_utilities.h>
#include <test/support/QueryTableTestQueries.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54324, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


class EmpTableTest : public EmpBaseTest {};





// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_one_column) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);




    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";


}





// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_varchar) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery =  "SELECT l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);




    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

}



TEST_F(EmpTableTest, encrypt_table_two_cols) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? aliceDb : bobDb;

    std::string inputQuery = "SELECT l_orderkey, l_comment "
                             "FROM lineitem "
                             "ORDER BY l_orderkey, l_linenumber "
                             "LIMIT 10";

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    netio->flush();
    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}



// test more column types
TEST_F(EmpTableTest, encrypt_table) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery = QueryTableTestQueries::getInputQuery();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);

    std::cout << "Initial table: " << *inputTable << std::endl;

    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;

    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob",  inputQuery, false);

    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}

TEST_F(EmpTableTest, encrypt_table_dummy_tag) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery = QueryTableTestQueries::getInputQueryDummyTag();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, true);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;
    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery,
                                                                                 true);

    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    return RUN_ALL_TESTS();
}

