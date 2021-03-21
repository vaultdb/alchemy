#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include "field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {


    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class BoolField :  public Field<BoolField>  {

    public:

        BoolField() :  Field(FieldType::BOOL) {}
        explicit BoolField(const Field & srcField);

        BoolField(const BoolField & src);

        explicit BoolField(const bool & src);
        explicit BoolField(const int8_t * src);
        ~BoolField() = default;


        // constructor for decryption
        BoolField(const emp::Bit & src, const int & party);
     
        bool getPayload() const { return this->getValue<bool>(); }

        // for use in print statements, etc.
        bool getBool() const { return getPayload(); }

        BoolField& operator=(const BoolField& other);


        BoolField  operator+(const BoolField &rhs) const  { throw; } // cast to int before doing arithmetic expressions
        BoolField  operator-(const BoolField &rhs) const { throw; }
        BoolField  operator*(const BoolField &rhs) const  { throw; }
        BoolField  operator/(const BoolField &rhs) const  { throw; }
        BoolField  operator%(const BoolField &rhs) const  { throw; }


        BoolField neg() const;



        BoolField  eq(const BoolField & cmp) const;
        BoolField gteq(const BoolField & cmp) const;

        BoolField  operator == (const bool &cmp) const { return BoolField(getPayload() == cmp);   }
        BoolField  operator != (const bool &cmp) const { return BoolField(getPayload() != cmp);   }


        // swappable
        BoolField  selectValue(const BoolField & choice, const BoolField & other) const;

        std::string str() const {  return getPayload() ? "true" : "false"; }


        // bitwise ops
        BoolField  operator&(const BoolField &right) const { return  BoolField((getPayload()) & (right.getPayload())); }
        BoolField  operator^(const BoolField &right) const { return  BoolField((getPayload()) ^ (right.getPayload())); }
        BoolField  operator|(const BoolField &right) const { return  BoolField((getPayload()) | (right.getPayload())); }

        BoolField  operator&(const bool &right) const { return  BoolField((getPayload()) & right); }
        BoolField  operator^(const bool &right) const { return  BoolField((getPayload()) ^ right); }
        BoolField  operator|(const bool &right) const { return  BoolField((getPayload()) | right); }

        // serialize
        void ser(int8_t * target) const { *target = getPayload(); }



    };

    std::ostream &operator<<(std::ostream &os, const BoolField &aValue);

}
#endif //BOOL_FIELD_H
