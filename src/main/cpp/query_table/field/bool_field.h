#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include "field_instance.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class BoolField : public FieldInstance<BoolField, BoolField> {
    protected:

        Field field_; // points to Field.data
        bool *payload_ = nullptr;

    public:

        BoolField() = default;
        explicit BoolField(const Field & srcField);

        BoolField(const BoolField & src);

        explicit BoolField(const bool & src);
        explicit BoolField(const int8_t * src)  {
            bool input = reinterpret_cast<const bool &>(*src);
            field_ = Field::createBool(input);
            payload_ = reinterpret_cast<bool *>(field_.getData());
        }


        // constructor for decryption
        BoolField(const emp::Bit & src, const int & party);


        Field getBaseField() const { return field_; }

        BoolField& operator=(const BoolField& other);


        bool encrypted() const override { return false; }
        //void copy(const BoolField & src) {*payload = *src.payload; }
        //void assign(const bool & src) {*payload = src; }

        static FieldType type() { return FieldType::BOOL; }

        static size_t size()  { return FieldUtils::getPhysicalSize(type()); }


        BoolField  operator+(const BoolField &rhs) const  { throw; } // cast to int before doing arithmetic expressions
        BoolField  operator-(const BoolField &rhs) const { throw; }
        BoolField  operator*(const BoolField &rhs) const  { throw; }
        BoolField  operator/(const BoolField &rhs) const  { throw; }
        BoolField  operator%(const BoolField &rhs) const  { throw; }


        BoolField negate() const { return BoolField(!(*payload_)); }



        BoolField  operator >= (const BoolField &cmp) const;

        BoolField  operator == (const BoolField &cmp) const;




        bool primitive() const { return *payload_; }
        std::string str() const { return  *payload_ ? "true" : "false"; }

        // swappable
        BoolField  select(const BoolField & choice, const BoolField & other) const;


        void serialize(int8_t *dst) const override { field_.serialize(dst); }

        // bitwise ops
        BoolField  operator&(const BoolField &right) const { return  BoolField((*payload_) & *(right.payload_)); }

        BoolField  operator^(const BoolField &right) const { return  BoolField((*payload_) ^ *(right.payload_)); }

        BoolField  operator|(const BoolField &right) const { return  BoolField((*payload_) | *(right.payload_)); }




    };

    std::ostream &operator<<(std::ostream &os, const BoolField &aValue);

}
#endif //BOOL_FIELD_H
