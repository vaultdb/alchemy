#ifndef _FLOAT_INSTANCE_H
#define _FLOAT_INSTANCE_H

#include <query_table/field/float_field.h>

namespace vaultdb {
    class FloatInstance : public FieldInstance<BoolField> {
    public:
        FloatInstance(Field <BoolField> *src);
        ~FloatInstance() = default;

        FloatInstance & operator=(const FloatInstance & other);

        Field<BoolField> plus(const Field<BoolField> &rhs) const override;

        Field<BoolField> minus(const Field<BoolField> &rhs) const override;

        Field<BoolField> multiply(const Field<BoolField> &rhs) const override;

        Field<BoolField> div(const Field<BoolField> &rhs) const override;

        Field<BoolField> modulus(const Field<BoolField> &rhs) const override;

        Field<BoolField> select(const BoolField &choice, const Field<BoolField> &other) const override;

        Field<BoolField> operator&(const Field<BoolField> &right) const override;

        Field<BoolField> operator^(const Field<BoolField> &right) const override;

        Field<BoolField> operator|(const Field<BoolField> &right) const override;

        BoolField geq(const Field<BoolField> &rhs) const override;

        BoolField equal(const Field<BoolField> &rhs) const override;

        BoolField negate() const override;

        string str() const override;

        void serialize(int8_t *dst) const override;


    private:
        FloatField *ref;

    };
}

#endif //_INT_INSTANCE_H
