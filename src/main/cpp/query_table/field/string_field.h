#ifndef STRING_FIELD_H
#define STRING_FIELD_H


#include "field_instance.h"
#include "bool_field.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class StringField : public FieldInstance<StringField, BoolField>, public Field  {

    public:

        StringField() = default;
        ~StringField() = default;
        explicit StringField(const Field & srcField);

        StringField(const StringField & src);

        explicit StringField(const std::string & src);
        explicit StringField(const int8_t * src, const size_t & strLength);


        // constructor for decryption
        StringField(const emp::Integer & src, const int & party);



        std::string getPayload() const { return getValue(); }

        StringField& operator=(const StringField& other);


        StringField  operator+(const StringField &rhs) const  {    return StringField(getPayload() + rhs.getPayload());  }
        StringField  operator-(const StringField &rhs) const  { throw; }   // semantics not clear
        StringField  operator*(const StringField &rhs) const  { throw; }
        StringField  operator/(const StringField &rhs) const  { throw; }
        StringField  operator%(const StringField &rhs) const  { throw; }


        StringField negate() const { throw; }



        BoolField  operator >= (const StringField &cmp) const;
        BoolField  operator == (const StringField &cmp) const;


        // swappable
        StringField  select(const BoolField & choice, const StringField & other) const;



        // bitwise ops
        StringField  operator&(const StringField &right) const {  throw; }
        StringField  operator^(const StringField &right) const  {  throw; }
        StringField  operator|(const StringField &right) const   {  throw; }


    };

    std::ostream &operator<<(std::ostream &os, const StringField &aValue);

}

#endif //STRING_FIELD_H
