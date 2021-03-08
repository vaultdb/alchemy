#include "util/type_utilities.h"
#include "query_table/field/secure_bool_field.h"
#include "query_table/field/bool_field.h"
/*#include "query_table/field/secure_int_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/secure_float_field.h"
#include "query_table/field/float_field.h"
#include "query_table/field/secure_string_field.h"
#include "query_table/field/string_field.h"
*/

#include <string>
 
 using namespace vaultdb;

 std::string TypeUtilities::getTypeString(const FieldType & FieldType) {
    switch(FieldType) {
        case FieldType::BOOL:
            return std::string("bool");
        case FieldType::DATE:
            return std::string("date");
        case FieldType::INT32:
            return std::string("int32");
        case FieldType::INT64:
            return std::string("int64");
        case FieldType::FLOAT32:
            return std::string("float");
        case FieldType::STRING:
            return std::string("varchar");
        case FieldType::SECURE_INT32:
            return std::string("SECURE_int32");
        case FieldType::SECURE_INT64:
            return std::string("SECURE_int64") ;
        case FieldType::SECURE_BOOL:
            return std::string("SECURE_bool" );
        case FieldType::SECURE_FLOAT32:
            return std::string("SECURE_float");

        case FieldType::SECURE_STRING:
            return std::string("encrypted varchar");
        default:
            std::string FieldTypeStr = std::to_string((int) FieldType);
            return std::string("unsupported type! " + FieldTypeStr);
    }


}


// size is in bits
size_t TypeUtilities::getTypeSize(const FieldType & id) {
    switch (id) {
        case FieldType::SECURE_BOOL:
        case FieldType::BOOL:
            return 8; // stored size when we serialize it

        case FieldType::SECURE_FLOAT32:
        case FieldType::SECURE_INT32:
        case FieldType::INT32:
        case FieldType::FLOAT32:
            return 32;

        case FieldType::SECURE_INT64:
        case FieldType::INT64:
            return 64;

        case FieldType::SECURE_STRING:
        case FieldType::STRING: // to be multiplied by length in schema for true field size
            return 8;



        default: // unsupported type
            throw;

    }
}




FieldType TypeUtilities::toSecure(const FieldType &plainType) {
    switch(plainType) {
        case FieldType::BOOL:
            return FieldType::SECURE_BOOL;
        case FieldType::INT32:
            return FieldType::SECURE_INT32;
        case FieldType::DATE:
        case FieldType::INT64:
            return FieldType::SECURE_INT64;
        case FieldType::STRING:
            return FieldType::SECURE_STRING;
        case FieldType::FLOAT32:
            return FieldType::SECURE_FLOAT32;
        default: // already secure
            return plainType;

    }
}

FieldType TypeUtilities::toPlain(const FieldType &secureType) {
    switch(secureType) {
        case FieldType::SECURE_BOOL:
            return FieldType::BOOL;
        case FieldType::SECURE_INT32:
            return FieldType::INT32;
        case FieldType::SECURE_INT64:
            return FieldType::INT64;
        case FieldType::SECURE_FLOAT32:
            return FieldType::FLOAT32;
        case FieldType::SECURE_STRING:
            return FieldType::STRING;
        default: // already plain
            return secureType;
    }
}

bool TypeUtilities::isEncrypted(const FieldType &type) {
    switch(type) {
        case FieldType::SECURE_BOOL:
        case FieldType::SECURE_INT32:
        case FieldType::SECURE_INT64:
        case FieldType::SECURE_FLOAT32:
        case FieldType::SECURE_STRING:
            return true;
        default:
            return false;
    };

 }

bool TypeUtilities::getBool(const Field & field) {
     assert(field.getType() == FieldType::BOOL);
    return field.getValue<bool>();

}

/*int32_t TypeUtilities::getInt(const Field &field) {
    assert(field.which() == 1);
    return boost::get<IntField>(field).primitive();
}

template<class T, class P, class B, class BP>
int64_t TypeUtilities::getLong(const Field &field) {
    FieldType fieldType = boost::relaxed_get<FieldInstance<T,P,B,BP> >(field).getType();

    assert(fieldType == FieldType::INT64);
    return boost::get<LongField>(field).primitive();
}

float_t TypeUtilities::getFloat(const Field &field) {
    assert(field.which() == 3);
    return boost::get<FloatField>(field).primitive();

 }

std::string TypeUtilities::getString(const Field &field) {
    assert(field.which() == 4);
    return boost::get<StringField>(field).primitive();

 }


*/

emp::Bit TypeUtilities::getSecureBool(const Field &field) {
    assert(field.getType() == FieldType::SECURE_BOOL);
    return field.getValue<emp::Bit>();

 }

/*
 * emp::Integer TypeUtilities::getSecureInt(const Field &field) {
    assert(field.which() == 6);
    return boost::get<SecureIntField>(field).primitive();

 }

emp::Integer TypeUtilities::getSecureLong(const Field &field) {
    assert(field.which() == 7);
    return boost::get<SecureLongField>(field).primitive();

 }

emp::Float TypeUtilities::getSecureFloat(const Field &field) {
    assert(field.which() == 8);
    return boost::get<SecureFloatField>(field).primitive();

 }

emp::Integer TypeUtilities::getSecureString(const Field &field) {
    assert(field.which() == 9);
    return boost::get<SecureStringField>(field).primitive();

 }
*/