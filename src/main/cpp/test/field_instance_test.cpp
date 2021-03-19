#include <gtest/gtest.h>
#include <stdexcept>

#include <typeinfo>
#include <query_table/field/field.h>
#include <query_table/field/field_impl.h>
#include <query_table/field/int_field.h>
#include <query_table/field/bool_field.h>
#include <query_table/field/secure_int_field.h>
#include <query_table/field/secure_bool_field.h>
#include <emp-tool/emp-tool.h>
#include <query_table/field/field_factory.h>

#include <boost/variant2/variant.hpp>

using namespace vaultdb;
using namespace boost::variant2;


// for variant test
//#include <tuple>
//#include <boost/variant.hpp>






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
    SecureBoolField b(true);
    BoolField c(false);
    SecureBoolField d(false);

    vector<Field<BoolField> > fields;
    fields.reserve(4);
    fields.push_back(BoolField(true));
    fields.push_back(IntField(7));
    fields.push_back(BoolField(false));
    fields.push_back(IntField(42));

    ASSERT_EQ(fields[0].getType(), FieldType::BOOL);
    ASSERT_EQ(fields[1].getType(), FieldType::INT);

    ASSERT_EQ(fields[0].getValue<bool>(), true);
    ASSERT_EQ(fields[2].getValue<bool>(), false);

    ASSERT_EQ(fields[1].getValue<int32_t>(), 7);
    ASSERT_EQ(fields[3].getValue<int32_t>(), 42);


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
/*template<int N, typename... Ts> using NthTypeOf =
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
}*/


TEST_F(FieldInstanceTest, variantContainer) {
    typedef variant<BoolField *,
                    IntField *,
                    LongField *,
                    FloatField *,
                    StringField *,
                    SecureBoolField *,
                    SecureIntField *,
                    SecureLongField *,
                    SecureFloatField *,
                    SecureStringField *> Instance;
    IntField *intField = new IntField(7);
    IntField *intField2 = new IntField(12);

    const Instance i( intField);
    const Instance i2(intField2);
    // index is 1 for IntField
    //const int idx = i.index();


    std::cout << "Index: " << i.index() << std::endl;
    // Does not work, can't get from idx to hardcoded
   // IntField *p = get<idx>(i);

    //std::cout << (get<1>(i)) << std::endl;
    //bool res = (i == i2);

}




template<typename T> struct identity { typedef T type; };

template<typename T> void good(T *x, typename identity<T>::type value = 1) {
    std::cout << typeid(x).name() << std::endl;

    std::cout << typeid(value).name() << std::endl;
}


template<typename T>
T add(const T & lhs, const T & rhs){
    return lhs + rhs;
}


/* https://stackoverflow.com/questions/38773865/is-it-possible-to-store-the-type-of-an-object-in-a-map-for-casting-purposes
template<class...Ts>
struct types { using type=types; };

template<class T>struct tag_t{constexpr tag_t(){}; using type=T;};
template<class T>constexpr tag_t<T> tag{};

template<class T, class U>
T* tag_dynamic_cast( tag_t<T>, U* u ) {
    return dynamic_cast<T*>(u);
}
template<class T, class U>
T& tag_dynamic_cast( tag_t<T>, U& u ) {
    return dynamic_cast<T&>(u);
}

template<class F, class Sig>
struct invoker;
template<class F, class R, class...Args>
struct invoker<F, R(Args...)> {
    R(*)(void*, Args...) operator()() const {
        return [](void* ptr, Args...args)->R {
            return (*static_cast<F*>(ptr))(std::forward<Args>(args)...);
        };
    }
};
template<class F, class...Sigs>
std::tuple<Sigs*...> invokers() {
    return std::make_tuple( invoker<F, Sigs>{}()... );
}


TEST_F(FieldInstanceTest, deducedType) {

    IntField intField(5);
    IntField intField2(7);
    Field *f = &intField;

    std::map<std::string , types<BoolField, IntField> > myMap =
                                                             {{"bool" ,tag<BoolField>}, {"int", tag<IntField>}};
    myMap["int"]( [&](auto&& tag) {
        auto* dfoo = tag_dynamic_cast(tag, f);
    } );

   // std::map<FieldType, auto> decltypeMap;

   // std::map<std::string , types<BoolField, IntField> > myMap =
   //         { {"BoolField", tag<BoolField>}, {"IntField",tag<IntField>}};

    good(f, intField);

    using type1 = decltype(IntField());
    using type2 = decltype(intField);

    if(std::is_same<type1,type2>{}) {
        std::cout << "Same type!" << std::endl;
    }

    using type3 = decltype(f);

    if(!std::is_same<type1,type3>{}) {
        std::cout << "Parent type is different!" << std::endl;
    }

    type2 toImpl = static_cast<type2 &>(*f);

   // type2 res = add(*f, intField2);
}
*/


// https://ideone.com/u8kCcU

TEST_F(FieldInstanceTest, template_name) {

}

// What if we create a map to a function pointer that casts to the right type and applies functors?
// similar to: https://stackoverflow.com/questions/38773865/is-it-possible-to-store-the-type-of-an-object-in-a-map-for-casting-purposes
