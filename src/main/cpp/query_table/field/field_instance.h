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
    template<typename T, typename B>
    class FieldInstance {
    public:


        std::unique_ptr<FieldInstance > clone() const  {
            return std::make_unique<T>(static_cast<T const &>(*this));
        }


        // TODO: figure out why these methods need to be in header file to compile
        //  may need inline?
       virtual std::string toString() const  {
            return static_cast<T const &>(*this).str();
        }

        virtual void serialize(int8_t *dst) const  {
            T impl =  static_cast<T const &>(*this);
            impl.serialize(dst);
        }

        // TODO: convert this to bytes instead of bits to reduce CPU time
         size_t getSize() const  {
            return static_cast<T const &>(*this).size();
        }



         FieldType getType() const  {

            return static_cast<T const &>(*this).type();
        }


       virtual bool encrypted() const  {
            return static_cast<T const &>(*this).encrypted();
        }

        // assignment
        FieldInstance & operator=(const FieldInstance & other)  {
            copyTo(other);
            return *this;
        }



        virtual FieldInstance operator !() const  {
            return static_cast<T const &>(*this).negate();
        }

         FieldInstance operator+(const FieldInstance &rhs) const  {  return static_cast<T const &>(*this) + static_cast<const T &>(rhs); }
         FieldInstance operator-(const FieldInstance &rhs) const  {   return static_cast<const T &>(*this) - static_cast<T const &>(rhs); }
         FieldInstance operator*(const FieldInstance &rhs) const  {   return static_cast<const T &>(*this) * static_cast<T const &>(rhs); }
         FieldInstance operator/(const FieldInstance &rhs) const  {   return static_cast<const T &>(*this) / static_cast<T const &>(rhs); }
         FieldInstance operator%(const FieldInstance&rhs) const  {   return static_cast<const T &>(*this) % static_cast<T const &>(rhs); }


        virtual FieldInstance  geq(const FieldInstance & rhs) const  { return static_cast<T const &>(*this) >= (static_cast<const T &>(rhs)); }

        virtual FieldInstance equal(const FieldInstance & rhs) const  { return static_cast<T const &>(*this) == (static_cast<const T &>(rhs)); }

        FieldInstance compareAndSwap(const FieldInstance & select, const FieldInstance & other)  const  {
            const B choiceBit = static_cast<B const &> (select);
            const T otherOne = static_cast<T const &> (other);
            return  static_cast<T const &>(*this).select(choiceBit, otherOne);
        }

        // bitwise ops
        virtual FieldInstance operator&(const FieldInstance &right) const  { return  static_cast<const T&>(*this) &  static_cast<const T&>(right); }
        virtual FieldInstance operator|(const FieldInstance &right) const  { return  static_cast<const T&>(*this) |  static_cast<const T&>(right); }
        virtual FieldInstance operator^(const FieldInstance &right) const  { return  static_cast<const T&>(*this) ^  static_cast<const T&>(right); }



    protected:
        void copyTo(const FieldInstance & other)  {
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