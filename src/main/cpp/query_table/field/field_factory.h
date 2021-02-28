#ifndef _FIELD_FACTORY_H
#define _FIELD_FACTORY_H

#include <query_table/field/field.h>

namespace vaultdb {

    template<typename T>
    class FieldFactory {
    public:
        static Field * getField(const T &primitive, const FieldType &fieldType);
        static Field * getField(const int8_t * src, const FieldType & fieldType); // deserialize call
        static Field * obliviousIf(const Field & choice, const Field &lhs, const Field & rhs); // wrapper for select
    };



}

#endif
