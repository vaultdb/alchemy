#ifndef FLOAT_FIELD_H
#define FLOAT_FIELD_H

#include "bool_field.h"
#include "field.h"

namespace vaultdb {

    // FloatField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class FloatField :  public Field<BoolField>  {

    public:

        FloatField() :  Field(FieldType::FLOAT) {}
        ~FloatField() = default;
        explicit FloatField(const Field & srcField);

        FloatField(const FloatField & src);

        explicit FloatField(const float_t & src);
        explicit FloatField(const int8_t * src);


        // constructor for decryption
        FloatField(const emp::Float & src, const int & party);


        float_t getPayload() const { return getValue<float_t>(); }

        FloatField& operator=(const FloatField& other);


        FloatField  operator+(const FloatField &rhs) const  {    return FloatField(getPayload() + rhs.getPayload());  }
        FloatField  operator-(const FloatField &rhs) const  {    return FloatField(getPayload() - rhs.getPayload());  }
        FloatField  operator*(const FloatField &rhs) const  {    return FloatField(getPayload() * rhs.getPayload());  }
        FloatField  operator/(const FloatField &rhs) const  {    return FloatField(getPayload() / rhs.getPayload());  }
        FloatField  operator%(const FloatField &rhs) const  {    throw;  }


        // only for bool types
        BoolField neg() const { throw; }
        std::string str() const { return std::to_string(getPayload());  }




        BoolField  operator >= (const FloatField &cmp) const;
        BoolField  operator == (const FloatField &cmp) const;


        // swappable
        FloatField  selectValue(const BoolField & choice, const FloatField & other) const;



        // bitwise ops
        FloatField  operator&(const FloatField &right) const {  throw; }
        FloatField  operator^(const FloatField &right) const  {  throw; }
        FloatField  operator|(const FloatField &right) const   {  throw; }

        // serialize
        void ser(int8_t * target) const { *((int32_t *) target) = getPayload();  }

    };

    std::ostream &operator<<(std::ostream &os, const FloatField &aValue);

}

#endif //FLOAT_FIELD_H
