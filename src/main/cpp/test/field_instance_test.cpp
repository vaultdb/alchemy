#include <gtest/gtest.h>
#include <stdexcept>

#include <query_table/field/field_instance.h>
#include <query_table/field/int_field.h>

#include <emp-tool/emp-tool.h>

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

    Field *decrypted = aField->reveal();
    ASSERT_EQ(5, ((IntField *)decrypted)->getValue());

    *aField = *cmp;

    int32_t copiedValue = ((IntField *) aField)->getValue();
    ASSERT_EQ(6, copiedValue);


    Field *revealed = aField->reveal();
    ASSERT_EQ("6", revealed->toString());


    delete aField;
    delete cmp;
    delete revealed;

}

TEST_F(FieldInstanceTest, MultiTypeContainer) {
    // TODO: store IntField and FloatField in same container

}

/*TEST_F(FieldInstanceTest, ExampleTest) {

    AbstractShape *aSquare = new Square();
    AbstractShape *aCircle = new Circle();

    ASSERT_EQ("square", aSquare->toString());
    ASSERT_EQ("circle", aCircle->toString());

    delete aSquare;
    delete aCircle;

}*/

