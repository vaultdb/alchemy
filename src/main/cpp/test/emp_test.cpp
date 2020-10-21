//
// Created by Jennie Rogers on 8/2/20.
//
#include <data/PsqlDataProvider.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>


using namespace emp;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");






class EmpTest : public ::testing::Test {


protected:
    void SetUp() override {


    };
    void TearDown() override{};



};


// basic test to verify emp configuration for strings
TEST_F(EmpTest, emp_test_varchar) {
    emp::NetIO *netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party);
    std::cout << "Initialized with netio: " << (long) netio << std::endl;

    std::string initialString = "lithely regular deposits. fluffily";
    std::cout << "Encoding: " << initialString << std::endl;

    int stringLength = 44;
    int stringBitCount = stringLength * 8;

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

    netio->flush();
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
    netio->flush();
    delete netio;
    sleep(1);


}




//  verify emp configuration for int32s from both ALICE and BOB
TEST_F(EmpTest, emp_test_int) {
    emp::NetIO *netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party);
    std::cout << "Initialized with netio: " << (long) netio << std::endl;


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

    std::cout << "Tearing down netio: " << (long) netio << std::endl;

    netio->flush();
    delete netio;
    sleep(1);


}




// test encrypting a query table with a single int in EMP
TEST_F(EmpTest, encrypt_table_one_column) {
    emp::NetIO *netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party);



    const uint32_t tupleCount = 10;
    int32_t aliceInputData[10] = {1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    int32_t bobInputData[10] = {4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    int32_t *inputData = (FLAGS_party == emp::ALICE) ?  aliceInputData : bobInputData;

    QuerySchema schema(1);
    schema.putField(QueryFieldDesc(0, false, "test", "test_table", types::TypeId::INTEGER32));

    std::unique_ptr<QueryTable> inputTable(new QueryTable(tupleCount, 1, false));
    inputTable->setSchema(schema);

    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(inputData[i]);
        inputTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = inputTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    // set up expected result
    std::unique_ptr<QueryTable> expectedTable(new QueryTable(2*tupleCount, 1, false));
    expectedTable->setSchema(schema);
   // insert alice data first to last
    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(aliceInputData[i]);
        expectedTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = expectedTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }

    int offset = tupleCount;

    // add bob's tuples from last to first
    int readIdx = tupleCount;
    for(uint32_t i = 0; i < tupleCount; ++i) {
        --readIdx;
        types::Value val(bobInputData[readIdx]);
        expectedTable->getTuplePtr(i+offset)->setDummyTag(false);
        QueryField *fieldPtr = expectedTable->getTuplePtr(i + offset)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }

    std::cout << "Tearing down netio: " << (long) netio << std::endl;
    netio->flush();
    delete netio;

    std::string inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::unique_ptr<QueryTable> expectedTable2 = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);

    if(*expectedTable != *decryptedTable)
        std::cout << "Break point!" << std::endl;

    //verify output
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

   sleep(1);


}


// test encrypting a query table with a single int in EMP
TEST_F(EmpTest, encrypt_table_one_column2) {
    emp::NetIO *netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party);

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  psqlInputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);



    const uint32_t tupleCount = 10;
    int32_t aliceInputData[10] = {1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    int32_t bobInputData[10] = {4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    int32_t *inputData = (FLAGS_party == emp::ALICE) ?  aliceInputData : bobInputData;

    QuerySchema schema(1);
    schema.putField(QueryFieldDesc(0, false, "l_orderkey", "lineitem", types::TypeId::INTEGER32));

    std::unique_ptr<QueryTable> inputTable(new QueryTable(tupleCount, 1, false));
    inputTable->setSchema(schema);

    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(inputData[i]);
        inputTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = inputTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }


    std::cout << "Initial table: " << *inputTable << std::endl;
    ASSERT_EQ(*inputTable, *psqlInputTable);


    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    // set up expected result
    std::unique_ptr<QueryTable> expectedTable(new QueryTable(2*tupleCount, 1, false));
    expectedTable->setSchema(schema);
    // insert alice data first to last
    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(aliceInputData[i]);
        expectedTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = expectedTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }

    int offset = tupleCount;

    // add bob's tuples from last to first
    int readIdx = tupleCount;
    for(uint32_t i = 0; i < tupleCount; ++i) {
        --readIdx;
        types::Value val(bobInputData[readIdx]);
        expectedTable->getTuplePtr(i+offset)->setDummyTag(false);
        QueryField *fieldPtr = expectedTable->getTuplePtr(i + offset)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }

    std::cout << "Tearing down netio: " << (long) netio << std::endl;
    netio->flush();
    delete netio;


    //verify output
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

    sleep(2);


}



