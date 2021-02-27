#ifndef _INTFIELD_H
#define _INTFIELD_H

#include "field_instance.h"
#include <cstdint>

#include <cstdint>
#include <cstring>



namespace vaultdb {
    class IntField : public FieldInstance<IntField, int32_t, int32_t, bool> {
    public:

        IntField() {}
        IntField(const IntField & src) { payload = src.payload; }
        IntField(const int32_t & src) { payload = src; }
        IntField& operator=(const IntField& other);


        void copy(const IntField & src) {payload = src.payload; }
        void assign(const int32_t & src) {payload = src; }

        static FieldType type() { return FieldType::INT32; }
        int32_t primitive() const { return payload; }
        int32_t decrypt() const { return payload; }
        std::string str() const { return std::to_string(payload); }
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
        size_t size() const { return 32; }

        void serialize(int8_t *dst) const;
        static IntField deserialize(const int8_t *cursor);


    protected:
        int32_t payload = 0;

    };
}


#endif //_INTFIELD_H
