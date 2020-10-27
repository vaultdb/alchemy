//
// Created by Jennie Rogers on 8/14/20.
//



#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-tool/emp-tool.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <util/emp_manager.h>
#include <test/support/EmpBaseTest.h>


using namespace emp;
using namespace vaultdb::types;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class SecureValueExpressionTest : public EmpBaseTest {};



// test overload of assignment operator
TEST_F(SecureValueExpressionTest, test_value_assignment) {
    Value src(true);
    Value dst = src;

    ASSERT_EQ(src.getBool(), dst.getBool());



}

TEST_F(SecureValueExpressionTest, test_string_compare) {

    std::string lhsStr = "EGYPT                    ";
    std::string rhsStr = "ARGENTINA                ";

    types::Value lhsValue(lhsStr);
    types::Value rhsValue(rhsStr);

    types::Value lhsEncrypted = EmpManager::secretShareValue(&lhsValue, lhsStr.length() * 8, FLAGS_party,  emp::ALICE);
    types::Value rhsEncrypted = EmpManager::secretShareValue(&rhsValue, rhsStr.length() * 8, FLAGS_party,  emp::ALICE);

    types::Value gtEncrypted = (lhsValue > rhsValue);
    bool gt = gtEncrypted.reveal().getBool();


    ASSERT_TRUE(gt);

}


TEST_F(SecureValueExpressionTest, test_emp_int_math) {


    // alice inputs 7, bob inputs 12
    int32_t inputValue =  FLAGS_party == emp::ALICE ? 7 : 12;

    emp::Integer aliceSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? inputValue : 0,   emp::ALICE);
    Value aliceEncryptedValue(TypeId::ENCRYPTED_INTEGER32, aliceSecretShared);

    emp::Integer bobSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? 0 : inputValue,   emp::BOB);
    Value bobEncryptedValue(TypeId::ENCRYPTED_INTEGER32, bobSecretShared);


    emp::Integer multiplier(32, 2); // set multiplier to two
    Value multiplierValue(TypeId::ENCRYPTED_INTEGER32, multiplier);



    Value result = (aliceEncryptedValue + bobEncryptedValue) * multiplierValue;

    Value revealed = result.reveal(emp::PUBLIC);

    ASSERT_EQ(revealed.getInt32(), 19*2);



}




TEST_F(SecureValueExpressionTest, test_millionaires) {

    // alice inputs 7, bob inputs 12
    int32_t inputValue =  FLAGS_party == emp::ALICE ? 7 : 12;

    emp::Integer aliceSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? inputValue : 0,   emp::ALICE);
    Value aliceEncryptedValue(TypeId::ENCRYPTED_INTEGER32, aliceSecretShared);

    emp::Integer bobSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? 0 : inputValue,   emp::BOB);
    Value bobEncryptedValue(TypeId::ENCRYPTED_INTEGER32, bobSecretShared);



    Value result = aliceEncryptedValue > bobEncryptedValue;
    Value revealed = result.reveal(emp::PUBLIC);

    ASSERT_EQ(revealed.getBool(), false);


}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





