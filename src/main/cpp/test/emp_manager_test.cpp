//
// Created by Jennie Rogers on 8/2/20.
//
#include <data/PsqlDataProvider.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <util/emp_manager.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include "support/QueryTableTestQueries.h"


using namespace emp;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");






class EmpManagerTest : public ::testing::Test {


protected:
    void SetUp() override {
        EmpManager *empManager = EmpManager::getInstance();
        empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
        std::cout << "Instantiated: " << empManager->toString() << std::endl;
    };
    void TearDown() override{
        EmpManager *empManager = EmpManager::getInstance();
        std::cout << "Deleting "  << empManager->toString() << std::endl;
        empManager->close();
        sleep(5); // wait for teardown between tests in a sequence
    };


};


// basic test to verify emp configuration for strings
TEST_F(EmpManagerTest, emp_manager_test_varchar) {

    // already configured during SetUp method
    EmpManager *empManager = EmpManager::getInstance();
    std::cout << "Received emp manager at " << empManager->toString() << std::endl;

    std::string initialString = "lithely regular deposits. fluffily";
    std::cout << "Encoding: " << initialString << std::endl;

    int stringBitCount = 352; // 42 * 8
    int stringLength = 44;
    while(initialString.length() != stringLength) {
        initialString += " ";
    }


    bool *bools = DataUtilities::bytesToBool((int8_t *) initialString.c_str(), stringLength);

    // encrypting as ALICE
    emp::Integer aliceSecretShared(stringBitCount, 0L, emp::ALICE);

    if(FLAGS_party == emp::ALICE) {
        ProtocolExecution::prot_exec->feed((block *)aliceSecretShared.bits.data(), emp::ALICE, bools, stringBitCount);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)aliceSecretShared.bits.data(), emp::ALICE, nullptr, stringBitCount);
    }

    empManager->flush();
    delete [] bools;



    // the standard reveal method converts this to decimal.  Need to reveal it bitwise

    bools = new bool[stringBitCount];
    ProtocolExecution::prot_exec->reveal(bools, emp::PUBLIC, (block *)aliceSecretShared.bits.data(),  stringBitCount);



    char *decodedBytes = (char *) DataUtilities::boolsToBytes(bools, stringBitCount);
    // make the char * null terminated
    char *tmp = new char[stringLength + 1];
    memcpy(tmp, decodedBytes, stringLength);
    tmp[stringLength] = '\0';
    delete [] decodedBytes;
    decodedBytes = tmp;


    std::string decodedString(decodedBytes);
    delete [] decodedBytes;
    delete [] bools;

    std::cout << "Decoded string: " << decodedString << std::endl;

    ASSERT_EQ(initialString, decodedString);

   // TearDown method will call netio destructor

}




//  verify emp configuration for int32s from both ALICE and BOB
TEST_F(EmpManagerTest, emp_manager_test_int) {

    EmpManager *empManager = EmpManager::getInstance();
    std::cout << "Setup: " << empManager->toString() << std::endl;


    // test encrypting an int from ALICE
    int32_t inputValue =  FLAGS_party == emp::ALICE ? 5 : 0;
    emp::Integer aliceSecretShared = emp::Integer(32, inputValue,  emp::ALICE);
    int32_t decrypted = aliceSecretShared.reveal<int32_t>(emp::PUBLIC);

    ASSERT_EQ(5, decrypted);


    // test encrypting int from BOB
    inputValue =  FLAGS_party == emp::ALICE ? 0 : 4;
    emp::Integer bobSecretShared = emp::Integer(32, inputValue,  emp::BOB);
    decrypted = bobSecretShared.reveal<int32_t>(emp::PUBLIC);

    ASSERT_EQ(4, decrypted);
   // delete netio_;
   //empManager->close();

}




// test encrypting a query table with a single int in EMP
TEST_F(EmpManagerTest, encrypt_table_one_column) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    //std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);




    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";


}


TEST_F(EmpManagerTest, encrypt_table_two_cols) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery = "SELECT l_orderkey, l_comment "
                             "FROM lineitem "
                             "ORDER BY l_orderkey, l_linenumber "
                             "LIMIT 10";

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare();

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(expectedTable->toString(), decryptedTable->toString()); // check the obvious stuff first
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";


}


// test more column types
TEST_F(EmpManagerTest, encrypt_table) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";
    EmpManager *empManager = EmpManager::getInstance();

    std::string inputQuery = QueryTableTestQueries::getInputQuery();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);

    std::cout << "Initial table: " << *inputTable << std::endl;

    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare();

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    empManager->flush();

    std::unique_ptr<QueryTable> expectedTable = QueryTableTestQueries::getExpectedSecureOutput(); //DataUtilities::getUnionedResults("tpch_alice", "tpch_bob",  EmpManagerTestEnvironment::getInputQuery(), false);

    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}

TEST_F(EmpManagerTest, encrypt_table_dummy_tag) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery = QueryTableTestQueries::getInputQueryDummyTag();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, true);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare();

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    std::unique_ptr<QueryTable> expectedTable = QueryTableTestQueries::getExpectedSecureOutputDummyTag(); //DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery,
                                                  //                               true);
                                                  
    std::cout << "Expected:\n" << expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

