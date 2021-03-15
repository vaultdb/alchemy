#include <gtest/gtest.h>
#include <stdexcept>

#include <typeinfo>
#include <query_table/field/field.h>
#include <query_table/field/field_instance.h>
#include <query_table/field/int_field.h>
#include <query_table/field/bool_field.h>
#include <query_table/field/secure_int_field.h>
#include <query_table/field/secure_bool_field.h>
#include <emp-tool/emp-tool.h>
#include <query_table/field/field_factory.h>

#include <boost/variant2/variant.hpp>

// for variant test
#include <tuple>
#include <boost/variant.hpp>




using namespace vaultdb;
using namespace boost::variant2;


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
    Field *aField = &a;
    Field *bField = &b;

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
    SecureBoolField b(true);
    BoolField c(false);
    SecureBoolField d(false);

    vector<std::unique_ptr<Field> > fields;
    fields.reserve(4);
    fields.push_back(std::unique_ptr<Field>(new BoolField(true)));

    fields.push_back(std::unique_ptr<Field>(new SecureBoolField(true)));
    fields.push_back(std::unique_ptr<Field>(new BoolField(false)));
    fields.push_back(std::unique_ptr<Field>(new SecureBoolField(false)));

    ASSERT_EQ(fields[0]->getType(), FieldType::BOOL);
    ASSERT_EQ(fields[1]->getType(), FieldType::SECURE_BOOL);

    ASSERT_EQ(fields[0]->getValue<bool>(), true);
    ASSERT_EQ(fields[2]->getValue<bool>(), false);

    ASSERT_EQ((fields[1]->getValue<emp::Bit>()).reveal(), true);
    ASSERT_EQ((fields[3]->getValue<emp::Bit>()).reveal(), false);



}






TEST_F(FieldInstanceTest, printLongTest) {
    LongField l(830044800L);
    ASSERT_EQ(l.toString(), "830044800");
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
    bool rField = ((BoolField *)revealed)->primitive();

    ASSERT_EQ(true, rValue);

    delete encrypted;
    delete revealed;
*/
}

// based on: https://wandbox.org/permlink/Q6EjjI4HsgR0asIO
// returns typename of Nth element in Tuple
template<int N, typename... Ts> using NthTypeOf =
typename std::tuple_element<N, std::tuple<Ts...>>::type;

template<int N, typename... Ts>
auto &get(boost::variant<Ts...> &v) {
    using target = NthTypeOf<N, Ts...>;
    return boost::get<target>(v);
}

template<int N, typename... Ts>
auto &get(const boost::variant<Ts...> &v) {
    using target = NthTypeOf<N, Ts...>;
    return boost::get<target>(v);
}


TEST_F(FieldInstanceTest, variantContainer) {
    typedef boost::variant<BoolField &, IntField &, LongField &, FloatField &, StringField &, SecureBoolField &, SecureIntField &, SecureLongField &, SecureFloatField &, SecureStringField &> Instance;
    IntField *intField = new IntField(7);
    IntField *intField2 = new IntField(12);

    const Instance i( *intField);
    const Instance i2( *intField2);
    // index is 1 for IntField
   // const int idx = i.which();


    std::cout << "Index: " << i.which() << std::endl;
    // Does not work, can't get from idx to hardcoded
    //IntField *p = get<idx>(i);

    //std::cout << (get<1>(i)) << std::endl;
    //bool res = (i == i2);

}

template<typename T>
T add(const T & lhs, const T & rhs){
    return lhs + rhs;
}