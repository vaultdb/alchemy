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
        static PlainField *getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src);
        static PlainField getZero(const FieldType & type);
        static PlainField getOne(const FieldType & type);

        static FloatField toFloat(const PlainField &src);
        static LongField toLong(const PlainField &src);

        static PlainField getMin(const FieldType & type);
        static PlainField getMax(const FieldType & type);

        static PlainField getInt(const int32_t & src) { return IntField(src);  }
        static FieldInstance<BoolField> *getFieldInstance(PlainField *src);
    };


    template<>
    class FieldFactory<SecureBoolField> {
    public:
        static SecureField getZero(const FieldType & type);
        static SecureField getOne(const FieldType & type);

        static SecureField toFloat(const SecureField &src);
        static SecureLongField toLong(const SecureField &field);

        static SecureField getMin(const FieldType & type);
        static SecureField getMax(const FieldType & type);

        static SecureField getInt(const int32_t & src) { return SecureIntField(src);  }
        static FieldInstance<SecureBoolField> *getFieldInstance(SecureField *src);

    };

}

#endif
