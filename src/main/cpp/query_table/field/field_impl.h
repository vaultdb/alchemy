#ifndef _FIELD_IMPL_H
#define _FIELD_IMPL_H

#include <cstring>
#include <defs.h>

namespace vaultdb {

    template<typename B> class Field;

    // T = derived field
    // B = boolean field result
    template<typename T, typename B>
    class FieldImpl   {
    public:


        std::unique_ptr<FieldImpl > clone() const  {
            return std::make_unique<T>(static_cast<T const &>(*this));
        }



        // assignment
        FieldImpl & operator=(const FieldImpl & other)  {
            copyTo(other);
            return *this;
        }



        virtual B operator !() const  {
            return static_cast<T const &>(*this).neg();
        }

         Field<B> operator+(const Field<B> &rhs) const  {  return static_cast<T const &>(*this) + static_cast<T const &>(rhs); }
        Field<B> operator-(const Field<B> &rhs) const  {   return static_cast<const T &>(*this) - static_cast<T const &>(rhs); }
        Field<B> operator*(const Field<B> &rhs) const  {   return static_cast<const T &>(*this) * static_cast<T const &>(rhs); }
        Field<B> operator/(const Field<B> &rhs) const  {   return static_cast<const T &>(*this) / static_cast<T const &>(rhs); }
        Field<B> operator%(const Field<B> &rhs) const  {   return static_cast<const T &>(*this) % static_cast<T const &>(rhs); }


        virtual B  geq(const FieldImpl & rhs) const  { return static_cast<T const &>(*this) >= (static_cast<const T &>(rhs)); }
        virtual B equal(const FieldImpl & rhs) const  { return static_cast<T const &>(*this) == (static_cast<const T &>(rhs)); }

        // based on emp::comaprable
        virtual B operator>=(const Field<B> & rhs) const { return static_cast<T const &>(*this).geq(static_cast<T const &>(rhs)); }
        virtual B operator==(const Field<B> & rhs) const { return static_cast<T const &>(*this).equal(static_cast<T const &>(rhs)); }
        B operator<(const Field<B>& rhs) const { return !( (*static_cast<const T*>(this))>= rhs ); }
        B operator<=(const Field<B>& rhs) const { return rhs >= *static_cast<const T*>(this); }
        B operator>(const Field<B>& rhs) const { return !(rhs >= *static_cast<const T*>(this)); }

        FieldImpl compareAndSwap(const FieldImpl & select, const FieldImpl & other)  const  {
            const B choiceBit = static_cast<B const &> (select);
            const T otherOne = static_cast<T const &> (other);
            return  static_cast<T const &>(*this).select(choiceBit, otherOne);
        }

        static FieldImpl If(const B & select, const FieldImpl & lhs, const FieldImpl & rhs)    {
            const B choiceBit = static_cast<B const &> (select);
            const T l = static_cast<T const &> (lhs);
            const T r = static_cast<T const &> (rhs);
            return l.select(choiceBit, r);
        }

        // bitwise ops
        virtual FieldImpl operator&(const FieldImpl &right) const  { return static_cast<const T&>(*this) & static_cast<const T&>(right); }
        virtual FieldImpl operator|(const FieldImpl &right) const  { return static_cast<const T&>(*this) | static_cast<const T&>(right); }
        virtual FieldImpl operator^(const FieldImpl &right) const  { return static_cast<const T&>(*this) ^ static_cast<const T&>(right); }



    protected:
        void copyTo(const FieldImpl & other)  {
            const T otherObj = static_cast<const T &>(other);
            static_cast<T &>(*this).copy(otherObj);
        }




    protected:
        // set up vtable entries
        FieldImpl() = default;

        FieldImpl(const FieldImpl &) = default;

        FieldImpl(FieldImpl &&) = default;
    };




}


#endif


/*
 *
    // :  public FieldInstance<B>
    template<typename T, typename B>
    class FieldImpl {
    public:
        std::unique_ptr<FieldInstance<B> > clone() const  override {
            return std::make_unique<T>(static_cast<T const &>(*this));
        }


        // assignment
        FieldImpl & operator=(const FieldImpl & other)  {
            copyTo(other);
            return *this;
        }



        FieldInstance<B> operator+(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) + static_cast<T const &>(rhs);
        }

        FieldInstance<B> operator-(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) - static_cast<T const &>(rhs);
        }

        FieldInstance<B> operator*(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) * static_cast<T const &>(rhs);
        }

        FieldInstance<B> operator/(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) / static_cast<T const &>(rhs);
        }

        FieldInstance<B> operator%(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) % static_cast<T const &>(rhs);
        }

        FieldInstance<B> operator&(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) & static_cast<T const &>(rhs);
        }

        FieldInstance<B> operator^(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) ^ static_cast<T const &>(rhs);
        }

        FieldInstance<B> operator|(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) | static_cast<T const &>(rhs);
        }

        B geq(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) >= static_cast<T const &>(rhs);
        }

        B equal(const FieldInstance<B> &rhs) const override {
            return static_cast<T const &>(*this) == static_cast<T const &>(rhs);
        }

        B negate() const override {
            return static_cast<T const &>(*this).neg();
        }

        FieldInstance<B> select(const B & choice, const FieldInstance<B> & other) const override {
            return static_cast<T const &>(*this).selectValue(choice, other);
        }

    protected:
        // set up vtable entries
        FieldImpl() = default;
        FieldImpl(const FieldImpl &) = default;
        FieldImpl(FieldImpl &&) = default;

        void copyTo(const FieldImpl & other)  {
            const T otherObj = static_cast<const T &>(other);
            static_cast<T &>(*this).copy(otherObj);
        }



    };

 */