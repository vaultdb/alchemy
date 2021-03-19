#ifndef SECURE_FLOAT_FIELD_H
#define SECURE_FLOAT_FIELD_H



#include "field_impl.h"
#include "field.h"
#include "secure_bool_field.h"
#include "float_field.h"
#include "secure_int_field.h"
#include "secure_long_field.h"

namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureFloatField : public FieldImpl<SecureFloatField, SecureBoolField>, public Field<SecureBoolField> {

    public:

        SecureFloatField()  : Field(FieldType::SECURE_FLOAT) {}
        ~SecureFloatField() = default;

        explicit SecureFloatField(const Field &srcField);

        SecureFloatField(const SecureFloatField &src);

        SecureFloatField(const emp::Float &src);

        explicit SecureFloatField(const FloatField *src, const int &myParty, const int &dstParty);

        explicit SecureFloatField(const int8_t *src);



        emp::Float getPayload() const { return getValue<emp::Float>(); }

        SecureFloatField &operator=(const SecureFloatField &other);


        SecureFloatField operator+(const SecureFloatField &rhs) const; // cast to int before doing arithmetic expressions
        SecureFloatField operator-(const SecureFloatField &rhs) const {
            return SecureFloatField(getPayload() - rhs.getPayload());
        }

        SecureFloatField operator*(const SecureFloatField &rhs) const {
            return SecureFloatField(getPayload() * rhs.getPayload());
        }

        SecureFloatField operator/(const SecureFloatField &rhs) const {
            return SecureFloatField(getPayload() / rhs.getPayload());
        }

        SecureFloatField operator%(const SecureFloatField &rhs) const { throw; }

        // not defined in EMP
        SecureBoolField neg() const { throw; }


        SecureBoolField operator>=(const SecureFloatField &cmp) const;

        SecureBoolField operator==(const SecureFloatField &cmp) const;


        // swappable
        SecureFloatField selectValue(const SecureBoolField &choice, const SecureFloatField &other) const;


        // bitwise ops
        SecureFloatField operator&(const SecureFloatField &right) const;

        SecureFloatField operator^(const SecureFloatField &right) const;

        SecureFloatField operator|(const SecureFloatField &right) const;

        void ser(int8_t * target) const;

        static SecureFloatField toFloat(const SecureIntField & f);
        static SecureFloatField toFloat(const SecureLongField & f);

    private:
        static SecureFloatField toFloat(const emp::Integer & src);



    };

    std::ostream &operator<<(std::ostream &os, const SecureFloatField &aValue);

}


#endif //SECURE_FLOAT_FIELD_H
