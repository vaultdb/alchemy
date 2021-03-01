#ifndef SECURE_BOOL_FIELD_H
#define SECURE_BOOL_FIELD_H

#include "field_instance.h"
#include <emp-tool/circuits/bit.h>
#include "bool_field.h"
#include "field.h"


namespace vaultdb {

    // TF = derived field
    // TP = primitive / payload of field
    // B = boolean field result
    // RP = revealed primitive  / encrypted field type - e.g., BoolField has SecureBoolField and vice versa


class SecureBoolField : public FieldInstance<SecureBoolField, emp::Bit, SecureBoolField> {
    protected:
        emp::Bit payload = emp::Bit(false, emp::PUBLIC);

    public:

        SecureBoolField()  {}
        SecureBoolField(const SecureBoolField & src)  { payload = src.payload; }
        SecureBoolField(const emp::Bit & src)  { payload = src; }
        SecureBoolField(const int8_t * src) {
            memcpy((int8_t *) &payload, src, size()/8);
        }

        // constructor for encrypting a bit
        SecureBoolField(const bool & src, const int & myParty, const int & dstParty) {
            bool bit = (myParty == dstParty) ? src : 0;
            payload = emp::Bit(bit, dstParty);
        }


        SecureBoolField& operator=(const SecureBoolField& other) {
            this->payload = other.payload;
            return *this;
        }


        void copy(const SecureBoolField & src) {payload = src.payload; }
        void assign(const bool & src) {payload = src; }

        bool encrypted() const  { return true; }

        static FieldType type() { return FieldType::SECURE_BOOL; }
        static FieldType revealedType() { return FieldType::BOOL; }

    size_t size() const override{ return 8; }

        bool decrypt(const int & party) const {
            return  payload.reveal(party);
        }

       /* Field & encrypt(const int & myParty, const int & dstParty = emp::PUBLIC) const { return *(new SecureBoolField(*this)); } */





        SecureBoolField & operator+(const SecureBoolField &rhs) const { throw; } // semantics not defined
        SecureBoolField & operator-(const SecureBoolField &rhs) const { throw;  }
        SecureBoolField & operator*(const SecureBoolField &rhs) const { throw;  }
        SecureBoolField & operator/(const SecureBoolField &rhs) const { throw;  }
        SecureBoolField & operator%(const SecureBoolField &rhs) const { throw; }


        Field &  operator !() const override {
            return *(new SecureBoolField(!payload));
        }

        SecureBoolField & operator >= (const SecureBoolField &cmp) const {
            emp::Bit lhsVal = payload;
            emp::Bit rhsVal = cmp.payload;
            emp::Bit gt = (lhsVal == emp::Bit(true)) & (rhsVal == emp::Bit(false));
            emp::Bit eq = lhsVal == rhsVal;

            return *(new SecureBoolField(eq | gt));
        }

        SecureBoolField & operator == (const SecureBoolField &cmp) const {
            return *(new SecureBoolField(payload == cmp.payload));
        }




        emp::Bit primitive() const { return payload; }
        std::string str() const {
            return  "SECRET BIT";
        }

        // swappable
        SecureBoolField & select(const SecureBoolField & choice, const SecureBoolField & other) const {
            emp::Bit selection =  choice.payload;
            emp::Bit result =  emp::If(selection, payload, other.payload);
            return *(new SecureBoolField(result));
        }


    void serialize(int8_t *dst) const override {
            memcpy(dst, (int8_t *) &(payload.bit), size()/8);
        }

        // bitwise ops
        SecureBoolField & operator&(const SecureBoolField &right) const {
            return *(new SecureBoolField(payload & right.payload));
        }

        SecureBoolField & operator^(const SecureBoolField &right) const {
            return *(new SecureBoolField(payload ^ right.payload));
        }

        SecureBoolField & operator|(const SecureBoolField &right) const {
            return *(new SecureBoolField(payload | right.payload));
        }



    };

}
#endif //BOOL_FIELD_H
