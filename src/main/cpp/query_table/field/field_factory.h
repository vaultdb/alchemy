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

    };


    template<>
    class FieldFactory<BoolField> {
    public:
        static Field<BoolField> *getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src);
        // deepCopy may not be needed anymore, TODO: revisit this
        static Field<BoolField> *deepCopy(const Field<BoolField> *srcField);
        static Field<BoolField> getZero(const FieldType & type);
        static Field<BoolField> getOne(const FieldType & type);
        static FloatField toFloat(const Field<BoolField> *src);
        static FieldInstance<BoolField> *getFieldInstance(Field<BoolField> *src);
    };


    template<>
    class FieldFactory<SecureBoolField> {
    public:
        // deepCopy may not be needed anymore, TODO: revisit this
        static Field<SecureBoolField> *deepCopy(const Field<SecureBoolField> *srcField);
        static Field<SecureBoolField> getZero(const FieldType & type);
        static Field<SecureBoolField> getOne(const FieldType & type);
        static Field<SecureBoolField> toFloat(const Field<SecureBoolField> *src);
        static FieldInstance<SecureBoolField> *getFieldInstance(Field<SecureBoolField> *src);
    };

}

#endif
