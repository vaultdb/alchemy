#ifndef _SECURE_INT_INSTANCE_H
#define _SECURE_INT_INSTANCE_H

#include <query_table/field/secure_int_field.h>


namespace vaultdb {
    class SecureIntInstance : public FieldInstance<SecureBoolField> {
    public:
        SecureIntInstance(Field<SecureBoolField> *src);
        ~SecureIntInstance()= default;

        SecureIntInstance & operator=(const SecureIntInstance & other);

        Field<SecureBoolField> plus(const Field<SecureBoolField> &rhs) const override;

        Field<SecureBoolField> minus(const Field<SecureBoolField> &rhs) const override;

        Field<SecureBoolField> multiply(const Field<SecureBoolField> &rhs) const override;

        Field<SecureBoolField> div(const Field<SecureBoolField> &rhs) const override;

        Field<SecureBoolField> modulus(const Field<SecureBoolField> &rhs) const override;

        Field<SecureBoolField> select(const SecureBoolField &choice, const Field<SecureBoolField> &other) const override;

        Field<SecureBoolField> operator&(const Field<SecureBoolField> &right) const override;

        Field<SecureBoolField> operator^(const Field<SecureBoolField> &right) const override;

        Field<SecureBoolField> operator|(const Field<SecureBoolField> &right) const override;

        SecureBoolField geq(const Field<SecureBoolField> &rhs) const override;

        SecureBoolField equal(const Field<SecureBoolField> &rhs) const override;

        SecureBoolField negate() const override;

        string str() const override;

        void serialize(int8_t *dst) const override;


    private:
        SecureIntField *ref;

    };
}


#endif //VAULTDB_EMP_SECURE_INT_INSTANCE_H
