#ifndef _INTFIELD_H
#define _INTFIELD_H

#include "field_instance.h"
#include "bool_field.h"
#include <cstdint>

#include <cstdint>
#include <cstring>
#include <emp-tool/circuits/integer.h>



namespace vaultdb {
    //     T = derived field
    //     P = primitive field
    //     B = boolean field result
    //    template<typename T, typename P, typename B>
    class IntField : public FieldInstance<IntField, int32_t, BoolField> {
    protected:
        int32_t payload = 0;

    public:

        IntField()  {}
        IntField(const IntField & src)  { payload = src.payload; }
        IntField(const int32_t & src) { payload = src; }
        IntField(const int8_t * src) { memcpy((int8_t *) &payload, src, size()/8); }
        IntField(const emp::Integer & src, const int & party) { // for decryption
            payload = src.reveal<int32_t>(party);
        }

        IntField& operator=(const IntField& other) {
                this->payload = other.payload;
                return *this;
        }


        static FieldType type() { return FieldType::INT32; }

        bool encrypted() { return false; }
        size_t size() const override { return 32; }
        void copy(const IntField & src) { payload = src.payload; }
        void assign(const int32_t & src) {payload = src; }
        int32_t decrypt(const int & party) const { return payload; }

        int32_t primitive() const { return payload; }
        std::string str() const { return std::to_string(payload); }


        IntField & operator+(const IntField &rhs) const { return *(new IntField(payload + rhs.payload)); }
        IntField & operator-(const IntField &rhs) const { return *(new IntField(payload - rhs.payload)); }
        IntField & operator*(const IntField &rhs) const { return *(new IntField(payload * rhs.payload)); }
        IntField & operator/(const IntField &rhs) const { return *(new IntField(payload / rhs.payload)); }
        IntField & operator%(const IntField &rhs) const { return *(new IntField(payload % rhs.payload)); }


        // comparators
        Field &  operator !() const override { return *(new IntField(!payload)); }
        BoolField & operator >= (const IntField &cmp) const { return *(new BoolField(payload >= cmp.payload)); }
        BoolField & operator == (const IntField &cmp) const { return *(new BoolField(payload == cmp.payload)); }


        // swappable
        IntField & select(const BoolField & choice, const IntField & other) const {
            bool selection =  choice.primitive();
            return selection ? *(new IntField(*this)) : *(new IntField(other));
        }

        void serialize(int8_t *dst) const override {
            size_t len  = size() / 8;
            std::memcpy(dst, (int8_t*) payload, len);
        }

       /* Field & encrypt(const int & myParty, const int & dstParty = emp::PUBLIC) const {
            int32_t value = (myParty == dstParty) ? payload : 0;
            emp::Integer intVal(32, value, dstParty);

            return *(new SecureIntField(intVal));
        }*/

        // bitwise ops
        IntField & operator&(const IntField &right) const {
            return *(new IntField(payload & right.payload));
        }

        IntField & operator^(const IntField &right) const {
            return *(new IntField(payload ^ right.payload));
        }

        IntField & operator|(const IntField &right) const {
            return *(new IntField(payload | right.payload));
        }


    };
}


#endif //_INTFIELD_H
