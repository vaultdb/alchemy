#include <util/data_utilities.h>

#include "emp_base_test.h"
#include <query_table/field/field_factory.h>

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54326, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");

using namespace vaultdb;
using namespace emp;

class EmpFloatToIntTest : public EmpBaseTest {

protected:
    void testIntToFloat(const int32_t & input) const;
    std::string printFloatBits(const Float & aFloat) const;
    std::string printIntBits(const int32_t & anInt) const;

};



void EmpFloatToIntTest::testIntToFloat(const int32_t & inputInt) const {

    int32_t input =  FLAGS_party == ALICE ? inputInt : 0;
    emp::Integer encryptedInt(32, input, ALICE);
    int32_t check = encryptedInt.reveal<int32_t>();

    std::string inputBitstring = printIntBits(inputInt);
    ASSERT_EQ(check, inputInt);

    SecureField f(FieldType::SECURE_INT, encryptedInt);
    SecureField encryptedFloat =  FieldFactory<emp::Bit>::toFloat(f);
    PlainField revealed = encryptedFloat.reveal(PUBLIC);

    // support range: [-2^24, 2^24]
    float expectedOutput = (float) inputInt;
    if(inputInt > (1 << 24))  expectedOutput = (float) INT_MAX;
    if(inputInt < -1 * (1 << 24))  expectedOutput = (float) INT_MIN;
    ASSERT_FLOAT_EQ(expectedOutput, revealed.getValue<float_t>());

}

std::string EmpFloatToIntTest::printFloatBits(const Float &aFloat) const {
    std::string output;

    for(int i = 0; i < FLOAT_LEN; ++i) {
        output += (aFloat.value[i].reveal()) ? "1" : "0";
    }

    return output;
}

std::string EmpFloatToIntTest::printIntBits(const int32_t &input) const {
    bool* b = new bool[32];
    int_to_bool<int64_t>(b, input, 32);
    std::string output;
    for(int i = 0; i < 32; ++i)
        output +=  b[i] ? "1" : "0";
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

 TEST_F(EmpFloatToIntTest, randomized_test) {
    // seed to make alice and bob produce the same random bits
    block seed = makeBlock(64, 0);
    PRG prg(&seed, 0);
    int rnd;
    int range = 1 << 24;

    for(int i = 0; i < 200; ++i) {
        prg.random_data(&rnd, 4);
        rnd %= range;
        testIntToFloat(rnd);
    }

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
