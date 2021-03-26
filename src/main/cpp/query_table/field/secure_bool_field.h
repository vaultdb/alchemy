#ifndef SECURE_BOOL_FIELD_H
#define SECURE_BOOL_FIELD_H

#include "bool_field.h"
#include <emp-tool/circuits/bit.h>
#include "field_instance_impl.h"


namespace vaultdb {


    // SecureBoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureBoolField : public FieldInstanceImpl<SecureBoolField, SecureBoolField>  {

    public:

        SecureBoolField()  {}
        ~SecureBoolField() = default;

        explicit SecureBoolField(const FieldInstanceImpl & srcField);

        SecureBoolField(const SecureBoolField & src);

        explicit SecureBoolField(const emp::Bit & src);

        explicit SecureBoolField(const int8_t * src);
        // encrypt from public
        explicit SecureBoolField(const bool & src);

        // constructor for encrypting a bit
        SecureBoolField(const BoolField *src, const int & myParty, const int & dstParty);



        emp::Bit getPayload() const { return payload; }

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

        SecureBoolField  operator!() const { return SecureBoolField(!payload);   }
        SecureBoolField  operator == (const bool &cmp) const { return SecureBoolField(getPayload() == emp::Bit(cmp));   }
        SecureBoolField  operator != (const bool &cmp) const { return !(*this == cmp);   }


        SecureBoolField  operator == (const SecureBoolField &cmp) const { return this->eq(cmp);   }
        SecureBoolField  operator != (const SecureBoolField &cmp) const { return this->eq(cmp).neg();   }


        // swappable
        SecureBoolField  selectValue(const SecureBoolField & choice, const SecureBoolField & other) const;

        // bitwise ops
        SecureBoolField  operator&(const SecureBoolField &right) const { return  SecureBoolField(getPayload() & (right.getPayload())); }
        SecureBoolField  operator^(const SecureBoolField &right) const { return  SecureBoolField(getPayload() ^ (right.getPayload())); }
        SecureBoolField  operator|(const SecureBoolField &right) const { return  SecureBoolField(getPayload() | (right.getPayload())); }

        SecureBoolField  operator&(const bool &right) const { return  SecureBoolField((getPayload()) & emp::Bit(right)); }
        SecureBoolField  operator^(const bool &right) const { return  SecureBoolField((getPayload()) ^ emp::Bit(right)); }
        SecureBoolField  operator|(const bool &right) const { return  SecureBoolField((getPayload()) | emp::Bit(right)); }


        void ser(int8_t * target) const;

    private:
        emp::Bit payload;

    };


    std::ostream &operator<<(std::ostream &os, const SecureBoolField &aValue);

}
#endif //SECURE_BOOL_FIELD_H
