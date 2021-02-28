#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include "field_instance.h"


namespace vaultdb {


    class BoolField : public FieldInstance<BoolField, BoolField, BoolField, int32_t> {
    protected:
        bool payload = true;

    public:

        BoolField() : FieldInstance<BoolField, BoolField, BoolField, int32_t>() {}
        BoolField(const BoolField & src) : FieldInstance<BoolField, BoolField, BoolField, int32_t>(src) { payload = src.payload; }
        BoolField(const int32_t & src) : FieldInstance<BoolField, BoolField, BoolField, int32_t>() { payload = src; }
        BoolField(const int8_t * src)  : FieldInstance<BoolField, BoolField, BoolField, int32_t>(){
            memcpy((int8_t *) &payload, src, size()/8);
        }

        BoolField& operator=(const BoolField& other) {
            this->payload = other.payload;
            return *this;
        }


        void copy(const BoolField & src) {payload = src.payload; }
        void assign(const int32_t & src) {payload = src; }

        static FieldType type() { return FieldType::BOOL; }
        size_t size() const override{ return 8; }

        Field *decrypt() const { return new BoolField(*this); }




        BoolField & operator+(const BoolField &rhs) const { return *(new BoolField(payload + rhs.payload)); }
        BoolField & operator-(const BoolField &rhs) const { return *(new BoolField(payload - rhs.payload)); }
        BoolField & operator*(const BoolField &rhs) const { return *(new BoolField(payload * rhs.payload)); }
        BoolField & operator/(const BoolField &rhs) const { return *(new BoolField(payload / rhs.payload)); }
        BoolField & operator%(const BoolField &rhs) const { return *(new BoolField(payload % rhs.payload)); }


        Field &  operator !() const override {
            return *(new BoolField(!payload));
        }

        BoolField & operator >= (const BoolField &cmp) const {
            return *(new BoolField(payload >= cmp.payload));
        }

        BoolField & operator == (const BoolField &cmp) const {
            return *(new BoolField(payload == cmp.payload));
        }



        // bitwise ops
        BoolField & operator&(const BoolField &rhs) const { return *(new BoolField(payload & rhs.payload)); }
        BoolField & operator|(const BoolField &rhs) const { return *(new BoolField(payload | rhs.payload)); }
        BoolField & operator^(const BoolField &rhs) const { return *(new BoolField(payload ^ rhs.payload)); }




        int32_t primitive() const { return payload; }
        std::string str() const {
            return  payload ? "true" : "false";
        }

        // swappable
        BoolField & select(const BoolField & choice, const BoolField & other) const {
            bool selection = (bool) choice.payload;
            return selection ? *(new BoolField(*this)) : *(new BoolField(other));
        }






    };

}
#endif //BOOL_FIELD_H
