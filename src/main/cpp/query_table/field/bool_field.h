#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include "field.h"
#include "field_instance.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class BoolField : public FieldInstance<BoolField, BoolField>, public Field {

    public:

        BoolField() = default;
        explicit BoolField(const Field & srcField);

        BoolField(const BoolField & src);

        explicit BoolField(const bool & src);
        explicit BoolField(const int8_t * src);
        ~BoolField() = default;

        // constructor for decryption
        BoolField(const emp::Bit & src, const int & party);


     
        bool getPayload() const { return this->getValue<bool>(); }

        BoolField& operator=(const BoolField& other);


        BoolField  operator+(const BoolField &rhs) const  { throw; } // cast to int before doing arithmetic expressions
        BoolField  operator-(const BoolField &rhs) const { throw; }
        BoolField  operator*(const BoolField &rhs) const  { throw; }
        BoolField  operator/(const BoolField &rhs) const  { throw; }
        BoolField  operator%(const BoolField &rhs) const  { throw; }


        BoolField negate() const { return BoolField(!(getPayload())); }



        BoolField  operator >= (const BoolField &cmp) const;
        BoolField  operator == (const BoolField &cmp) const;


        // swappable
        BoolField  select(const BoolField & choice, const BoolField & other) const;



        // bitwise ops
        BoolField  operator&(const BoolField &right) const { return  BoolField((getPayload()) & (right.getPayload())); }
        BoolField  operator^(const BoolField &right) const { return  BoolField((getPayload()) ^ (right.getPayload())); }
        BoolField  operator|(const BoolField &right) const { return  BoolField((getPayload()) | (right.getPayload())); }


    };

    std::ostream &operator<<(std::ostream &os, const BoolField &aValue);

}
#endif //BOOL_FIELD_H
