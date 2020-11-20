#include <util/data_utilities.h>
#include <util/emp_manager.h>
#include "support/EmpBaseTest.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

using namespace vaultdb;
using namespace emp;

class EmpFloatToIntTest : public EmpBaseTest {

protected:
    void testIntToFloat(const int32_t & input) const;
    std::string printFloatBits(const Float & aFloat) const;

};



void EmpFloatToIntTest::testIntToFloat(const int32_t & inputInt) const {
    std::cout << "****Input: " << inputInt << std::endl;

    int32_t input =  FLAGS_party == ALICE ? inputInt : 0;
    emp::Integer xInt(32, input, ALICE);
    int32_t check = xInt.reveal<int32_t>();
    ASSERT_EQ(check, inputInt);
    std::cout << "Int input bits: " << xInt.reveal<std::string>() << std::endl;

    emp::Float xFloat = EmpManager::castIntToFloat(xInt);
    float revealed = xFloat.reveal<double>(PUBLIC);
    std::cout << "Encrypted bits: " << printFloatBits(xFloat) << std::endl;


    float expectedOutput = (float) inputInt;
    if(inputInt > (1 << 24))  expectedOutput = (float) INT_MAX;
    if(inputInt < -1 * (1 << 24))  expectedOutput = (float) INT_MIN;
    ASSERT_FLOAT_EQ(expectedOutput, revealed);

}

std::string EmpFloatToIntTest::printFloatBits(const Float &aFloat) const {
    std::string output;

    for(int i = 0; i < FLOAT_LEN; ++i) {
        output += (aFloat.value[i].reveal()) ? "1" : "0";
    }

    return output;
}


// pick a few examples
TEST_F(EmpFloatToIntTest, basic_test) {
    // encrypting int from alice, but input party doesn't matter
    // using netio to test whole pipeline

    testIntToFloat(3);
    testIntToFloat(52);
    testIntToFloat(7);
    testIntToFloat(-5);
    testIntToFloat(INT_MIN);
    testIntToFloat( INT_MAX);
}

/*
void EmpFloatToIntTest::testIntToFloat(const int32_t & inputInt) const {
    std::cout << "******Input: " << inputInt << std::endl;

    int32_t input =  FLAGS_party == ALICE ? inputInt : 0;
    emp::Integer xInt(32, input, ALICE);
    int32_t check = xInt.reveal<int32_t>();
    ASSERT_EQ(check, inputInt);
    std::cout << "Int input bits: " << xInt.reveal<std::string>() << std::endl;

    emp::Float xFloat = EmpManager::castIntToFloat(xInt);
    float revealed = xFloat.reveal<double>(PUBLIC);
    std::cout << "Encrypted bits: " << printFloatBits(xFloat) << std::endl;
    float expectedOutput = (float) inputInt;
    if(inputInt > (1 << 24))  expectedOutput = (float) INT_MAX;
    if(inputInt > -1 * (1 << 24))  expectedOutput = (float) INT_MIN;

    ASSERT_EQ((float) inputInt, revealed );

}

std::string EmpFloatToIntTest::printFloatBits(const Float &aFloat) const {
    std::string output;

    for(int i = 0; i < FLOAT_LEN; ++i) {
        output += (aFloat.value[i].reveal()) ? "1" : "0";
    }

    return output;
}


// pick a few examples
TEST_F(EmpFloatToIntTest, basic_test) {
    // encrypting int from alice, but input party doesn't matter
    // using netio to test whole pipeline

    testIntToFloat(3);
    testIntToFloat(52);
    testIntToFloat(7);
    testIntToFloat(-5);
    testIntToFloat((int) INT_MIN);
    testIntToFloat((int) INT_MAX-1);
}

/*TODO: add this one next
 * TEST_F(EmpFloatToIntTest, randomized_test) {
    PRG prg;
    int rnd;

    for(int i = 0; i < 50; ++i) {
        prg.random_data(&rnd, 4);
        rnd %= (int) FLT_MAX;
        testIntToFloat(rnd);
    }

}
*/

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}
