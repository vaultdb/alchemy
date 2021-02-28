#include "field_factory.h"
#include "int_field.h"
#include "bool_field.h"

using namespace vaultdb;


template<typename T>
Field * vaultdb::FieldFactory<T>::getField(const T &primitive, const FieldType &fieldType) {

    switch (fieldType) {
        case FieldType::INT32:
            return new IntField((int32_t) primitive);
        case FieldType::BOOL:
            return new BoolField((bool) primitive);

        default:
            return nullptr;

    }

    return nullptr;


}

template<typename T>
Field *FieldFactory<T>::getField(const int8_t *src, const FieldType &fieldType) {

    switch (fieldType) {
        case FieldType::INT32:
            return new IntField(src);
        case FieldType::BOOL:
            return new BoolField(src);

        default:
            return nullptr;

    }

    return nullptr;

}

template<typename T>
Field *FieldFactory<T>::obliviousIf(const Field & choice, const Field & lhs,const Field & rhs) {
    return &(lhs.compareAndSwap(choice, rhs));
}
