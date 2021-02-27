#ifndef _FIELD_FACTORY_H
#define _FIELD_FACTORY_H

#include <query_table/field/field.h>

namespace vaultdb {

    template<typename T>
    class FieldFactory {
    public:
        static std::shared_ptr<Field> getField(const T &primitive, const FieldType &fieldType);
    };



}

#endif
