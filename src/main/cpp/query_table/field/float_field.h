#ifndef _FLOAT_FIELD_H
#define _FLOAT_FIELD_H


#include "field_instance.h"
#include "bool_field.h"
#include <cstdint>
#include <emp-tool/circuits/float32.h>

#include <cstdint>
#include <cstring>



namespace vaultdb {
    //  // T = derived field
    //    // P = primitive / payload of field, needed for serialize
    //    // B = boolean field result
    //    template<typename T, typename P, typename B>
    class FloatField : public FieldInstance<FloatField, float_t, BoolField> {
    protected:
        float_t payload = 0.0;

    public:

        FloatField()  {}
        FloatField(const FloatField & src)   { payload = src.payload; }
        FloatField(const float_t & src)   { payload = src; }
        FloatField(const int8_t * src)   { memcpy((int8_t *) &payload, src, size()/8); }
        FloatField(const emp::Float & src, const int & party) {
            payload = src.reveal<double>(party);
        }

        FloatField& operator=(const FloatField& other) {
            this->payload = other.payload;
            return *this;
        }


        bool encrypted() const { return false; }
        void copy(const FloatField & src) { payload = src.payload; }
        void assign(const float_t & src) {payload = src; }

        static FieldType type() { return FieldType::FLOAT32; }

        size_t size() const override { return 32; }




        FloatField & operator+(const FloatField &rhs) const { return *(new FloatField(payload + rhs.payload)); }
        FloatField & operator-(const FloatField &rhs) const { return *(new FloatField(payload - rhs.payload)); }
        FloatField & operator*(const FloatField &rhs) const { return *(new FloatField(payload * rhs.payload)); }
        FloatField & operator/(const FloatField &rhs) const { return *(new FloatField(payload / rhs.payload)); }
        FloatField & operator%(const FloatField &rhs) const { throw; }


        Field &  operator !() const override {
            return *(new FloatField(!payload));
        }

        BoolField & operator >= (const FloatField &cmp) const {
            return *(new BoolField(payload >= cmp.payload));
        }

        BoolField & operator == (const FloatField &cmp) const {
            return *(new BoolField(payload == cmp.payload));
        }



        float_t primitive() const { return payload; }
        std::string str() const { return std::to_string(payload); }

        // swappable
        FloatField & select(const BoolField & choice, const FloatField & other) const {
            bool selection =  choice.primitive();
            return selection ? *(new FloatField(*this)) : *(new FloatField(other));
        }

        void serialize(int8_t *dst) const override {
            size_t len  = size() / 8;
            std::memcpy(dst,  (int8_t *) &payload, len);
        }

        FloatField & operator&(const FloatField &right) const { throw; } // bitwise ops not defined for float
        FloatField & operator^(const FloatField &right) const { throw; } // bitwise ops not defined for float
        FloatField & operator|(const FloatField &right) const { throw; } // bitwise ops not defined for float




    };
}

#endif //_FLOAT_FIELD_H
