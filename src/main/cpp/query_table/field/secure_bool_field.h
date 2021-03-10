#ifndef SECURE_BOOL_FIELD_H
#define SECURE_BOOL_FIELD_H

#include "field.h"
#include "field_instance.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {


    // T = derived field
    // B = boolean field result

    // SecureBoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureBoolField : public FieldInstance<SecureBoolField, SecureBoolField>, public Field  {

    public:

        SecureBoolField() = default;
        explicit SecureBoolField(const Field & srcField);

        SecureBoolField(const SecureBoolField & src);

        explicit SecureBoolField(const emp::Bit & src);

        explicit SecureBoolField(const int8_t * src);

        // constructor for encrypting a bit
        SecureBoolField(const Field *src, const int & myParty, const int & dstParty);
        ~SecureBoolField() = default;

        emp::Bit getPayload() const { return getValue<emp::Bit>(); }

        SecureBoolField& operator=(const SecureBoolField& other);

        SecureBoolField  operator+(const SecureBoolField &rhs) const  { throw; } // cast to int before doing arithmetic expressions
        SecureBoolField  operator-(const SecureBoolField &rhs) const { throw; }
        SecureBoolField  operator*(const SecureBoolField &rhs) const  { throw; }
        SecureBoolField  operator/(const SecureBoolField &rhs) const  { throw; }
        SecureBoolField  operator%(const SecureBoolField &rhs) const  { throw; }


        SecureBoolField negate() const { return SecureBoolField(!(getPayload())); }

        SecureBoolField  operator >= (const SecureBoolField &cmp) const;
        SecureBoolField  operator == (const SecureBoolField &cmp) const;


        // swappable
        SecureBoolField  select(const SecureBoolField & choice, const SecureBoolField & other) const;

        // bitwise ops
        SecureBoolField  operator&(const SecureBoolField &right) const { return  SecureBoolField(getPayload() & (right.getPayload())); }
        SecureBoolField  operator^(const SecureBoolField &right) const { return  SecureBoolField(getPayload() ^ (right.getPayload())); }
        SecureBoolField  operator|(const SecureBoolField &right) const { return  SecureBoolField(getPayload() | (right.getPayload())); }




    };






    std::ostream &operator<<(std::ostream &os, const SecureBoolField &aValue);

}
#endif //SECURE_BOOL_FIELD_H
