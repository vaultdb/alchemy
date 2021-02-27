#include "field_factory.h"
#include "int_field.h"

using namespace vaultdb;


template<typename T>
std::shared_ptr<Field> vaultdb::FieldFactory<T>::getField(const T &primitive, const FieldType &fieldType) {

    switch (fieldType) {
        case FieldType::INT32:
            return std::shared_ptr<Field>(new IntField((int32_t) primitive));
        default:
            return std::shared_ptr<Field>();

    }

    return std::shared_ptr<Field>();


}
