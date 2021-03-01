#include "field_factory.h"

using namespace vaultdb;

/* Can't seem to get past it failing on:
 * /src/main/cpp/query_table/field/field_factory.cpp:22:87: error: no matching conversion for C-style cast from 'const bool' to 'std::string' (aka 'basic_string<char, char_traits<char>, allocator<char> >')
    if(std::is_same<decltype(primitive), std::string>::value)  return new StringField((std::string) primitive);
template<typename T>
Field * vaultdb::FieldFactory<T>::getField(const T &primitive, const FieldType & fieldType) {

    if(std::is_same<decltype(primitive), bool>::value) return new BoolField((bool) primitive);
    if(std::is_same<decltype(primitive), int32_t>::value)      return new IntField((int32_t) primitive);
    if(std::is_same<decltype(primitive), int64_t>::value)      return new LongField((int64_t) primitive);
    if(std::is_same<decltype(primitive), float_t>::value)      return new FloatField((float_t) primitive);
    if(std::is_same<decltype(primitive), std::string>::value)  return new StringField((std::string) primitive);
    if(std::is_same<decltype(primitive), emp::Bit>::value)     return new SecureBoolField((emp::Bit) primitive);
    if(std::is_same<decltype(primitive), emp::Float>::value)     return new SecureFloatField((emp::Float) primitive);

    if(std::is_same<decltype(primitive), emp::Integer>::value) {
        switch (fieldType) {
            case FieldType::SECURE_INT32:
                return new SecureIntField((emp::Integer) primitive);
            case FieldType::SECURE_INT64:
                return new SecureLongField((emp::Integer) primitive);
            case FieldType::SECURE_STRING:
                return new SecureStringField((emp::Integer) primitive);
            default:
                throw;
        }
    }


}
 */

Field *FieldFactory::getField(const int8_t *src, const FieldType &fieldType, const size_t &strLength) {

    switch (fieldType) {
        case FieldType::BOOL:
            return new BoolField(src);
//        case FieldType::INT32:
//            return new IntField(src);
//        case FieldType::INT64:
//            return new LongField(src);
//
//        case FieldType::FLOAT32:
//            return new FloatField(src);
//
//        case FieldType::STRING:
//            return new StringField(src, strLength);
//
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(src);
//        case FieldType::SECURE_INT32:
//            return new SecureIntField(src);
//        case FieldType::SECURE_INT64:
//            return new SecureLongField(src);
//
//        case FieldType::SECURE_FLOAT32:
//            return new SecureFloatField(src);
//
//        case FieldType::SECURE_STRING:
//            return new SecureStringField(src, strLength);
//
//
       default:
            return nullptr;

    }


}

Field *FieldFactory::obliviousIf(const Field & choice, const Field & lhs,const Field & rhs) {
    return &(lhs.compareAndSwap(choice, rhs));
}
