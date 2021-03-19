#ifndef _SECURE_LONG_FIELD_H
#define _SECURE_LONG_FIELD_H

#include "field_impl.h"
#include "field.h"
#include "secure_bool_field.h"
#include "long_field.h"

namespace vaultdb {

    // T = derived field
    // B = boolean field result

    // BoolField is a decorator for Field
    // it implements all of the type-specific functionalities, but delegates storing the payload to the Field class
    class SecureLongField : public FieldImpl<SecureLongField, SecureBoolField>, public Field<SecureBoolField>  {

    public:

        SecureLongField()  : Field(FieldType::SECURE_LONG) {}
        ~SecureLongField() = default;

        explicit SecureLongField(const Field &srcField);

        SecureLongField(const SecureLongField &src);

        SecureLongField(const emp::Integer &src);

        explicit SecureLongField(const LongField *src, const int &myParty, const int &dstParty);

        explicit SecureLongField(const int8_t *src);
        // encrypt from public
        explicit SecureLongField(const int64_t & src);




        emp::Integer getPayload() const;

        SecureLongField &operator=(const SecureLongField &other);


        SecureLongField operator+(const SecureLongField &rhs) const;

        SecureLongField operator-(const SecureLongField &rhs) const;

        SecureLongField operator*(const SecureLongField &rhs) const;

        SecureLongField operator/(const SecureLongField &rhs) const;

        SecureLongField operator%(const SecureLongField &rhs) const;


        // not defined in EMP
        SecureBoolField neg() const { throw; }


        SecureBoolField operator>=(const SecureLongField &cmp) const;

        SecureBoolField operator==(const SecureLongField &cmp) const;


        // swappable
        SecureLongField selectValue(const SecureBoolField &choice, const SecureLongField &other) const;


        // bitwise ops
        SecureLongField operator&(const SecureLongField &right) const;

        SecureLongField operator^(const SecureLongField &right) const;

        SecureLongField operator|(const SecureLongField &right) const;

        void ser(int8_t * target) const;


    };

    std::ostream &operator<<(std::ostream &os, const SecureLongField &aValue);

}

#endif //_SECURE_LONG_FIELD_H
