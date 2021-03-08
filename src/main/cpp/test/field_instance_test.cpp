#include <gtest/gtest.h>
#include <stdexcept>

#include <typeinfo>
#include <query_table/field/field.h>
#include <query_table/field/field_instance.h>
//#include <query_table/field/int_field.h>
#include <query_table/field/bool_field.h>
//#include <query_table/field/secure_int_field.h>
#include <query_table/field/secure_bool_field.h>
#include <emp-tool/emp-tool.h>
//#include <util/emp_manager.h>

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>


using namespace vaultdb;

class FieldInstanceTest : public ::testing::Test {

protected:
    void SetUp() override{
        emp::setup_plain_prot(false, "");
    };
    void TearDown() override{
       emp::finalize_plain_prot();
    };

};


TEST_F(FieldInstanceTest, AssignmentTest) {
    BoolField a(true);
    BoolField b(false);
    Field aField = a.getBaseField();
    Field bField = b.getBaseField();

    ASSERT_EQ(aField.getValue<bool>(), true);
    ASSERT_EQ(bField.getValue<bool>(), false);

    aField = bField;
    ASSERT_EQ(aField.getValue<bool>(), false);

    BoolField c(aField);

    ASSERT_EQ(c.primitive(), false);
    ASSERT_EQ(c.getBaseField().getValue<bool>(), false);




}
TEST_F(FieldInstanceTest, MultiTypeContainer) {
    BoolField a(true);
    SecureBoolField b(true);
    BoolField c(false);
    SecureBoolField d(false);

    vector<Field> fields = {a.getBaseField(), b.getBaseField(), c.getBaseField(), d.getBaseField()};

    ASSERT_EQ(fields[0].getType(), FieldType::BOOL);
    ASSERT_EQ(fields[1].getType(), FieldType::SECURE_BOOL);

    ASSERT_EQ(fields[0].getValue<bool>(), true);
    ASSERT_EQ(fields[2].getValue<bool>(), false);

    ASSERT_EQ((fields[1].getValue<emp::Bit>()).reveal(), true);
    ASSERT_EQ((fields[3].getValue<emp::Bit>()).reveal(), false);



}






TEST_F(FieldInstanceTest, encryptTest) {
  BoolField boolField(true);
/*  IntField intField(7);

    Field encryptedBool = EmpManager::secretShareField(boolField.getBaseField(), emp::ALICE, emp::ALICE);
    bool revealed = ((SecureBoolField *) encryptedBool)->decrypt(emp::PUBLIC);
    ASSERT_EQ(true, revealed);

   Field * encryptedInt = EmpManager::secretShareField(intField, emp::ALICE, emp::ALICE);
   int32_t revealedInt = ((SecureIntField *) encryptedInt)->decrypt(emp::PUBLIC);
   ASSERT_EQ(7, revealedInt);*/


}


TEST_F(FieldInstanceTest, decryptTest) {

  /*  Field *encrypted = new SecureBoolField(true, emp::ALICE, emp::ALICE);
    Field *revealed = EmpManager::revealField( encrypted, emp::PUBLIC);
    bool rValue = ((BoolField *)revealed)->primitive();

    ASSERT_EQ(true, rValue);

    delete encrypted;
    delete revealed;
*/
}
