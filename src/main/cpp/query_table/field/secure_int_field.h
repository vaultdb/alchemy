#ifndef SECURE_INT_FIELD_H
#define SECURE_INT_FIELD_H

#include "field_instance.h"
#include "secure_bool_field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureIntField : public FieldInstance<SecureIntField, SecureBoolField>, public Field  {

    public:

        SecureIntField() = default;
        ~SecureIntField() = default;
        explicit SecureIntField(const Field & srcField);

        SecureIntField(const SecureIntField & src);
        SecureIntField(const emp::Integer & src);

        explicit SecureIntField(const Field *src, const int & myParty, const int & dstParty);
        explicit SecureIntField(const int8_t * src);


        emp::Integer getPayload() const;

        SecureIntField& operator=(const SecureIntField& other);


        SecureIntField  operator+(const SecureIntField &rhs) const  { return SecureIntField(getPayload() + rhs.getPayload());  } // cast to int before doing arithmetic expressions
        SecureIntField  operator-(const SecureIntField &rhs) const { return SecureIntField(getPayload() - rhs.getPayload()); }
        SecureIntField  operator*(const SecureIntField &rhs) const  { return SecureIntField(getPayload() * rhs.getPayload()); }
        SecureIntField  operator/(const SecureIntField &rhs) const  { return SecureIntField(getPayload() / rhs.getPayload()); }
        SecureIntField  operator%(const SecureIntField &rhs) const  { return SecureIntField(getPayload() % rhs.getPayload()); }


        // not defined in EMP
        SecureBoolField negate() const { throw; }



        SecureBoolField  operator >= (const SecureIntField &cmp) const;
        SecureBoolField  operator == (const SecureIntField &cmp) const;


        // swappable
        SecureIntField  select(const SecureBoolField & choice, const SecureIntField & other) const;



        // bitwise ops
        SecureIntField  operator&(const SecureIntField &right) const { return  SecureIntField((getPayload()) & right.getPayload()); }
        SecureIntField  operator^(const SecureIntField &right) const { return  SecureIntField((getPayload()) ^ (right.getPayload())); }
        SecureIntField  operator|(const SecureIntField &right) const { return  SecureIntField((getPayload()) | (right.getPayload())); }


    };

    std::ostream &operator<<(std::ostream &os, const SecureIntField &aValue);

}
#endif //BOOL_FIELD_H
