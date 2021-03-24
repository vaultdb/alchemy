#ifndef SECURE_STRING_FIELD_H
#define SECURE_STRING_FIELD_H


#include "field.h"
#include "secure_bool_field.h"
#include "string_field.h"

namespace vaultdb {

    // SecureStringField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureStringField : public Field<SecureBoolField> {

    public:

        SecureStringField() :  Field(FieldType::SECURE_INT) {}
        ~SecureStringField() = default;

        explicit SecureStringField(const Field &srcField);

        SecureStringField(const SecureStringField &src);

        SecureStringField(const emp::Integer &src);

        explicit SecureStringField(const StringField *src, const size_t &strLength, const int &myParty,
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
        SecureBoolField neg() const { throw; }
        std::string str() const { return "SECRET STRING"; }



        SecureBoolField operator>=(const SecureStringField &cmp) const;

        SecureBoolField operator==(const SecureStringField &cmp) const;


        // swappable
        SecureStringField selectValue(const SecureBoolField &choice, const SecureStringField &other) const;


        // bitwise ops
        SecureStringField operator&(const SecureStringField &right) const;

        SecureStringField operator^(const SecureStringField &right) const;

        SecureStringField operator|(const SecureStringField &right) const;

        void ser(int8_t * target) const;




    };

    std::ostream &operator<<(std::ostream &os, const SecureStringField &aValue);

}


#endif //SECURE_STRING_FIELD_H
