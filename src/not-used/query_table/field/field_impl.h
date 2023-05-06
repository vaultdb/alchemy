#ifndef _FIELD_IMPL_H
#define _FIELD_IMPL_H

template<typename T, typename B>
class FieldImpl {

};
#endif //VAULTDB_EMP_FIELD_IMPL_H
#ifndef _FIELD_H
#define _FIELD_H

#include <cstdint>
#include <string>
#include <memory>
#include <boost/any.hpp>
#include "field_methods.h"

// Field becomes a wrapper for our impls
// We store the impl in a boost::any
// use template hacking to deduce type of impl

/*#include "int_field.h"
#include "long_field.h"
#include "string_field.h"
#include "float_field.h"
#include "secure_string_field.h"
#include "secure_int_field.h"
#include "secure_long_field.h"
#include "secure_float_field.h"*/
#include "bool_field.h"


#include "comparator.h"
// what if QueryTuple holds a boost::any instead of a discrete type?
// we can make field a generic class that stores a boost::any for its impl and casts it to a math/comparable as needed
namespace vaultdb {
    /*   typedef boost::variant<FieldInstance < BoolField, bool, BoolField, bool>,
       FieldInstance <IntField, int32_t, BoolField, bool>,
       FieldInstance <LongField, int64_t, BoolField, bool>,
       FieldInstance <FloatField, float_t, BoolField, bool>,
       FieldInstance <StringField, std::string, BoolField, bool>,
       FieldInstance <SecureBoolField, emp::Bit, SecureBoolField, emp::Bit>,
       FieldInstance <SecureIntField, emp::Integer, SecureBoolField, emp::Bit>,
       FieldInstance <SecureLongField, emp::Integer, SecureBoolField, emp::Bit>,
       FieldInstance <SecureFloatField, float_t, SecureBoolField, emp::Bit>,
       FieldInstance <SecureStringField, std::string, SecureBoolField, emp::Bit> > Field;

     typedef boost::variant<BoolField, IntField, LongField, FloatField, StringField, SecureBoolField, SecureIntField, SecureLongField, SecureFloatField, SecureStringField> Field;
   */
    // top-level class so we can store heterogeneous types in the same container
    // dummy methods, need to delegate down to make them work
    // TODO: overload the << operator
    // TODO: add nullability here
    class Field  {

    public:
        virtual ~Field() = default;


        Field &operator=(const Field &other) {
            this->instance = other.instance;

            return *this;
        }

        //virtual FieldInstance<class T,class P,class B> getInstance() = 0;

        virtual std::string
        toString() const {} // generic, will throw for things that need to be cast to child first for now!
        virtual void serialize(int8_t *dst) const { return boost::any_cast<FieldMethods>(instance).size(); }
    }


    virtual size_t size() const override {
    return
    boost::any_cast<FieldMethods>(instance).size();
}

virtual FieldType type() const {
    return
    return boost::any_cast<FieldMethods>(instance).type();
}


Field operator+(const Field &rhs) const {
    assert(getType() == rhs.getType());

}

// no static methods since this is a virtual function
Field *selectField(const Field &select, const Field &other) const override { // TODO: delegate
}


BoolField geq(const Field &rhs) const override {

return geqImpl<typename get_template_type<typename std::decay<decltype(payload)>::type>::type>(rhs);
}

template <typename T>
BoolField geqImpl(const Field & rhs) {
    assert(this->type() == rhs.type());
    Comparable<T> lhs = boost::any_cast<Comparable<T> >(payload);
    Comparable<T> rhs = boost::any_cast<Comparable<T> >(rhs.payload);

    return lhs.geq(rhs);
}

BoolField equal(const Field &rhs) const override { throw; }

BoolField operator!() const override { throw; }

// for casting it to subclass
template<typename T>
static T getInstance(const Field &f) { return static_cast<T &>(f); }


/*


 virtual void copyFrom(const Field & other) = 0;
 virtual Field & geq(const Field & rhs) const = 0;
 virtual Field & equal(const Field & rhs) const = 0;
 virtual bool isEncrypted() const = 0;

 virtual Field &  operator !() const = 0;*/

protected:

// Field class needs to be inherited
Field() = default;

Field(const Field &) = default;

Field(Field &&) = default;

boost::any instance; // anything that inherits from Comparable, Computable




};
/*std::ostream &operator<<(std::ostream &os, const Field &field) {
    return os << field.toString();
}

 template <typename C> struct get_template_type;
}*/

// from https://ideone.com/u8kCcU
template<typename C>
struct get_template_type;

template<template<typename> class C, typename T>
struct get_template_type<C<T>> {
    using type = T;
};
}

#endif //_FIELD_H
