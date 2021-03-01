#include <gtest/gtest.h>
#include <stdexcept>

#include <query_table/field/field_instance.h>
#include <query_table/field/int_field.h>
#include <query_table/field/bool_field.h>
#include <query_table/field/secure_int_field.h>
#include <query_table/field/secure_bool_field.h>

#include <emp-tool/emp-tool.h>
#include <util/emp_manager.h>

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
    Field *aField = new IntField(5);
    Field *cmp = new IntField(6);

    ASSERT_EQ("5", aField->toString());

    // calling FieldInstance::getValue
    int32_t value = ((IntField *) aField)->getValue();
    ASSERT_EQ(5, value);


    // testing assignment
    *aField = *cmp;

    int32_t copiedValue = ((IntField *) aField)->getValue();
    ASSERT_EQ(6, copiedValue);




    // downcast to IntType
    // produces IntField &
    //decltype(*aField) added = *aField + *cmp;

    Field *addedField = static_cast<Field *>( &( *aField + *cmp));
    std::cout << "Field: " << ((IntField *) aField)->getValue() << " + "  << ((IntField *) cmp)->getValue() << std::endl;
    ASSERT_EQ("12", addedField->toString());


    delete aField;
    aField = new IntField(7);
    Field *geq  = &(*aField >= *cmp);

    std::cout << "Is 7 >= 6?  " << geq->toString() << std::endl;
    ASSERT_EQ("true", geq->toString());


    delete aField;
    delete cmp;
    delete addedField;
    delete geq;

}

TEST_F(FieldInstanceTest, MultiTypeContainer) {
    std::vector<Field *> myFields;
/*    myFields.emplace_back(new IntField(7));
    myFields.emplace_back(new BoolField(true));


    ASSERT_EQ(myFields.size(), 2);
    for(auto field : myFields) {
        std::cout << field->toString() << std::endl;
        delete field;
    }

    myFields.clear();*/
}


Field * encryptField(Field *srcField)  {
    FieldType type = srcField->getType();
    if(type == FieldType::BOOL) {
        BoolField *src = (BoolField *) srcField;
        bool toEncrypt = src->primitive();
        return new SecureBoolField(toEncrypt, emp::ALICE, emp::ALICE);
    }
    else if(type == FieldType::INT32) {
        IntField *src = (IntField *) srcField;
        int32_t toEncrypt = src->primitive();
        return new SecureIntField(toEncrypt, emp::ALICE, emp::ALICE);
    }

    return nullptr;

}


TEST_F(FieldInstanceTest, encryptTest) {
    Field *boolField = new BoolField(true);
    Field *intField = new IntField(7);

    Field * encryptedBool = EmpManager::encryptField(boolField, emp::ALICE, emp::ALICE);
    bool revealed = ((SecureBoolField *) encryptedBool)->decrypt(emp::PUBLIC);
    ASSERT_EQ(true, revealed);

   Field * encryptedInt = EmpManager::encryptField(intField, emp::ALICE, emp::ALICE);
   int32_t revealedInt = ((SecureIntField *) encryptedInt)->decrypt(emp::PUBLIC);
   ASSERT_EQ(7, revealedInt);

    delete boolField;
   delete intField;
    delete encryptedBool;
    delete encryptedInt;

}


TEST_F(FieldInstanceTest, decryptTest) {
    Field *encrypted = new SecureBoolField(true, emp::ALICE, emp::ALICE);
    Field *revealed = EmpManager::revealField( encrypted, emp::PUBLIC);
    bool rValue = ((BoolField *)revealed)->primitive();

    ASSERT_EQ(true, rValue);

    delete encrypted;
    delete revealed;

}