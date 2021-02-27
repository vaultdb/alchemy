#ifndef _FLOAT_FIELD_H
#define _FLOAT_FIELD_H


#include "field_instance.h"
#include <cstdint>

#include <cstdint>
#include <cstring>

namespace  vaultdb {
    class FloatField {
        FloatField() {}

        FloatField(const FloatField &src) { payload = src.payload; }

        FloatField(const int32_t &src) { payload = src; }

        FloatField &operator=(const FloatField &other);


        void copy(const FloatField &src) { payload = src.payload; }

        void assign(const int32_t &src) { payload = src; }

        static FieldType type() { return FieldType::FLOAT32; }

        int32_t primitive() const { return payload; }

        int32_t decrypt() const { return payload; }

        std::string str() const { return std::to_string(payload); }

/*
        // comparators
        bool geq(const FloatField & cmp) { return payload >= cmp.payload; }
        bool equal(const FloatField & cmp) { return payload == cmp.payload; }

        // arithmetic expressions
        FloatField operator+(const FloatField &rhs) const { return payload + rhs.payload; }
        FloatField operator-(const FloatField &rhs) const { return payload - rhs.payload; }
        FloatField operator*(const FloatField &rhs) const { return payload * rhs.payload; }
        FloatField operator/(const FloatField &rhs) const { return payload / rhs.payload; }
        FloatField operator%(const FloatField &rhs) const { return payload % rhs.payload; }

        // bitwise ops
        FloatField operator&(const FloatField &rhs) const { return payload & rhs.payload; }
        FloatField operator|(const FloatField &rhs) const { return payload | rhs.payload; }
        FloatField operator^(const FloatField &rhs) const { return payload ^ rhs.payload; }
        // flip the bits
        FloatField operator!() const { return FloatField(!payload); }


        // swappable
        FloatField select(bool choice, const FloatField & other) {
            if(choice) {
                return FloatField(*this);
            }
            return FloatField(other);
        }
*/
        size_t size() const { return 32; }

        static FloatField deserialize(const int8_t *cursor);


    protected:
        int32_t payload = 0;
    };


}
#endif //_FLOAT_FIELD_H
