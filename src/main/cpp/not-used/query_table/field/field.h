#ifndef _FIELD_H_
#define _FIELD_H_

#include "field_type.h"

//#include "field_instance.h"

namespace vaultdb {
// forward declaration
    template<typename T, typename B>
    class FieldInstance;

// helper method from
// from https://www.fluentcpp.com/2017/05/19/crtp-helper/
 /*   template<typename T, typename B>
    struct downcast {
        FieldInstance<T, B> &impl() { return static_cast<FieldInstance<T, B> &>(*this); }

        FieldInstance<T, B> const &impl() const { return static_cast<FieldInstance<T, B> const &>(*this); }
    };*/

// template <typename T, typename B> class FieldInstance;

// this is a wrapper to FieldInstance<..> objects
// curiously recurring template pattern in FieldInstance
// Maybe merge this and FieldInstance?
// // template<typename T, typename B>
    class Field { //: public downcast<T, B> {
    public:

        Field() : type(FieldType::INVALID) {}

        template<typename T, typename B>
        Field(const FieldInstance<T, B> &src) {
            *this = FieldInstance<T,B>(src);
            type = src.getType();
        }

        template<typename T, typename B>
          std::unique_ptr<Field > clone() const  { return static_cast<FieldInstance<T, B> &>(*this).clone();
        }


       /* No pure virtual methods so we can instantiate it
        virtual std::string toString() const {
            return this->impl().toString();
        }

        virtual Field geq(const Field & rhs) const {
            return this->impl().geq(rhs);
        }

        virtual Field equal(const Field & rhs) const {
            return this->impl().equal(rhs);
        }

        virtual FieldType getType() const {
            return this->impl().getType();
        }

        virtual bool encrypted() const {
            return this->impl().encrypted();
        }
        virtual size_t size() const  {
            return this->impl().size();
        }

        virtual void serialize(int8_t *dst) const  { this->impl().serialize(dst); }

        virtual Field<T,B>  operator !() const {
            return !(this->impl());
        }

        virtual Field operator+(const Field<T,B> & rhs) const  {  return this->impl() + rhs.impl(); }
        virtual Field operator-(const Field & rhs) const { return this->impl() - rhs.impl(); }
        virtual Field operator/(const Field & rhs) const  { return this->impl() / rhs.impl(); }
        virtual Field operator*(const Field & rhs) const  { return this->impl() * rhs.impl(); }
        virtual Field operator%(const Field & rhs) const  { return this->impl() % rhs.impl(); }


        virtual Field operator^(const Field & rhs) const  { return this->impl() ^ rhs.impl(); }
        virtual Field operator|(const Field & rhs) const  { return this->impl() | rhs.impl(); }
        virtual Field operator&(const Field & rhs) const  { return this->impl() & rhs.impl(); }


        virtual Field compareAndSwap(const Field<B,B> & select, const Field<T,B> & other)  const {
            this->impl().compareAndSwap(select, other);
        }
*/
        template<typename T>
        T getInstance() const {
            return static_cast<T &>(*this);

        }

        FieldType  type;
/*        TypeId getImplType() {
            switch(type) {
                case FieldType::BOOL:
                    return typeid(BoolField);
            }
        }*/
    };
}
#endif
