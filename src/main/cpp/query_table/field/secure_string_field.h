#ifndef SECURE_STRING_FIELD_H
#define SECURE_STRING_FIELD_H



#include "field_instance.h"
#include "field.h"
#include "secure_bool_field.h"

namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureStringField : public FieldInstance<SecureStringField, SecureBoolField>, public Field {

    public:

        SecureStringField() :  Field(FieldType::SECURE_INT) {}
        ~SecureStringField() = default;

        explicit SecureStringField(const Field &srcField);

        SecureStringField(const SecureStringField &src);

        SecureStringField(const emp::Integer &src);

        explicit SecureStringField(const Field *src, const size_t &strLength, const int &myParty,
                                   const int &dstParty);

        explicit SecureStringField(const int8_t *src, const size_t & strLength);


        emp::Integer getPayload() const { return getValue<emp::Integer>(); }

        SecureStringField &operator=(const SecureStringField &other);


        SecureStringField operator+(const SecureStringField &rhs) const { return SecureStringField(getPayload() + rhs.getPayload()); }
        SecureStringField operator-(const SecureStringField &rhs) const { throw; }
        SecureStringField operator*(const SecureStringField &rhs) const { throw;  }
        SecureStringField operator/(const SecureStringField &rhs) const { throw; }
        SecureStringField operator%(const SecureStringField &rhs) const { throw;  }


        // not defined in EMP
        SecureBoolField negate() const { throw; }


        SecureBoolField operator>=(const SecureStringField &cmp) const;

        SecureBoolField operator==(const SecureStringField &cmp) const;


        // swappable
        SecureStringField select(const SecureBoolField &choice, const SecureStringField &other) const;


        // bitwise ops
        SecureStringField operator&(const SecureStringField &right) const;

        SecureStringField operator^(const SecureStringField &right) const;

        SecureStringField operator|(const SecureStringField &right) const;


    };

    std::ostream &operator<<(std::ostream &os, const SecureStringField &aValue);

}


#endif //SECURE_STRING_FIELD_H
