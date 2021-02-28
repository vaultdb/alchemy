#ifndef _SECURE_BOOL_FIELD_H
#define _SECURE_BOOL_FIELD_H

#include <emp-tool/bit.h>

using namespace emp;

namespace vaultdb {


    class SecureBoolField : public FieldInstance<SecureBoolField, SecureBoolField, bool, bool> {

    public:

        SecureBoolField() : FieldInstance<SecureBoolField, SecureBoolField, bool, bool>() {}
        SecureBoolField(const SecureBoolField & src) : FieldInstance<SecureBoolField, SecureBoolField, bool, bool>(src)
        { payload = src.payload; }
        SecureBoolField(const int32_t & src) : FieldInstance<SecureBoolField, SecureBoolField, bool, bool>() { payload = src; }

        SecureBoolField& operator=(const SecureBoolField& other) {
            this->payload = other.payload;
            return *this;
        }


        void copy(const SecureBoolField & src) {payload = src.payload; }
        void assign(const int32_t & src) {payload = src; }

        static FieldType type() { return FieldType::SECURE_BOOL; }
        size_t size() const { return 8; }

        Field *decrypt() const { return new SecureBoolField(*this); }


        static SecureBoolField deserialize(const int8_t *cursor) {
            SecureBoolField ret;
            memcpy((int8_t *) &ret.payload, cursor, ret.size()/8);
            return ret;

        }

        int32_t primitive() const { return payload; }
        std::string str() const { return std::to_string(payload); }


        SecureBoolField  operator!() const {
            return !payload;
        }


        SecureBoolField & geq(const SecureBoolField & rhs) const {

            return SecureBoolField(payload >= rhs.payload);
        }


        SecureBoolField & equal(const SecureBoolField & rhs) const {
            return  SecureBoolField(payload == rhs.payload);
        }

    protected:
        Bit payload;

    };
#endif //_SECURE_BOOL_FIELD_H
