#ifndef SECURE_BOOL_FIELD_H
#define SECURE_BOOL_FIELD_H

#include "field.h"
#include "field_instance.h"
#include <emp-tool/circuits/bit.h>


namespace vaultdb {


    // T = derived field
    // B = boolean field result

    // SecureBoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureBoolField : public FieldInstance<SecureBoolField, SecureBoolField> {
    protected:

        Field field_; // points to Field.data
        emp::Bit *payload_ = nullptr;

    public:

        SecureBoolField() = default;
        explicit SecureBoolField(const Field & srcField)   : field_(srcField) {
            payload_ = reinterpret_cast<emp::Bit *>(field_.getData());
        }

        SecureBoolField(const SecureBoolField & src)  {
            field_ = Field::createSecureBool(src.primitive());
            payload_ = reinterpret_cast<emp::Bit *>(field_.getData());
        }

        explicit SecureBoolField(const emp::Bit & src)  {
            field_ = Field::createSecureBool(src);
            payload_ = reinterpret_cast<emp::Bit *>(field_.getData());
        }

        explicit SecureBoolField(const int8_t * src);



        // constructor for encrypting a bit
        SecureBoolField(const bool & src, const int & myParty, const int & dstParty);


        Field getBaseField() const { return field_; }

        SecureBoolField& operator=(const SecureBoolField& other);


        bool encrypted() const override { return true; }
        //void copy(const SecureBoolField & src) {*payload = *src.payload; }
        //void assign(const bool & src) {*payload = src; }

        static FieldType type() { return FieldType::SECURE_BOOL; }
        static size_t size()  { return FieldUtils::getPhysicalSize(type()); }


        SecureBoolField  operator+(const SecureBoolField &rhs) const  { throw; } // cast to int before doing arithmetic expressions
        SecureBoolField  operator-(const SecureBoolField &rhs) const { throw; }
        SecureBoolField  operator*(const SecureBoolField &rhs) const  { throw; }
        SecureBoolField  operator/(const SecureBoolField &rhs) const  { throw; }
        SecureBoolField  operator%(const SecureBoolField &rhs) const  { throw; }


        SecureBoolField negate() const { return SecureBoolField(!(*payload_)); }

        SecureBoolField  operator >= (const SecureBoolField &cmp) const;
        SecureBoolField  operator == (const SecureBoolField &cmp) const;




        emp::Bit primitive() const { return *payload_; }
        std::string str() const { return "SECURE BIT"; }

        // swappable
        SecureBoolField  select(const SecureBoolField & choice, const SecureBoolField & other) const;


        void serialize(int8_t *dst) const override { field_.serialize(dst); }

        // bitwise ops
        SecureBoolField  operator&(const SecureBoolField &right) const { return  SecureBoolField(*payload_ & *(right.payload_)); }

        SecureBoolField  operator^(const SecureBoolField &right) const { return  SecureBoolField(*payload_ ^ *(right.payload_)); }

        SecureBoolField  operator|(const SecureBoolField &right) const { return  SecureBoolField(*payload_ | *(right.payload_)); }




    };






    std::ostream &operator<<(std::ostream &os, const SecureBoolField &aValue);

}
#endif //BOOL_FIELD_H
