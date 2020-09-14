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


using namespace emp;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


// TODO: refactor this into a common utility with QueryTableTest
// TODO: figure out how to codify that this test depends on QueryTableTest
class EmpManagerTestEnvironment : public ::testing::Environment {
public:
    // Assume there's only going to be a single instance of this class, so we can just
    // hold the timestamp as a const static local variable and expose it through a
    // static member function
    static std::string getInputQuery() {
        // selecting one of each type:
        // int
        // varchar
        // fixed char
        // numeric
        // date

        static const std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                                              "EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                              "FROM lineitem "
                                              "ORDER BY l_orderkey "
                                              "LIMIT 5";
        return inputQuery;
    }

    static const std::unique_ptr<QueryTable> getExpectedOutput() {

        return DataUtilities::getUnionedResults(getInputQuery(), "tpch_alice", "tpch_bob", false);

    }

    static std::string getInputQueryDummyTag() {
        // selecting one of each type:
        // int
        // varchar
        // fixed char
        // numeric
        // date
        // dummy tag

        static const std::string inputQueryDummyTag = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                                                      "EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                                      "l_returnflag <> 'N' AS dummy "  // simulate a filter for l_returnflag = 'N' -- all of the ones that dont match are dummies
                                                      "FROM lineitem "
                                                      "ORDER BY l_orderkey "
                                                      "LIMIT 5";
        return inputQueryDummyTag;
    }


    static const std::unique_ptr<QueryTable> getExpectedOutputDummyTag() {

        return DataUtilities::getUnionedResults(getInputQueryDummyTag(), "tpch_alice", "tpch_bob", false);

    }
    virtual void SetUp() {

    }

};




class EmpManagerTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};





//  verify emp configuration for int32s
TEST_F(EmpManagerTest, emp_manager_test_int) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port,  FLAGS_party);

    int32_t inputValue =  FLAGS_party == emp::ALICE ? 1 : 0;

    emp::Integer aliceSecretShared = emp::Integer(32, inputValue,  emp::ALICE);
    empManager->flush();

    int32_t decrypted = aliceSecretShared.reveal<int32_t>(emp::PUBLIC);
    empManager->flush();

    ASSERT_EQ(1, decrypted);

    inputValue =  FLAGS_party == emp::ALICE ? 0 : 4;

    emp::Integer bobSecretShared = emp::Integer(32, inputValue,  emp::BOB);

    empManager->flush();
    decrypted = bobSecretShared.reveal<int32_t>(emp::PUBLIC);
    ASSERT_EQ(4, decrypted);

    empManager->close();

}


// basic test to verify emp configuration for strings
TEST_F(EmpManagerTest, emp_manager_test_varchar) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);

    std::string initialString = "lithely regular deposits. fluffily";
    std::cout << "Encoding: " << initialString << std::endl;

    int stringBitCount = 352; // 42 * 8
    int stringLength = 44;
    while(initialString.length() != stringLength) {
        initialString += " ";
    }

    bool *bools = DataUtilities::bytesToBool((int8_t *) initialString.c_str(), stringLength);
    // ENCRYPT THIS
    emp::Bit *bits = new Bit[stringBitCount];
    if(FLAGS_party == emp::ALICE) {
        emp::init(bits, bools, stringBitCount, emp::ALICE);
    }
    else {
        emp::init(bits, nullptr, stringBitCount, emp::ALICE);
    }
    empManager->flush();

    delete [] bools;


    Integer aliceSecretShared = Integer(stringBitCount, bits);

    // the standard reveal method converts this to decimal.  Need to reveal it bitwise

    bools = new bool[stringBitCount];
    ProtocolExecution::prot_exec->reveal(bools, emp::PUBLIC, (block *)aliceSecretShared.bits,  stringBitCount);



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

    empManager->close();






}



// test encrypting a query table with a single int in EMP
TEST_F(EmpManagerTest, encrypt_table_one_column) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port,  FLAGS_party);


    std::string inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 2";
    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = empManager->secretShareTable(inputTable.get());

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;

    empManager->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);


    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

    empManager->close();


}


TEST_F(EmpManagerTest, encrypt_table_two_cols) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port,  FLAGS_party);

    std::string inputQuery = "SELECT l_orderkey, l_comment "
                             "FROM lineitem "
                             "ORDER BY l_orderkey, l_linenumber "
                             "LIMIT 10";

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = empManager->secretShareTable(inputTable.get());

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    empManager->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery, false);
    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(expectedTable->toString(), decryptedTable->toString()); // check the obvious stuff first
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

    empManager->close();


}


// test more column types
TEST_F(EmpManagerTest, encrypt_table) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port,  FLAGS_party);

    std::string inputQuery = EmpManagerTestEnvironment::getInputQuery();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);

    std::cout << "Initial table: " << *inputTable << std::endl;

    std::shared_ptr<QueryTable> encryptedTable = empManager->secretShareTable(inputTable.get());

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    empManager->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults(
           "tpch_alice", "tpch_bob",  EmpManagerTestEnvironment::getInputQuery(), false);

    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

    empManager->close();


}


TEST_F(EmpManagerTest, encrypt_table_dummy_tag) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);

    std::string inputQuery = EmpManagerTestEnvironment::getInputQueryDummyTag();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, true);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = empManager->secretShareTable(inputTable.get());
    empManager->flush();

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;


    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery,
                                                                                 true);


    std::cout << "Expected:\n" << expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";

    empManager->close();


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

