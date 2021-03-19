#include <gtest/gtest.h>
#include <stdexcept>

#include <typeinfo>
#include <emp-tool/emp-tool.h>
#include <query_table/field/field.h>
#include <query_table/field/int_field.h>
#include <query_table/field/bool_field.h>
#include <query_table/field/secure_bool_field.h>
#include <query_table/field/long_field.h>


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
    Field<BoolField> *aField = &a;
    Field<BoolField> *bField = &b;

    ASSERT_EQ(aField->getValue<bool>(), true);
    ASSERT_EQ(bField->getValue<bool>(), false);

    *aField = *bField;
    ASSERT_EQ(aField->getValue<bool>(), false);

    BoolField c(*aField);

    ASSERT_EQ(c.getPayload(), false);
    ASSERT_EQ(c.getValue<bool>(), false);




}
TEST_F(FieldInstanceTest, MultiTypeContainer) {
    BoolField a(true);
    IntField b(7);
    BoolField c(false);
    IntField d(42);

    vector<Field<BoolField> > fields;
    fields.reserve(4);
    fields.push_back(a);
    fields.push_back(b);
    fields.push_back(c);
    fields.push_back(d);



    ASSERT_EQ(fields[0].getType(), FieldType::BOOL);
    ASSERT_EQ(fields[1].getType(), FieldType::INT);

    ASSERT_EQ(fields[0].getValue<bool>(), true);
    ASSERT_EQ(fields[2].getValue<bool>(), false);

    ASSERT_EQ(fields[1].getValue<int32_t>(), 7);
    ASSERT_EQ(fields[3].getValue<int32_t>(), 42);

    std::cout << "Entering cleanup!" << std::endl;

}






TEST_F(FieldInstanceTest, printLongTest) {
    LongField l(830044800L);
    ASSERT_EQ(l.toString(), "830044800");
}

TEST_F(FieldInstanceTest, encryptTest) {
  BoolField boolField(true);
  Field<SecureBoolField> *encrypted = Field<BoolField>::secretShare(&boolField, FieldType::BOOL, 0, emp::ALICE, emp::ALICE);
  BoolField *revealed = (BoolField *) encrypted->reveal();
  ASSERT_EQ(revealed->getPayload(), true);

  delete encrypted;
  delete revealed;

  IntField intField(7);
  encrypted = Field<BoolField>::secretShare(&intField, FieldType::INT, 0, emp::ALICE, emp::ALICE);
  IntField *revealedInt = (IntField *) encrypted->reveal();
  ASSERT_EQ(revealedInt->getPayload(), 7);

  delete encrypted;
  delete revealedInt;

}


