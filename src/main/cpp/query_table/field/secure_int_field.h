#ifndef _SECURE_INT_FIELD_H
#define _SECURE_INT_FIELD_H

#include "field_instance.h"
#include <cstdint>

#include <cstdint>
#include <cstring>
#include <emp-tool/circuits/integer.h>



namespace vaultdb {

    class IntField;

    //  // T = derived field
    //    // P =  primitive
    //    // B = boolean field result
    //    template<typename T, typename P, typename B>
class SecureIntField : public FieldInstance<SecureIntField, emp::Integer, SecureBoolField> {
    protected:
        emp::Integer payload = emp::Integer(32, 0);

    public:

        SecureIntField() {}
        SecureIntField(const SecureIntField & src)  { payload = src.payload; }
        SecureIntField(const emp::Integer & src) { payload = src; }
        SecureIntField(const int8_t * src)
                { memcpy((int8_t *) payload.bits.data(), src, size()/8); }

        SecureIntField(const int32_t & src, const int & myParty, const int & dstParty) {
            int32_t toEncrypt = (myParty == dstParty) ? src : 0;
            payload = emp::Integer(32, toEncrypt, dstParty);
        }

        SecureIntField& operator=(const SecureIntField& other) {
                this->payload = other.payload;
                return *this;
        }

        bool encrypted() { return true; }
        static FieldType type() { return FieldType::SECURE_INT32; }

        size_t size() const override { return 32; }
        void copy(const SecureIntField & src) { payload = src.payload; }
        void assign(const emp::Integer & src) {payload = src; }
        int32_t decrypt(const int & party) const { return payload.reveal<int32_t>(party); }
        emp::Integer primitive() const { return payload; }
        std::string str() const { return "SECRET INT"; }


        SecureIntField & operator+(const SecureIntField &rhs) const { return *(new SecureIntField(payload + rhs.payload)); }
        SecureIntField & operator-(const SecureIntField &rhs) const { return *(new SecureIntField(payload - rhs.payload)); }
        SecureIntField & operator*(const SecureIntField &rhs) const { return *(new SecureIntField(payload * rhs.payload)); }
        SecureIntField & operator/(const SecureIntField &rhs) const { return *(new SecureIntField(payload / rhs.payload)); }
        SecureIntField & operator%(const SecureIntField &rhs) const { return *(new SecureIntField(payload % rhs.payload)); }


        // comparators
        Field &  operator !() const override { throw; } // TODO: implement NOT'ing the bits
        SecureBoolField & operator >= (const SecureIntField &cmp) const { return *(new SecureBoolField(payload >= cmp.payload)); }
        SecureBoolField & operator == (const SecureIntField &cmp) const { return *(new SecureBoolField(payload == cmp.payload)); }


        // swappable
        SecureIntField & select(const SecureBoolField & choice, const SecureIntField & other) const {
            emp::Bit selection = choice.primitive();
            emp::Integer result = emp::If(selection, payload, other.payload);
            return *(new SecureIntField(result));
        }

        void serialize(int8_t *dst) const override {
            std::memcpy(dst, payload.bits.data(), size() * sizeof(emp::Bit));
        }


        // bitwise ops
        SecureIntField & operator&(const SecureIntField &right) const {
            return *(new SecureIntField(payload & right.payload));
        }

        SecureIntField & operator^(const SecureIntField &right) const {
            return *(new SecureIntField(payload ^ right.payload));
        }

        SecureIntField & operator|(const SecureIntField &right) const {
            return *(new SecureIntField(payload | right.payload));
        }


    };
}


#endif //_SecureIntField_H
