#ifndef _LONG_FIELD_H
#define _LONG_FIELD_H


#include "bool_field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {

    // LongField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class LongField : public Field<BoolField>  {

    public:

        LongField() :  Field(FieldType::LONG) {}
        ~LongField() = default;
        explicit LongField(const Field & srcField);

        LongField(const LongField & src);

        explicit LongField(const int64_t & src);
        explicit LongField(const int8_t * src);


        // constructor for decryption
        LongField(const emp::Integer & src, const int & party);


        int64_t getPayload() const { return getValue<int64_t>(); }

        LongField& operator=(const LongField& other);


        LongField  operator+(const LongField &rhs) const  { return LongField(getPayload() + rhs.getPayload()); } // cast to int before doing arithmetic expressions
        LongField  operator-(const LongField &rhs) const  { return LongField(getPayload() - rhs.getPayload()); }
        LongField  operator*(const LongField &rhs) const  { return LongField(getPayload() * rhs.getPayload()); }
        LongField  operator/(const LongField &rhs) const  { return LongField(getPayload() / rhs.getPayload()); }
        LongField  operator%(const LongField &rhs) const  { return LongField(getPayload() % rhs.getPayload()); }


        // only for bool types
        BoolField neg() const { throw; }
        std::string str() const { return std::to_string(getPayload());  }



        BoolField  operator >= (const LongField &cmp) const;
        BoolField  operator == (const LongField &cmp) const;


        // swappable
        LongField  selectValue(const BoolField & choice, const LongField & other) const;



        // bitwise ops
        LongField  operator&(const LongField &right) const { return  LongField(getPayload() & (right.getPayload())); }
        LongField  operator^(const LongField &right) const { return  LongField(getPayload() ^ (right.getPayload())); }
        LongField  operator|(const LongField &right) const { return  LongField(getPayload() | (right.getPayload())); }

        // serialize
        void ser(int8_t * target) const { *((int64_t *) target) = getPayload();  }


    };

    std::ostream &operator<<(std::ostream &os, const LongField &aValue);

}
#endif //VAULTDB_EMP_LONG_FIELD_H
