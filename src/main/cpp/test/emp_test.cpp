#include <util/data_utilities.h>
#include "support/EmpBaseTest.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

using namespace vaultdb;

class EmpTest : public EmpBaseTest {
};




//  verify emp configuration for int32s from both ALICE and BOB

 TEST_F(EmpTest, emp_test_int) {


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

}



// basic test to verify emp configuration for strings
TEST_F(EmpTest, emp_test_varchar) {

    std::string initialString = "lithely regular deposits. fluffily";

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



    vector<int8_t> decodedBytes =  DataUtilities::boolsToBytes(bools, stringBitCount);
    decodedBytes.resize(stringLength + 1);
    decodedBytes[stringLength+1] = '\0';


    std::string decodedString((char *) decodedBytes.data());
    delete [] bools;


    ASSERT_EQ(initialString, decodedString);


}


// test encrypting a query table with a single int in EMP
// Testing absent psql dependency
TEST_F(EmpTest, encrypt_table_one_column) {
    const uint32_t tupleCount = 10;
    int32_t aliceInputData[10] = {1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    int32_t bobInputData[10] = {4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    int32_t *inputData = (FLAGS_party == emp::ALICE) ?  aliceInputData : bobInputData;

    QuerySchema schema(1);
    schema.putField(QueryFieldDesc(0, "test", "test_table", types::TypeId::INTEGER32));

    std::unique_ptr<QueryTable> inputTable(new QueryTable(tupleCount, 1));
    inputTable->setSchema(schema);

    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(inputData[i]);
        inputTable->getTuplePtr(i)->setDummyTag(false);
        QueryField *fieldPtr = inputTable->getTuplePtr(i)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }


    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    // set up expected result
    std::unique_ptr<QueryTable> expectedTable(new QueryTable(2 * tupleCount, 1));
    expectedTable->setSchema(schema);
    // insert alice data first to last
    for(uint32_t i = 0; i < tupleCount; ++i) {
        types::Value val(aliceInputData[i]);
        expectedTable->getTuplePtr(i)->setDummyTag(types::Value(false));
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
        expectedTable->getTuplePtr(i + offset)->setDummyTag(types::Value(false));
        QueryField *fieldPtr = expectedTable->getTuplePtr(i + offset)->getFieldPtr(0);
        fieldPtr->setValue(val);
        fieldPtr->setOrdinal(0);
    }


    //verify output
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";




}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}