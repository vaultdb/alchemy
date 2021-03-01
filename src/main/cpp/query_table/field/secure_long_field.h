#ifndef _SECURE_LONG_FIELD_H
#define _SECURE_LONG_FIELD_H

#include <emp-tool/circuits/integer.h>

namespace  vaultdb {
//  // T = derived field
//    // P = primitive / payload of field, needed for serialize
//    // B = boolean field result
//    template<typename T, typename P, typename B>
class SecureLongField : public FieldInstance<SecureLongField, emp::Integer, SecureBoolField> {
    protected:
        emp::Integer payload = emp::Integer(64, 0);

    public:

        SecureLongField() {}

        SecureLongField(const SecureLongField &src)
                { payload = src.payload; }

        SecureLongField(const emp::Integer &src)  { payload = src; }

        SecureLongField(const int8_t *src) {
            memcpy((int8_t *) &payload, src, size() * sizeof(emp::Bit));
        }

        SecureLongField(const int64_t & toEncrypt, const int & myParty, const int & dstParty) {
            int64_t value = (myParty == dstParty) ? toEncrypt : 0L;
            payload = emp::Integer(64, value, dstParty);
        }

        SecureLongField &operator=(const SecureLongField &other) {
            this->payload = other.payload;
            return *this;
        }


        bool encrypted() { return true; }
        static FieldType type() { return FieldType::SECURE_INT64; }


        size_t size() const override { return 64; }

        void copy(const SecureLongField &src) { payload = src.payload; }

        void assign(const emp::Integer &src) { payload = src; }


        int64_t decrypt(const int & party) const { return payload.reveal<int64_t>(party); }

        emp::Integer primitive() const { return payload; }

        std::string str() const { return "SECRET LONG"; }


        SecureLongField &operator+(const SecureLongField &rhs) const { return *(new SecureLongField(payload + rhs.payload)); }

        SecureLongField &operator-(const SecureLongField &rhs) const { return *(new SecureLongField(payload - rhs.payload)); }

        SecureLongField &operator*(const SecureLongField &rhs) const { return *(new SecureLongField(payload * rhs.payload)); }

        SecureLongField &operator/(const SecureLongField &rhs) const { return *(new SecureLongField(payload / rhs.payload)); }

        SecureLongField &operator%(const SecureLongField &rhs) const { return *(new SecureLongField(payload % rhs.payload)); }


        // comparators
        Field &operator!() const override { throw;  }

        SecureBoolField &operator>=(const SecureLongField &cmp) const { return *(new SecureBoolField(payload >= cmp.payload)); }

        SecureBoolField &operator==(const SecureLongField &cmp) const { return *(new SecureBoolField(payload == cmp.payload)); }


        // swappable
        SecureLongField &select(const SecureBoolField &choice, const SecureLongField &other) const {
            emp::Bit selection = choice.primitive();
            emp::Integer result = emp::If(selection, payload, other.payload);
            return  *(new SecureLongField(result));
        }

        void serialize(int8_t *dst) const override {
            std::memcpy(dst, payload.bits.data(), size() * sizeof(emp::Bit));
        }

        // bitwise ops
        SecureLongField & operator&(const SecureLongField &right) const {
            return *(new SecureLongField(payload & right.payload));
        }

        SecureLongField & operator^(const SecureLongField &right) const {
            return *(new SecureLongField(payload ^ right.payload));
        }

        SecureLongField & operator|(const SecureLongField &right) const {
            return *(new SecureLongField(payload | right.payload));
        }


    };
}

#endif //_LONG_FIELD_H
