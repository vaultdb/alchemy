#ifndef _FIELD_FACTORY_H
#define _FIELD_FACTORY_H

#include <emp-tool/circuits/bit.h>
#include <emp-tool/circuits/integer.h>
#include <emp-tool/circuits/float32.h>
#include "bool_field.h"
#include "secure_bool_field.h"

#include "int_field.h"
#include "long_field.h"
#include "string_field.h"
#include "float_field.h"
#include "secure_string_field.h"
#include "secure_int_field.h"
#include "secure_long_field.h"
#include "secure_float_field.h"


namespace vaultdb {


    template<typename B>
    class FieldFactory {
    public:
/*
        template<typename T>
         static inline Field * getField(const FieldType & type, const T & src) {
            switch(type) {
                case FieldType::BOOL:
                    return new BoolField((bool) src);
                case FieldType::INT:
                    return new IntField((int32_t) src);
                case FieldType::LONG:
                    return new LongField((int64_t) src);

                case FieldType::FLOAT:
                    return new FloatField((float_t) src);
                case FieldType::STRING:
                    return new StringField(std::string(src));
                case FieldType::SECURE_BOOL:
                    return new SecureBoolField((emp::Bit) src);
                case FieldType::SECURE_INT:
                    return new SecureIntField((emp::Integer) src);
                case FieldType::SECURE_LONG:
                    return new SecureLongField((emp::Integer) src);
                case FieldType::SECURE_FLOAT:
                    return new SecureFloatField((emp::Float) src);
                case FieldType::SECURE_STRING:
                    return new SecureStringField((emp::Integer) src);

                default: // invalid
                    throw;
            }
        }*/

        static Field<B> * getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src);
        static Field<B> *deepCopy(const Field<B> *srcField);
        static Field<B> *toFloat(const Field<B> *src);
        static Field<B> getZero(const FieldType & type);
        static Field<B> getOne(const FieldType & type);
        static FieldInstance<B> *getInstance(const Field<B> *src);
    };



}

#endif
