#ifndef _QUERYFIELD_H
#define _QUERYFIELD_H

// generic query field API for all field types, e.g., ints or floats
// analogous to emp's Comparable
// formerly known as Value
#include <cstring>
#include <defs.h>
#include "field.h"
//#include "field_factory.h"


namespace vaultdb {


    // T = derived field
    // P = primitive / payload of field
    // B = boolean field result
    template<typename T, typename P, typename B>
    class FieldInstance : public Field {
    public:


        std::unique_ptr<Field> clone() const override {
            return std::make_unique<T>(static_cast<T const &>(*this));
        }

        P getValue() const {
            return static_cast<T const &>(*this).primitive();
        }

        // TODO: figure out why these methods need to be in header file to compile
        //  may need inline?
        std::string toString() const override {
            return static_cast<T const &>(*this).str();
        }

        void serialize(int8_t *dst) const override {
            T impl =  static_cast<T const &>(*this);
            impl.serialize(dst);
        }

        // TODO: convert this to bytes instead of bits to reduce CPU time
         size_t size() const override {
            return static_cast<T const &>(*this).size();
        }






        FieldType getType() const override {
            return static_cast<T const &>(*this).type();
        }


        // assignment
        FieldInstance & operator=(const FieldInstance & other)  {
            copyTo(other);
            return *this;
        }

        Field & operator !() const override {
            return !(static_cast<T const &>(*this));
        }

        Field & operator+(const Field &rhs) const override {  return static_cast<T const &>(*this) + static_cast<const T &>(rhs); }
        Field & operator-(const Field &rhs) const override {   return static_cast<const T &>(*this) - static_cast<T const &>(rhs); }
        Field & operator*(const Field &rhs) const override {   return static_cast<const T &>(*this) * static_cast<T const &>(rhs); }
        Field & operator/(const Field &rhs) const override {   return static_cast<const T &>(*this) / static_cast<T const &>(rhs); }
        Field & operator%(const Field &rhs) const override {   return static_cast<const T &>(*this) % static_cast<T const &>(rhs); }


        Field & geq(const Field & rhs) const override { return static_cast<T const &>(*this) >= (static_cast<const T &>(rhs)); }

        Field & equal(const Field & rhs) const override { return static_cast<T const &>(*this) == (static_cast<const T &>(rhs)); }

        Field & compareAndSwap(const Field & select, const Field & other)  const override {
            const B choiceBit = static_cast<B const &> (select);
            const T otherOne = static_cast<T const &> (other);
            return  static_cast<T const &>(*this).select(choiceBit, otherOne);
        }

        // bitwise ops
        Field & operator&(const Field &right) const override { return  static_cast<const T&>(*this) &  static_cast<const T&>(right); }
        Field & operator|(const Field &right) const override { return  static_cast<const T&>(*this) |  static_cast<const T&>(right); }

        Field & operator^(const Field &right) const override { return  static_cast<const T&>(*this) ^  static_cast<const T&>(right); }



    protected:
        void copyTo(const Field & other) override {
            const T otherObj = static_cast<const T &>(other);
            static_cast<T &>(*this).copy(otherObj);
        }




    protected:
        // set up vtable entries
        FieldInstance() = default;

        FieldInstance(const FieldInstance &) = default;

        FieldInstance(FieldInstance &&) = default;
    };




}


#endif