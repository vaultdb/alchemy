#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-tool/emp-tool.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <test/support/EmpBaseTest.h>
#include <query_table/field/field.h>
#include <query_table/field/string_field.h>
#include <query_table/field/secure_string_field.h>
#include <query_table/field/secure_int_field.h>
#include <query_table/field/secure_bool_field.h>
#include <query_table/field/secure_long_field.h>
#include <query_table/field/secure_float_field.h>




using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class SecureValueExpressionTest : public EmpBaseTest {

};



// test overload of assignment operator
TEST_F(SecureValueExpressionTest, test_value_assignment) {
    BoolField src(true);
    BoolField dst = src;

    ASSERT_EQ(src.getPayload(), dst.getPayload());


}

TEST_F(SecureValueExpressionTest, test_string_compare) {

    std::string lhsStr = "EGYPT                    ";
    std::string rhsStr = "ARGENTINA                ";

    StringField lhsValue(lhsStr);
    StringField rhsValue(rhsStr);

    SecureStringField lhsEncrypted(&lhsValue, lhsStr.length(), FLAGS_party, emp::ALICE);
    SecureStringField rhsEncrypted(&rhsValue, rhsStr.length(), FLAGS_party, emp::ALICE);


    SecureBoolField gtEncrypted = (lhsEncrypted > rhsEncrypted);
    bool gt = ((BoolField) gtEncrypted.reveal()).getPayload();
    ASSERT_TRUE(gt);

}


TEST_F(SecureValueExpressionTest, test_emp_int_math) {


    // alice inputs 7, bob inputs 12
    int32_t inputField =  FLAGS_party == emp::ALICE ? 7 : 12;

    emp::Integer aliceSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? inputField : 0,   emp::ALICE);

    SecureIntField aliceEncryptedValue(aliceSecretShared);

    emp::Integer bobSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? 0 : inputField,   emp::BOB);
    SecureIntField bobEncryptedValue(bobSecretShared);


    emp::Integer multiplier(32, 2); // set multiplier to two
    SecureIntField multiplierValue(multiplier);



    SecureIntField result = (aliceEncryptedValue + bobEncryptedValue) * multiplierValue;

    PlainField revealed = result.reveal(emp::PUBLIC);

    ASSERT_EQ(revealed.getValue<int32_t>(), 19*2);



}




TEST_F(SecureValueExpressionTest, test_millionaires) {

    // alice inputs 7, bob inputs 12
    int32_t inputField =  FLAGS_party == emp::ALICE ? 7 : 12;


    emp::Integer aliceSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? inputField : 0,   emp::ALICE);
    SecureIntField aliceEncryptedValue(aliceSecretShared);

    emp::Integer bobSecretShared = emp::Integer(32, FLAGS_party == emp::ALICE ? 0 : inputField,   emp::BOB);
    SecureIntField bobEncryptedValue(bobSecretShared);



    SecureBoolField result = aliceEncryptedValue > bobEncryptedValue;
    PlainField revealed = result.reveal(emp::PUBLIC);

    ASSERT_EQ(revealed.getValue<bool>(), false);


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


    StringField lhsField(lhsStr);
    StringField rhsField(rhsStr);

    SecureStringField lhsPrivateField = static_cast<SecureStringField>(PlainField::secretShare(&lhsField, FieldType::STRING, 8, FLAGS_party, emp::ALICE));
    SecureStringField rhsPrivateField = static_cast<SecureStringField>(PlainField::secretShare(&rhsField, FieldType::STRING, 8, FLAGS_party, emp::BOB));




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


    // compare manual emp int to Field one
    privateEq = ((BoolField)(lhsPrivateField == rhsPrivateField).reveal()).getPayload();
    privateGeq = ((BoolField)(lhsPrivateField >= rhsPrivateField).reveal()).getPayload();
    privateGt = ((BoolField)(lhsPrivateField > rhsPrivateField).reveal()).getPayload();


    ASSERT_EQ(publicEq, privateEq);
    ASSERT_EQ(publicGeq, privateGeq);
    ASSERT_EQ(privateGt, publicGt);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





