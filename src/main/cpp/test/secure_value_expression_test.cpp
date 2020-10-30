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

    types::Value lhsEncrypted =
            EmpManager::secretShareValue(lhsValue,
                                                             TypeId::VARCHAR, lhsStr.length() * 8, FLAGS_party,
                                                             emp::ALICE);
    types::Value rhsEncrypted = EmpManager::secretShareValue(rhsValue,
                                                             TypeId::VARCHAR, rhsStr.length() * 8, FLAGS_party,
                                                             emp::ALICE);


    std::string lhsRevealed = lhsEncrypted.reveal(emp::PUBLIC).getVarchar();
    std::string rhsRevealed = rhsEncrypted.reveal(emp::PUBLIC).getVarchar();

    std::cout << "Lhs bytes: ";
    for(int i = 0; i < lhsRevealed.size(); ++i)
        std::cout << (int) lhsRevealed[i] << " ";
    std::cout << " for " << lhsRevealed << " encrypted bits: " << lhsEncrypted.getEmpInt().reveal<std::string>() << std::endl;

    std::cout << "Rhs bytes: ";
    for(int i = 0; i < rhsRevealed.size(); ++i)
        std::cout << (int) rhsRevealed[i] << " ";
    std::cout <<  " for " << rhsRevealed  <<  " encrypted bits: " << rhsEncrypted.getEmpInt().reveal<std::string>() <<  std::endl;

    Integer thisExtend(lhsEncrypted.getEmpInt()), rhsExtend(rhsEncrypted.getEmpInt());
    size_t size = thisExtend.size();
    thisExtend.resize(size+1, true);
    rhsExtend.resize(size+1, true);
    Integer tmp = thisExtend-rhsExtend;

    // care about last bit, sign bit
    // it is 1, which indicates "less than"
    std::cout << "Geq temp: " << tmp.reveal<std::string>() << std::endl;

    types::Value gtEncrypted = (lhsEncrypted > rhsEncrypted);
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



TEST_F(SecureValueExpressionTest, test_char_comparison) {
    char lhs = 'O';
    char rhs = 'F';

    std::string lhsStr("O"), rhsStr("F");
/*  char lhs = 'N'; // 78
    char rhs = 'R'; // 82

    std::string lhsStr("N"), rhsStr("R");
*/

    emp::Integer lhsSecretShared(8, (int8_t) (FLAGS_party == emp::ALICE) ?  lhs : 0,
                                 emp::ALICE);

    emp::Integer rhsSecretShared(8, (int8_t) (FLAGS_party == emp::BOB) ?   rhs : 0,
                                 emp::BOB);
    types::Value lhsPrivateValue = EmpManager::secretShareValue(types::Value(lhsStr), types::TypeId::VARCHAR, 8, FLAGS_party, emp::ALICE);
    types::Value rhsPrivateValue = EmpManager::secretShareValue(types::Value(rhsStr), types::TypeId::VARCHAR, 8, FLAGS_party, emp::BOB);



    // sanity check
    bool publicEq = (lhs == rhs);
    bool privateEq = (lhsSecretShared == rhsSecretShared).reveal();
    bool publicGeq = lhs >= rhs;
    bool privateGeq = (lhsSecretShared >= rhsSecretShared).reveal();
    bool publicGt = lhsStr > rhsStr;
    bool privateGt = (lhsSecretShared >= rhsSecretShared).reveal();


    ASSERT_EQ(publicEq, privateEq);
    ASSERT_EQ(publicGeq, privateGeq);
    ASSERT_EQ(privateGt, publicGt);


    // compare manual emp int to Value one
    std::cout << "Manual secret shared int " << lhsSecretShared.reveal<std::string>() << std::endl
              << "Value-based int:         " << lhsPrivateValue.getEmpInt().reveal<std::string>() << std::endl;

    privateEq = (lhsPrivateValue == rhsPrivateValue).reveal().getBool();
    privateGeq = (lhsPrivateValue >= rhsPrivateValue).reveal().getBool();
    privateGt = (lhsPrivateValue > rhsPrivateValue).reveal().getBool();

    ASSERT_EQ(publicEq, privateEq);
    ASSERT_EQ(publicGeq, privateGeq);
    ASSERT_EQ(privateGt, publicGt);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





