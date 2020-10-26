//
// Created by Jennie Rogers on 8/2/20.
//
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include "support/EmpBaseTest.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class EmpTest : public EmpBaseTest {};




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


}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}