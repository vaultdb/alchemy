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
#include <boost/tuple/tuple.hpp>
#include<boost/tuple/tuple_io.hpp>
#include<boost/tuple/tuple_comparison.hpp>
//#include <boost/fusion/algorithm/iteration/for_each.hpp>
//#include <boost/fusion/include/for_each.hpp>

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
    /*
    PlainBoolField starter(true);
    FieldInstance<PlainBoolField, PlainBoolField> exampleImpl = starter;
    Field field(exampleImpl);

    Field aField(BoolField(true));
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
    delete geq;*/


}

// from https://stackoverflow.com/questions/2709315/is-boost-tuple-compatible-with-c0x-variadic-templates
template <typename... Args>
auto construct_tuple(Args... args) -> decltype(boost::make_tuple(args...))
{
    return {args...};
}



template <typename... Args>
auto construct_list(Args... args) -> decltype(boost::mpl::list<Args...>())
{
    return {};
}

struct value_printer
{
    template< typename U > void operator()(U x)
    {
        std::cout << ", " << x;
    }
};

struct print
{
    template <typename T>
    void operator()( const T &t) const
    {
        std::cout << t << '\n';
    }
};


// from: https://www.petekeen.net/iterate-boost-tuple-elements

struct print_element
{
    template<typename T>
    void operator()(const T & t, const int index)
    {
        std::cout << index << ": " << t.toString() << std::endl;
    }
};

template<typename tuple_type, typename F, int Index, int Max>
struct foreach_tuple_impl {
    void operator()(tuple_type & t, F f) {
        f(boost::get<Index>(t), Index);
        foreach_tuple_impl<tuple_type, F, Index + 1, Max>()(t, f);
    }
};

template<typename tuple_type, typename F>
void foreach_tuple_element(tuple_type & t, F f)
{
    foreach_tuple_impl<
            tuple_type,
            F,
            0,
            boost::tuples::length<tuple_type>::value - 1
    >()(t, f);
}

template<typename tuple_type, typename F, int Max>
struct foreach_tuple_impl<tuple_type, F, Max, Max> {
    void operator()(tuple_type & t, F f) {
        f(boost::get<Max>(t), Max);
    }
};


TEST_F(FieldInstanceTest, MultiTypeContainer) {

    // experiment with using boost::tuple to store multiple field types


    BoolField a(true);
    SecureBoolField b(false);
    SecureBoolField c(true);
    BoolField d(false);
    boost::tuple aTuple = construct_tuple(a, b, c, d);


    const int tupleLength = boost::tuples::length<decltype(aTuple)>::value;
    std::cout << "Tuple length: " << tupleLength<< " elements: ("
    << boost::get<0>(aTuple).toString()
    << ", " <<  boost::get<1>(aTuple).toString()
    << ", " << boost::get<2>(aTuple).toString()
    << ", " << boost::get<3>(aTuple).toString()
     << ")" << std::endl;


    boost::mpl::for_each< boost::mpl::range_c<int,0,tupleLength> >(print());

    std::cout << "Recursive solution:" << std::endl;

    foreach_tuple_element( aTuple, print_element() );

    /*for(int i = 0; i < tupleLength; ++i) {
        const int j = i;
       // print(boost::get<j>(aTuple));
    }*/

    auto myList = construct_list(a, b, c, d);
   // boost::mpl::for_each()
    //for(auto pos = aTuple.get_head(); pos != aTuple.get_tail(); ++pos) {

    //}
  //  boost::fusion::for_each(aTuple, print{});
    //std::cout << aTuple << std::endl;

/*    for(int i = 1; i < tupleLength; ++i) {
        boost::get<i>(aTuple);
        std::cout << ", " << boost::get<i>(aTuple).toString();
    }*/



    /*std::vector<Field *> myFields;

  myFields.emplace_back(new IntField(7));
    myFields.emplace_back(new BoolField(true));


    ASSERT_EQ(myFields.size(), 2);
    for(auto field : myFields) {
        std::cout << field->toString() << std::endl;
        delete field;
    }

    myFields.clear();*/
}






TEST_F(FieldInstanceTest, encryptTest) {
 /*   Field *boolField = new BoolField(true);
    Field *intField = new IntField(7);

    Field * encryptedBool = EmpManager::secretShareField(boolField, emp::ALICE, emp::ALICE);
    bool revealed = ((SecureBoolField *) encryptedBool)->decrypt(emp::PUBLIC);
    ASSERT_EQ(true, revealed);

   Field * encryptedInt = EmpManager::secretShareField(intField, emp::ALICE, emp::ALICE);
   int32_t revealedInt = ((SecureIntField *) encryptedInt)->decrypt(emp::PUBLIC);
   ASSERT_EQ(7, revealedInt);

    delete boolField;
   delete intField;
    delete encryptedBool;
    delete encryptedInt;*/

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

TEST_F(FieldInstanceTest, downcastTest) {
   /* Field *aField = new IntField(7);
    IntField intField = *((IntField *)aField);
    intField.assign(8);

    // we can sidestep all of these pass-by-reference challenges if we can instantiate FieldInstance
    auto myField =  aField->getInstance();

    std::cout << myField.getValue() << std::endl;
    //template<class T, class P, class B> FieldInstance<T, P,B> f = ;
    //FieldInstance *instance = (FieldInstance<);
    */
}