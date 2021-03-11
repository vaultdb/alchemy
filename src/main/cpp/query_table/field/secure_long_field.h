#ifndef _SECURE_LONG_FIELD_H
#define _SECURE_LONG_FIELD_H

#include "field_instance.h"
#include "field.h"
#include "secure_bool_field.h"

namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureLongField : public FieldInstance<SecureLongField, SecureBoolField>, public Field  {

    public:

        SecureLongField() = default;
        ~SecureLongField() = default;

        explicit SecureLongField(const Field &srcField);

        SecureLongField(const SecureLongField &src);

        SecureLongField(const emp::Integer &src);

        explicit SecureLongField(const Field *src, const int &myParty, const int &dstParty);

        explicit SecureLongField(const int8_t *src);




        emp::Integer getPayload() const;

        SecureLongField &operator=(const SecureLongField &other);


        SecureLongField operator+(const SecureLongField &rhs) const {
            return SecureLongField(getPayload() + rhs.getPayload());
        } // cast to int before doing arithmetic expressions
        SecureLongField operator-(const SecureLongField &rhs) const {
            return SecureLongField(getPayload() - rhs.getPayload());
        }

        SecureLongField operator*(const SecureLongField &rhs) const {
            return SecureLongField(getPayload() * rhs.getPayload());
        }

        SecureLongField operator/(const SecureLongField &rhs) const {
            return SecureLongField(getPayload() / rhs.getPayload());
        }

        SecureLongField operator%(const SecureLongField &rhs) const {
            return SecureLongField(getPayload() % rhs.getPayload());
        }


        // not defined in EMP
        SecureBoolField negate() const { throw; }


        SecureBoolField operator>=(const SecureLongField &cmp) const;

        SecureBoolField operator==(const SecureLongField &cmp) const;


        // swappable
        SecureLongField select(const SecureBoolField &choice, const SecureLongField &other) const;


        // bitwise ops
        SecureLongField operator&(const SecureLongField &right) const;

        SecureLongField operator^(const SecureLongField &right) const;

        SecureLongField operator|(const SecureLongField &right) const;


    };

    std::ostream &operator<<(std::ostream &os, const SecureLongField &aValue);

}

#endif //_SECURE_LONG_FIELD_H