// test encrypting a query table with a single int in EMP
TEST_F(EmpTest, encrypt_table_one_column3) {
    emp::NetIO *netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party);



    const uint32_t tupleCount = 10;
    int32_t aliceInputData[10] = {1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    int32_t bobInputData[10] = {4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    int32_t *inputData = (FLAGS_party == emp::ALICE) ?  aliceInputData : bobInputData;

    QuerySchema schema(1);
    schema.putField(QueryFieldDesc(0, false, "test", "test_table", types::TypeId::INTEGER32));

    std::unique_ptr<QueryTable> inputTable(new QueryTable(tupleCount, 1, false));
    inputTable->setSchema(schema);

    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(inputData[i]);
        inputTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = inputTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    // set up expected result
    std::unique_ptr<QueryTable> expectedTable(new QueryTable(2*tupleCount, 1, false));
    expectedTable->setSchema(schema);
    // insert alice data first to last
    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(aliceInputData[i]);
        expectedTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = expectedTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }

    int offset = tupleCount;

    // add bob's tuples from last to first
    int readIdx = tupleCount;
    for(uint32_t i = 0; i < tupleCount; ++i) {
        --readIdx;
        types::Value val(bobInputData[readIdx]);
        expectedTable->getTuplePtr(i+offset)->setDummyTag(false);
        QueryField *fieldPtr = expectedTable->getTuplePtr(i + offset)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }

    std::cout << "Tearing down netio: " << (long) netio << std::endl;
    netio->flush();
    delete netio;

    if(*expectedTable != *decryptedTable)
        std::cout << "Break point!" << std::endl;

    //verify output
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

    //sleep(2);


}
/*
TEST_F(EmpTest, encrypt_table_one_column_psql) {
    emp::NetIO *netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party);
    std::cout << "Initialized with netio: " << (long) netio << std::endl;

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    //std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);


    netio->flush();
    delete netio;

    if(*expectedTable != *decryptedTable)
        std::cout << "Break point!" << std::endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";
    sleep(2);


}



TEST_F(EmpTest, encrypt_table_two_cols) {
    emp::NetIO *netio =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
    emp::setup_semi_honest(netio, FLAGS_party);
    std::cout << "Initialized with netio: " << (long) netio << std::endl;


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
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    std::cout << "Tearing down netio: " << (long) netio << std::endl;

    netio->flush();
    delete netio;

    if(*expectedTable != *decryptedTable)
        std::cout << "Break point!" << std::endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";


    sleep(2);


}



// test more column types
TEST_F(EmpTest, encrypt_table) {

    netio_ =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
        emp::setup_semi_honest(netio_, FLAGS_party);
        std::cout << "Initialized with netio: " << (long) netio_ << std::endl;

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

    std::unique_ptr<QueryTable> expectedTable = QueryTableTestQueries::getExpectedSecureOutput(); //DataUtilities::getUnionedResults("tpch_alice", "tpch_bob",  EmpTestEnvironment::getInputQuery(), false);

    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

         std::cout << "Tearing down netio: " <<  (long) netio_ << std::endl;

        netio_->flush();
        delete netio_;
            sleep(2);




}

TEST_F(EmpTest, encrypt_table_dummy_tag) {
    netio_ =  new emp::NetIO(FLAGS_party == emp::ALICE ? nullptr : FLAGS_alice_host.c_str(), FLAGS_port);
        emp::setup_semi_honest(netio_, FLAGS_party);
        std::cout << "Initialized with netio: " << (long) netio_ << std::endl;


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

         std::cout << "Tearing down netio: " <<  (long) netio_ << std::endl;

        netio_->flush();
        delete netio_;

    sleep(2);


}
 */


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

