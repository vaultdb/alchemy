#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include "field_instance.h"
#include "secure_bool_field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {

    // TF = derived field
    // TP = primitive / payload of field
    // B = boolean field result
    // RF = revealed field type / encrypted field type - e.g., BoolField has SecureBoolField and vice versa
    // RP = revealed primitive

    class BoolField : public FieldInstance<BoolField, bool, BoolField> {
    protected:
        bool payload = true;

    public:

        BoolField()   {}
        BoolField(const BoolField & src)  { payload = src.payload; }
        BoolField(const bool & src)  { payload = src; }
        BoolField(const int8_t * src)  {
            memcpy((int8_t *) &payload, src, size()/8);
        }

        // constructor for decryption
        BoolField(const emp::Bit & src, const int & party) {
            payload = src.reveal(party);
        }

        BoolField& operator=(const BoolField& other) {
            this->payload = other.payload;
            return *this;
        }


        bool encrypted() const { return false; }
        void copy(const BoolField & src) {payload = src.payload; }
        void assign(const bool & src) {payload = src; }

        static FieldType type() { return FieldType::BOOL; }

        size_t size() const override{ return 8; }

        /*bool decrypt(const int & party) const { return payload; } // do not invoke, FieldInstance should sidestep this in reveal()

        SecureBoolField & encrypt(const int & myParty, const int & dstParty = emp::PUBLIC) const {
            bool bit = (myParty == dstParty) ? payload : 0;
            emp::Bit encrypted(bit, dstParty);

            return *(new SecureBoolField(encrypted));
        }*/



        BoolField & operator+(const BoolField &rhs) const { return *(new BoolField(payload + rhs.payload)); }
        BoolField & operator-(const BoolField &rhs) const { return *(new BoolField(payload - rhs.payload)); }
        BoolField & operator*(const BoolField &rhs) const { return *(new BoolField(payload * rhs.payload)); }
        BoolField & operator/(const BoolField &rhs) const { return *(new BoolField(payload / rhs.payload)); }
        BoolField & operator%(const BoolField &rhs) const { return *(new BoolField(payload % rhs.payload)); }


        Field &  operator !() const override {
            return *(new BoolField(!payload));
        }

        BoolField & operator >= (const BoolField &cmp) const {
            return *(new BoolField(payload >= cmp.payload));
        }

        BoolField & operator == (const BoolField &cmp) const {
            return *(new BoolField(payload == cmp.payload));
        }




        bool primitive() const { return payload; }
        std::string str() const {
            return  payload ? "true" : "false";
        }

        // swappable
        BoolField & select(const BoolField & choice, const BoolField & other) const {
            bool selection = (bool) choice.payload;
            return selection ? *(new BoolField(*this)) : *(new BoolField(other));
        }


        void serialize(int8_t *dst) const override {
            size_t len  = size() / 8;
            std::memcpy(dst, (int8_t*) payload, len);
        }

        // bitwise ops
        BoolField & operator&(const BoolField &right) const {
            return *(new BoolField(payload & right.payload));
        }

        BoolField & operator^(const BoolField &right) const {
            return *(new BoolField(payload ^ right.payload));
        }

        BoolField & operator|(const BoolField &right) const {
            return *(new BoolField(payload | right.payload));
        }



    };

}
#endif //BOOL_FIELD_H
