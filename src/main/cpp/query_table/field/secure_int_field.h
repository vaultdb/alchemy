#ifndef SECURE_INT_FIELD_H
#define SECURE_INT_FIELD_H

#include "secure_bool_field.h"
#include "int_field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {


    // SecureIntField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureIntField : public Field<SecureBoolField>  {

    public:

        SecureIntField() : Field(FieldType::SECURE_INT) {}
        ~SecureIntField() = default;
        explicit SecureIntField(const Field & srcField);

        SecureIntField(const SecureIntField & src);
        SecureIntField(const emp::Integer & src);
        SecureIntField(const int32_t & src);

        explicit SecureIntField(const IntField *src, const int & myParty, const int & dstParty);
        explicit SecureIntField(const int8_t * src);


        emp::Integer getPayload() const;

        SecureIntField& operator=(const SecureIntField& other);


        SecureIntField  operator+(const SecureIntField &rhs) const  { return SecureIntField(getPayload() + rhs.getPayload());  } // cast to int before doing arithmetic expressions
        SecureIntField  operator-(const SecureIntField &rhs) const { return SecureIntField(getPayload() - rhs.getPayload()); }
        SecureIntField  operator*(const SecureIntField &rhs) const  { return SecureIntField(getPayload() * rhs.getPayload()); }
        SecureIntField  operator/(const SecureIntField &rhs) const  { return SecureIntField(getPayload() / rhs.getPayload()); }
        SecureIntField  operator%(const SecureIntField &rhs) const  { return SecureIntField(getPayload() % rhs.getPayload()); }


        // not defined in EMP
        SecureBoolField neg() const { throw; }
        std::string str() const { return "SECRET INT"; }




        SecureBoolField  operator >= (const SecureIntField &cmp) const;
        SecureBoolField  operator == (const SecureIntField &cmp) const;


        // swappable
        SecureIntField  selectValue(const SecureBoolField & choice, const SecureIntField & other) const;



        // bitwise ops
        SecureIntField  operator&(const SecureIntField &right) const { return  SecureIntField((getPayload()) & right.getPayload()); }
        SecureIntField  operator^(const SecureIntField &right) const { return  SecureIntField((getPayload()) ^ (right.getPayload())); }
        SecureIntField  operator|(const SecureIntField &right) const { return  SecureIntField((getPayload()) | (right.getPayload())); }

        void ser(int8_t * target) const;


    };

    std::ostream &operator<<(std::ostream &os, const SecureIntField &aValue);



}
#endif //BOOL_FIELD_H
