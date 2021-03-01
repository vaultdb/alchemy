#ifndef _LONG_FIELD_H
#define _LONG_FIELD_H

#include <emp-tool/circuits/integer.h>

namespace  vaultdb {
//  // T = derived field
//    // P = primitive field
//    // B = boolean field result
//    template<typename T, typename R, typename B, typename P>
    class LongField : public FieldInstance<LongField, int64_t, BoolField> {
    protected:
        int64_t payload = 0;

    public:

        LongField()  {}

        LongField(const LongField &src)  { payload = src.payload; }

        LongField(const int64_t &src)  { payload = src; }

        LongField(const int8_t *src) {
            memcpy((int8_t *) &payload, src, size() / 8);
        }

        LongField(const emp::Integer & toEncrypt, const int & party) {
            payload =  toEncrypt.reveal<int64_t>(party);
        }

        LongField &operator=(const LongField &other) {
            this->payload = other.payload;
            return *this;
        }

        bool encrypted() const { return false; }
        static FieldType type() { return FieldType::INT64; }

        size_t size() const override { return 64; }

        void copy(const LongField &src) { payload = src.payload; }

        void assign(const int64_t &src) { payload = src; }


        int64_t decrypt(const int & party) const { return payload; }

        int64_t primitive() const { return payload; }

        std::string str() const { return std::to_string(payload); }


        LongField &operator+(const LongField &rhs) const { return *(new LongField(payload + rhs.payload)); }

        LongField &operator-(const LongField &rhs) const { return *(new LongField(payload - rhs.payload)); }

        LongField &operator*(const LongField &rhs) const { return *(new LongField(payload * rhs.payload)); }

        LongField &operator/(const LongField &rhs) const { return *(new LongField(payload / rhs.payload)); }

        LongField &operator%(const LongField &rhs) const { return *(new LongField(payload % rhs.payload)); }


        // comparators
        Field &operator!() const override { return *(new LongField(!payload)); }

        BoolField &operator>=(const LongField &cmp) const { return *(new BoolField(payload >= cmp.payload)); }

        BoolField &operator==(const LongField &cmp) const { return *(new BoolField(payload == cmp.payload)); }


        // swappable
        LongField &select(const BoolField &choice, const LongField &other) const {
            bool selection = choice.primitive();
            return selection ? *(new LongField(*this)) : *(new LongField(other));
        }

        void serialize(int8_t *dst) const override {
            size_t len  = size() / 8;
            std::memcpy(dst, (int8_t*) payload, len);
        }

        // bitwise ops
        LongField & operator&(const LongField &right) const {
            return *(new LongField(payload & right.payload));
        }

        LongField & operator^(const LongField &right) const {
            return *(new LongField(payload ^ right.payload));
        }

        LongField & operator|(const LongField &right) const {
            return *(new LongField(payload | right.payload));
        }


    };
}

#endif //_LONG_FIELD_H
