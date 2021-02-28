#ifndef _INTFIELD_H
#define _INTFIELD_H

#include "field_instance.h"
#include <cstdint>

#include <cstdint>
#include <cstring>



namespace vaultdb {
    //  // T = derived field
    //    // R = revealed field
    //    // B = boolean field result
    //    // P = primitive / payload of field, needed for serialize
    //    template<typename T, typename R, typename B, typename P>
    // TODO: make the third one BoolField instead of bool
    class IntField : public FieldInstance<IntField, IntField, bool, int32_t> {
    public:

        IntField() : FieldInstance<IntField, IntField, bool, int32_t>() {}
        IntField(const IntField & src) : FieldInstance<IntField, IntField, bool, int32_t>(src)
                { payload = src.payload; }
        IntField(const int32_t & src) : FieldInstance<IntField, IntField, bool, int32_t>() { payload = src; }

        IntField& operator=(const IntField& other) {
                this->payload = other.payload;
                return *this;
        }


        void copy(const IntField & src) {payload = src.payload; }
        void assign(const int32_t & src) {payload = src; }

        static FieldType type() { return FieldType::INT32; }
        size_t size() const { return 32; }

        Field *decrypt() const { return new IntField(*this); }


        static IntField deserialize(const int8_t *cursor) {
            IntField ret;
            memcpy((int8_t *) &ret.payload, cursor, ret.size()/8);
            return ret;

        }

        IntField & operator+(const IntField &rhs) const {
            IntField *result = new IntField(payload + rhs.payload);
            return *result;
        }

        IntField & operator >= (const IntField &cmp) const {
            return *(new IntField(payload >= cmp.payload));
        }


        /*
        // comparators
        bool geq(const IntField & cmp) { return payload >= cmp.payload; }
        bool equal(const IntField & cmp) { return payload == cmp.payload; }

        // arithmetic expressions
        IntField operator+(const IntField &rhs) const { return payload + rhs.payload; }
        IntField operator-(const IntField &rhs) const { return payload - rhs.payload; }
        IntField operator*(const IntField &rhs) const { return payload * rhs.payload; }
        IntField operator/(const IntField &rhs) const { return payload / rhs.payload; }
        IntField operator%(const IntField &rhs) const { return payload % rhs.payload; }

        // bitwise ops
        IntField operator&(const IntField &rhs) const { return payload & rhs.payload; }
        IntField operator|(const IntField &rhs) const { return payload | rhs.payload; }
        IntField operator^(const IntField &rhs) const { return payload ^ rhs.payload; }
        // flip the bits
        IntField operator!() const { return IntField(!payload); }


        // swappable
        IntField select(bool choice, const IntField & other) {
            if(choice) {
                return IntField(*this);
            }
            return IntField(other);
        }
*/

        int32_t primitive() const { return payload; }
        std::string str() const { return std::to_string(payload); }


    protected:
        int32_t payload = 0;



    };
}


#endif //_INTFIELD_H
