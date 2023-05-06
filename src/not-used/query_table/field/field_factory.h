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
#include "secure_float_field.h"*


namespace vaultdb {


    class FieldFactory {
    public:
        static Field *copyField(const Field * src);
        static Field *getField(const std::string & src, const FieldType & type, const int & strLength = 0);
        static Field *getZero(const FieldType & type);
        static Field *getOne(const FieldType & type);

        // can't seem to do this in just one statement because we can't enforce the relationship between fieldType and the templated T type
        //static Field * getField(const bool &primitive, const FieldType &fieldType) { return reinterpret_cast<Field *>(new BoolField(primitive)); }
       /* static Field * getField(const int32_t &primitive, const FieldType &fieldType) { return reinterpret_cast<Field *>(new IntField(primitive)); }
        static Field * getField(const int64_t &primitive, const FieldType &fieldType) { return reinterpret_cast<Field *>(new LongField(primitive)); }
        static Field * getField(const float_t &primitive, const FieldType &fieldType) { return reinterpret_cast<Field *>(new FloatField(primitive)); }
        static Field * getField(const std::string &primitive, const FieldType &fieldType) { return reinterpret_cast<Field *>(new StringField(primitive)); }*/
      // static Field * getField(const emp::Bit &primitive, const FieldType &fieldType) { return reinterpret_cast<Field *>(new SecureBoolField(primitive)); }
/*        static Field * getField(const emp::Float &primitive, const FieldType &fieldType) { return reinterpret_cast<Field *>(new SecureFloatField(primitive)); }
        static Field * getField(const emp::Integer &primitive, const FieldType &fieldType) {
            switch (fieldType) {
                case FieldType::SECURE_INT32:
                    return reinterpret_cast<Field *>(new SecureIntField(primitive));
                case FieldType::SECURE_INT64:
                    return reinterpret_cast<Field *>(new SecureLongField(primitive));
                case FieldType::SECURE_STRING:
                    return reinterpret_cast<Field *>(new SecureStringField(primitive));
                default:
                    throw;
            }
        }
*/

        static Field *getField(const int8_t *src, const FieldType &fieldType, const size_t &stringLength); // deserialize call
        static Field * obliviousIf(const Field & choice, const Field &lhs, const Field & rhs); // wrapper for select
        static void compareAndSwap(const Field & choice,  Field & lhs,  Field & rhs);

        static Field *toFloat(const Field *src);
    };



}

#endif
