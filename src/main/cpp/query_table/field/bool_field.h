#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include <emp-tool/circuits/bit.h>
#include "field_instance_impl.h"

namespace vaultdb {


    class BoolField : public FieldInstanceImpl<BoolField, BoolField> {

    public:

        BoolField()  {}
        explicit BoolField(const FieldInstance<BoolField> & srcField);

        BoolField(const BoolField & src);

        explicit BoolField(const bool & src);
        explicit BoolField(const int8_t * src);

        FieldType getType() const { return FieldType::BOOL; }
        ~BoolField() = default;


        // constructor for decryption
        BoolField(const emp::Bit & src, const int & party);
     
        bool getPayload() const { return payload_; }

        // for use in print statements, etc.
        bool getBool() const { return payload_; }


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

        BoolField  operator == (const BoolField &cmp) const { return this->eq(cmp);   }
        BoolField  operator != (const BoolField &cmp) const { return this->eq(cmp).neg();   }


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
        void ser(int8_t * target) const { *target = payload_; }


    private:
        bool payload_ = false;


    };

    std::ostream &operator<<(std::ostream &os, const BoolField &aValue);

}
#endif //BOOL_FIELD_H
