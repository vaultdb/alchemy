#ifndef SECURE_FLOAT_FIELD_H
#define SECURE_FLOAT_FIELD_H



#include "field_instance.h"
#include "field.h"
#include "secure_bool_field.h"

namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureFloatField : public FieldInstance<SecureFloatField, SecureBoolField>, public Field {

    public:

        SecureFloatField() = default;
        ~SecureFloatField() = default;

        explicit SecureFloatField(const Field &srcField);

        SecureFloatField(const SecureFloatField &src);

        SecureFloatField(const emp::Float &src);

        explicit SecureFloatField(const Field *src, const int &myParty, const int &dstParty);

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
        SecureFloatField negate() const { throw; }


        SecureBoolField operator>=(const SecureFloatField &cmp) const;

        SecureBoolField operator==(const SecureFloatField &cmp) const;


        // swappable
        SecureFloatField select(const SecureBoolField &choice, const SecureFloatField &other) const;


        // bitwise ops
        SecureFloatField operator&(const SecureFloatField &right) const;

        SecureFloatField operator^(const SecureFloatField &right) const;

        SecureFloatField operator|(const SecureFloatField &right) const;


    };

    std::ostream &operator<<(std::ostream &os, const SecureFloatField &aValue);

}


#endif //SECURE_FLOAT_FIELD_H
