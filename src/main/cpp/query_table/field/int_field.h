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
    class IntField : public FieldInstance<IntField, IntField, IntField, int32_t> {
    protected:
        int32_t payload = 0;

    public:

        IntField() : FieldInstance<IntField, IntField, IntField, int32_t>() {}
        IntField(const IntField & src) : FieldInstance<IntField, IntField, IntField, int32_t>(src) { payload = src.payload; }
        IntField(const int32_t & src) : FieldInstance<IntField, IntField, IntField, int32_t>() { payload = src; }
        IntField(const int8_t * src)  : FieldInstance<IntField, IntField, IntField, int32_t>(){
            memcpy((int8_t *) &payload, src, size()/8);
        }

        IntField& operator=(const IntField& other) {
                this->payload = other.payload;
                return *this;
        }


        void copy(const IntField & src) {payload = src.payload; }
        void assign(const int32_t & src) {payload = src; }

        static FieldType type() { return FieldType::INT32; }
        size_t size() const { return 32; }

        Field *decrypt() const { return new IntField(*this); }




        IntField & operator+(const IntField &rhs) const { return *(new IntField(payload + rhs.payload)); }
        IntField & operator-(const IntField &rhs) const { return *(new IntField(payload - rhs.payload)); }
        IntField & operator*(const IntField &rhs) const { return *(new IntField(payload * rhs.payload)); }
        IntField & operator/(const IntField &rhs) const { return *(new IntField(payload / rhs.payload)); }
        IntField & operator%(const IntField &rhs) const { return *(new IntField(payload % rhs.payload)); }


        Field &  operator !() const override {
                return *(new IntField(!payload));
        }

        // TODO: make this return a BoolField when it becomes possible
        IntField & operator >= (const IntField &cmp) const {
            return *(new IntField(payload >= cmp.payload));
        }

        // TODO: make this return a BoolField when it becomes possible
        IntField & operator == (const IntField &cmp) const {
            return *(new IntField(payload == cmp.payload));
        }



        // bitwise ops
        IntField & operator&(const IntField &rhs) const { return *(new IntField(payload & rhs.payload)); }
        IntField & operator|(const IntField &rhs) const { return *(new IntField(payload | rhs.payload)); }
        IntField & operator^(const IntField &rhs) const { return *(new IntField(payload ^ rhs.payload)); }




        int32_t primitive() const { return payload; }
        std::string str() const { return std::to_string(payload); }

        // swappable
        //TODO: select  should be BoolField
        IntField & select(const IntField & choice, const IntField & other) const {
            bool selection = (bool) choice.payload;
            return selection ? *(new IntField(*this)) : *(new IntField(other));
        }







    };
}


#endif //_INTFIELD_H
