#ifndef _FIELD_INSTANCE_IMPL_H
#define _FIELD_INSTANCE_IMPL_H

#include "field_instance.h"

namespace vaultdb {
    template<typename T, typename B>
    class FieldInstanceImpl : public FieldInstance<B> {
    public:

        FieldInstance<B> plus(const FieldInstance<B> &rhs) const override {
            return *static_cast<T *>(this) + *static_cast<T *>(&rhs);
        }

        FieldInstance<B> minus(const FieldInstance<B> &rhs) const override {
            return *static_cast<T *>(this) - *static_cast<T *>(&rhs);

        }

        FieldInstance<B> multiply(const FieldInstance<B> &rhs) const override {
            return FieldInstance<B>();
        }

        FieldInstance<B> div(const FieldInstance<B> &rhs) const override {
            return FieldInstance<B>();
        }

        FieldInstance<B> modulus(const FieldInstance<B> &rhs) const override {
            return FieldInstance<B>();
        }

        FieldInstance<B> select(const B &choice, const FieldInstance<B> &other) const override {
            return FieldInstance<B>();
        }

        FieldInstance<B> operator&(const FieldInstance<B> &right) const override {
            return FieldInstance<B>();
        }

        FieldInstance<B> operator^(const FieldInstance<B> &right) const override {
            return FieldInstance<B>();
        }

        FieldInstance<B> operator|(const FieldInstance<B> &right) const override {
            return FieldInstance<B>();
        }


        B geq(const FieldInstance<B> &rhs) const override {
            return nullptr;
        }

        B equal(const FieldInstance<B> &rhs) const override {
            return nullptr;
        }

        void serialize(int8_t *dst) const override {

        }

        B operator>=(const FieldInstance<B> & rhs) const override {

        }
        B operator==(const FieldInstance<B> & rhs) const override {

        }
        B operator!=(const FieldInstance<B> & rhs) const override {

        }

        B operator<(const FieldInstance<B> & rhs) const  override{

        }

        B operator<=(const FieldInstance<B> & rhs) const override {

        }

        B operator>(const FieldInstance<B> & rhs) const override {

        }

        string toString() const override {
            return static_cast<T*>(this)->str();
        }

    protected:
        FieldInstanceImpl() {}
        ~FieldInstanceImpl(){}

    };

}
#endif //_FIELD_INSTANCE_IMPL_H
