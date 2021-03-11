#ifndef _QUERY_FIELD_H
#define _QUERY_FIELD_H

#include <cstring>
#include <defs.h>
#include "field.h"


namespace vaultdb {


    // T = derived field
    // P = primitive / payload of field
    // B = boolean field result
    template<typename T, typename B>
    class FieldInstance   {
    public:


        std::unique_ptr<FieldInstance > clone() const  {
            return std::make_unique<T>(static_cast<T const &>(*this));
        }



        // assignment
        FieldInstance & operator=(const FieldInstance & other)  {
            copyTo(other);
            return *this;
        }



        virtual B operator !() const  {
            return static_cast<T const &>(*this).negate();
        }

         T operator+(const T &rhs) const  {  return static_cast<T const &>(*this) + rhs; }
         T operator-(const T &rhs) const  {   return static_cast<const T &>(*this) - rhs; }
         T operator*(const T &rhs) const  {   return static_cast<const T &>(*this) * rhs; }
         T operator/(const T &rhs) const  {   return static_cast<const T &>(*this) / rhs; }
         T operator%(const T &rhs) const  {   return static_cast<const T &>(*this) % rhs; }


        virtual B  geq(const FieldInstance & rhs) const  { return static_cast<T const &>(*this) >= (static_cast<const T &>(rhs)); }
        virtual B equal(const FieldInstance & rhs) const  { return static_cast<T const &>(*this) == (static_cast<const T &>(rhs)); }

        // based on emp::comaprable
        virtual B operator>=(const FieldInstance & rhs) const { return static_cast<T const &>(*this).geq(static_cast<T const &>(rhs)); }
        virtual B operator==(const FieldInstance & rhs) const { return static_cast<T const &>(*this).equal(static_cast<T const &>(rhs)); }
        B operator<(const T& rhs) const { return !( (*static_cast<const T*>(this))>= rhs ); }
        B operator<=(const T& rhs) const { return rhs >= *static_cast<const T*>(this); }
        B operator>(const T& rhs) const { return !(rhs >= *static_cast<const T*>(this)); }
        B operator==(const T& rhs) const { return static_cast<const T*>(this)->equal(rhs); }
        B operator!=(const T& rhs) const { return !(*static_cast<const T*>(this) == rhs); }

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