#include <util/data_utilities.h>
#include "support/EmpBaseTest.h"
#include "query_table/secure_tuple.h"
#include "query_table/plain_tuple.h"

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

    size_t stringLength = 44;
    int stringBitCount = stringLength * 8;

    while(initialString.length() != stringLength) {
        initialString += " ";
    }



    bool *bools = Utilities::bytesToBool((int8_t *) initialString.c_str(), stringLength);

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



    vector<int8_t> decodedBytes =  Utilities::boolsToBytes(bools, stringBitCount);
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
    vector<int32_t> aliceInputData{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bobInputData{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};

    int32_t *inputData = (FLAGS_party == emp::ALICE) ?  aliceInputData.data() : bobInputData.data();
    
    QuerySchema schema(1);
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));


    std::unique_ptr<PlainTable> inputTable(new PlainTable(tupleCount, schema));

    for(uint32_t i = 0; i < tupleCount; ++i) {
        Field<bool> val(FieldType::INT,inputData[i]);
        PlainTuple tuple = (*inputTable)[i];

        tuple.setDummyTag(false);
        tuple.setField(0, val);
    }


    std::shared_ptr<SecureTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    SecureTuple secureTuple = (*encryptedTable)[0];
    emp::Integer decryptTest = secureTuple.getField(0).getValue<emp::Integer>();
    ASSERT_EQ(3, decryptTest.reveal<int32_t>());

    netio->flush();

    std::unique_ptr<PlainTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    // set up expected result
    std::unique_ptr<PlainTable > expectedTable(new PlainTable(2 * tupleCount, schema));

    // insert alice data last to first
    std::reverse(aliceInputData.begin(), aliceInputData.end());
    for(uint32_t i = 0; i < tupleCount; ++i) {
        Field<bool> val(FieldType::INT, aliceInputData[i]);
        PlainTuple tuple = (*expectedTable)[i];
        tuple.setDummyTag(false);
        tuple.setField(0, val);
    }

    // add bob's tuples from first to last
    for(uint32_t i = 0; i < tupleCount; ++i) {
        Field<bool> val(FieldType::INT, bobInputData[i]);
        PlainTuple tuple = (*expectedTable)[i+tupleCount];
        tuple.setDummyTag(false);
        tuple.setField(0, val);
    }

    //verify output
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";




}



TEST_F(EmpTest, sort_and_encrypt_table_one_column) {
    const uint32_t tupleCount = 10;
    vector<int32_t> aliceInputData{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bobInputData{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};

    /*const size_t tupleCount = 4;
    vector<int32_t> aliceInputData{1, 3, 4, 7};
    vector<int32_t> bobInputData{2, 5, 6, 8}; */
    int32_t *inputData = (FLAGS_party == emp::ALICE) ?  aliceInputData.data() : bobInputData.data();

    QuerySchema schema(1);
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(1);
    std::unique_ptr<PlainTable> inputTable(new PlainTable(tupleCount, schema, sortDefinition));

    for(uint32_t i = 0; i < tupleCount; ++i) {
        Field<bool> val(FieldType::INT,inputData[i]);
        PlainTuple tuple = (*inputTable)[i];

        tuple.setDummyTag(false);
        tuple.setField(0, val);
    }


    std::shared_ptr<SecureTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    netio->flush();

    std::unique_ptr<PlainTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);

    // set up expected result

    std::vector<int32_t> input_tuples = aliceInputData;
    input_tuples.insert(input_tuples.end(), bobInputData.begin(), bobInputData.end());
    std::sort(input_tuples.begin(), input_tuples.end());


    std::unique_ptr<PlainTable > expectedTable(new PlainTable(input_tuples.size(), schema, sortDefinition));

    for(uint32_t i = 0; i < input_tuples.size(); ++i) {
        Field<bool> val(FieldType::INT, input_tuples[i]);
        PlainTuple tuple = (*expectedTable)[i];
        tuple.setDummyTag(false);
        tuple.setField(0, val);
    }

    //verify output
    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";




}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}