#ifndef INT_FIELD_H
#define INT_FIELD_H

#include "field_impl.h"
#include "bool_field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class IntField : public FieldImpl<IntField, BoolField>, public Field<BoolField> {

    public:

        IntField() :  Field(FieldType::INT) {}
        ~IntField() = default;
        explicit IntField(const Field & srcField);

        IntField(const IntField & src);

        explicit IntField(const int32_t & src);
        explicit IntField(const int8_t * src);


        // constructor for decryption
        IntField(const emp::Integer & src, const int & party);


        int32_t getPayload() const { return getValue<int32_t>(); }

        IntField& operator=(const IntField& other);
        IntField& operator=(const int32_t & other);


        IntField  operator+(const IntField &rhs) const  { return IntField(getPayload() + rhs.getPayload()); } // cast to int before doing arithmetic expressions
        IntField  operator-(const IntField &rhs) const  { return IntField(getPayload() - rhs.getPayload()); }
        IntField  operator*(const IntField &rhs) const  { return IntField(getPayload() * rhs.getPayload()); }
        IntField  operator/(const IntField &rhs) const  { return IntField(getPayload() / rhs.getPayload()); }
        IntField  operator%(const IntField &rhs) const  { return IntField(getPayload() % rhs.getPayload()); }


        // only for bool types
        BoolField neg() const { throw; }



        BoolField  operator >= (const IntField &cmp) const;
        BoolField  operator == (const IntField &cmp) const;


        // swappable
        IntField  selectValue(const BoolField & choice, const IntField & other) const;



        // bitwise ops
        IntField  operator&(const IntField &right) const { return  IntField(getPayload() & (right.getPayload())); }
        IntField  operator^(const IntField &right) const { return  IntField((getPayload() ^ (right.getPayload()))); }
        IntField  operator|(const IntField &right) const { return  IntField((getPayload() | (right.getPayload()))); }

        // serialize
        void ser(int8_t * target) const { *((int32_t *) target) = getPayload();  }


    };

    std::ostream &operator<<(std::ostream &os, const IntField &aValue);



}
#endif //INT_FIELD_H
