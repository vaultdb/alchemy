#ifndef _SECURE_FLOAT_FIELD_H
#define _SECURE_FLOAT_FIELD_H


#include "field_instance.h"
#include "bool_field.h"
#include <emp-tool/circuits/float32.h>

#include <cstdint>
#include <cstring>



namespace vaultdb {
    //  // T = derived field
    //    // P = primitive / payload of field, needed for serialize
    //    // B = boolean field result
    //    template<typename T, typename P, typename B>
    class SecureFloatField : public FieldInstance<SecureFloatField, emp::Float, SecureBoolField> {
    protected:
        emp::Float payload = emp::Float(0.0);

    public:

        SecureFloatField()   {}
        SecureFloatField(const SecureFloatField & src) { payload = src.payload; }
        SecureFloatField(const emp::Float & src)  { payload = src; }
        SecureFloatField(const int8_t * src)  {
            memcpy(payload.value.data(), (emp::Bit *) src, size()*sizeof(emp::Bit) );
        }

        SecureFloatField(const float_t & src, const int & myParty, const int & dstParty) {
            float_t toEncode = (myParty == dstParty) ? src : 0.0;
            payload = emp::Float(toEncode, dstParty);
        }

        SecureFloatField& operator=(const SecureFloatField& other) {
            this->payload = other.payload;
            return *this;
        }


        void copy(const SecureFloatField & src) { payload = src.payload; }
        void assign(const float_t & src) {payload = src; }

        static FieldType type() { return FieldType::SECURE_FLOAT32; }
        static FieldType revealedType() { return FieldType::FLOAT32; }

        size_t size() const override { return 32; }
        Field & encrypt(const int & myParty, const int & dstParty = emp::PUBLIC) const { return *(new SecureFloatField(*this)); }

        float_t decrypt(const int & party) const { return payload.reveal<float_t>(party); }




        SecureFloatField & operator+(const SecureFloatField &rhs) const { return *(new SecureFloatField(payload + rhs.payload)); }
        SecureFloatField & operator-(const SecureFloatField &rhs) const { return *(new SecureFloatField(payload - rhs.payload)); }
        SecureFloatField & operator*(const SecureFloatField &rhs) const { return *(new SecureFloatField(payload * rhs.payload)); }
        SecureFloatField & operator/(const SecureFloatField &rhs) const { return *(new SecureFloatField(payload / rhs.payload)); }
        SecureFloatField & operator%(const SecureFloatField &rhs) const { throw; }


        Field &  operator !() const override { throw; }

        SecureBoolField & operator >= (const SecureFloatField &cmp) const {
            emp::Bit lt = payload.less_than(cmp.payload);
            return *(new SecureBoolField(!lt)); // >=
        }

        SecureBoolField & operator == (const SecureFloatField &cmp) const {
            return *(new SecureBoolField(payload.equal(cmp.payload)));
        }


        bool encrypted() { return true; }

        emp::Float primitive() const { return payload; }
        std::string str() const { return "SECRET FLOAT"; }

        // swappable
        SecureFloatField & select(const SecureBoolField & choice, const SecureFloatField & other) const {
            emp::Bit selection = choice.primitive();
            emp::Float result = emp::If(selection, payload, other.payload);
            return *(new SecureFloatField(result));
        }

        void serialize(int8_t *dst) const override {
            size_t len  = size() / 8;
            std::memcpy(dst,  (int8_t *) &payload, len);
        }

        SecureFloatField & operator&(const SecureFloatField &right) const { throw; } // bitwise ops not defined for float
        SecureFloatField & operator^(const SecureFloatField &right) const { throw; } // bitwise ops not defined for float
        SecureFloatField & operator|(const SecureFloatField &right) const { throw; } // bitwise ops not defined for float




    };
}

#endif //_FLOAT_FIELD_H
