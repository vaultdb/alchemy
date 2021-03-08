#ifndef _FIELD_FACTORY_H
#define _FIELD_FACTORY_H

#include <emp-tool/circuits/bit.h>
#include <emp-tool/circuits/integer.h>
#include <emp-tool/circuits/float32.h>
#include "bool_field.h"
#include "secure_bool_field.h"

/*#include "int_field.h"
#include "long_field.h"
#include "string_field.h"
#include "float_field.h"
#include "secure_string_field.h"
#include "secure_int_field.h"
#include "secure_long_field.h"
#include "secure_float_field.h"*/


namespace vaultdb {


    class FieldFactory {
    public:

        static Field getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src);
        //static Field *toFloat(const Field *src);
    };



}

#endif
