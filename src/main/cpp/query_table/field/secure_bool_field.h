#ifndef SECURE_BOOL_FIELD_H
#define SECURE_BOOL_FIELD_H

#include "field.h"
#include "bool_field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {


    // SecureBoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureBoolField : public Field<SecureBoolField>  {

    public:

        SecureBoolField() :  Field(FieldType::SECURE_BOOL) {}
        explicit SecureBoolField(const Field & srcField);

        SecureBoolField(const SecureBoolField & src);

        explicit SecureBoolField(const emp::Bit & src);

        explicit SecureBoolField(const int8_t * src);
        // encrypt from public
        explicit SecureBoolField(const bool & src);

        // constructor for encrypting a bit
        SecureBoolField(const BoolField *src, const int & myParty, const int & dstParty);

        ~SecureBoolField() = default;

        emp::Bit getPayload() const { return getValue<emp::Bit>(); }

        // for use in print statements, etc.
        bool getBool() const { return true;  }

        std::string str() const { return "SECRET BOOL";  }

        SecureBoolField& operator=(const SecureBoolField& other);

        SecureBoolField  operator+(const SecureBoolField &rhs) const  { throw; } // cast to int before doing arithmetic expressions
        SecureBoolField  operator-(const SecureBoolField &rhs) const { throw; }
        SecureBoolField  operator*(const SecureBoolField &rhs) const  { throw; }
        SecureBoolField  operator/(const SecureBoolField &rhs) const  { throw; }
        SecureBoolField  operator%(const SecureBoolField &rhs) const  { throw; }


        SecureBoolField neg() const { return SecureBoolField(!(getPayload())); }

        SecureBoolField  gteq(const SecureBoolField &cmp) const;
        SecureBoolField  eq(const SecureBoolField &cmp) const;

        SecureBoolField  operator == (const bool &cmp) const { return SecureBoolField(getPayload() == emp::Bit(cmp));   }
        SecureBoolField  operator != (const bool &cmp) const { return !(*this == cmp);   }



        // swappable
        SecureBoolField  selectValue(const SecureBoolField & choice, const SecureBoolField & other) const;

        // bitwise ops
        SecureBoolField  operator&(const SecureBoolField &right) const { return  SecureBoolField(getPayload() & (right.getPayload())); }
        SecureBoolField  operator^(const SecureBoolField &right) const { return  SecureBoolField(getPayload() ^ (right.getPayload())); }
        SecureBoolField  operator|(const SecureBoolField &right) const { return  SecureBoolField(getPayload() | (right.getPayload())); }

        void ser(int8_t * target) const;


    };


    std::ostream &operator<<(std::ostream &os, const SecureBoolField &aValue);

}
#endif //SECURE_BOOL_FIELD_H
