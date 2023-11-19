#include "util/type_utilities.h"

#include <string>
 
 using namespace vaultdb;

 std::string TypeUtilities::getTypeName(const FieldType & type) {
    switch(type) {
        case FieldType::BOOL:
            return std::string("bool");
        case FieldType::DATE:
            return std::string("date");
        case FieldType::INT:
            return std::string("int32");
        case FieldType::LONG:
            return std::string("int64");
        case FieldType::FLOAT:
            return std::string("float");
        case FieldType::STRING:
            return std::string("varchar");
        case FieldType::SECURE_INT:
            return std::string("shared-int32");
        case FieldType::SECURE_LONG:
            return std::string("shared-int64") ;
        case FieldType::SECURE_BOOL:
            return std::string("shared-bool" );
        case FieldType::SECURE_FLOAT:
            return std::string("shared-float");

        case FieldType::SECURE_STRING:
            return std::string("shared-varchar");
        default:
            std::string FieldTypeStr = std::to_string((int) type);
            return std::string("unsupported type! " + FieldTypeStr);
    }


}

FieldType TypeUtilities::getTypeFromString(const string & field_str) {
     string field_str_lower = field_str;
    transform(field_str_lower.begin(), field_str_lower.end(), field_str_lower.begin(), ::tolower);

    if(field_str_lower == "bool")
        return FieldType::BOOL;

    if(field_str_lower == "date")
        return FieldType::DATE;

    if(field_str_lower == "int32")
        return FieldType::INT;

    if(field_str_lower == "int64")
        return FieldType::LONG;

    if(field_str_lower == "float")
        return FieldType::FLOAT;
    // TODO: rename this to char!
    if(field_str_lower == "varchar")
        return FieldType::STRING;

    if(field_str_lower == "shared-bool")
        return FieldType::SECURE_BOOL;


    if(field_str_lower == "shared-int32")
        return FieldType::SECURE_INT;

    if(field_str_lower == "shared-int64")
        return FieldType::SECURE_LONG;

    if(field_str_lower == "shared-float")
        return FieldType::SECURE_FLOAT;
    if(field_str_lower == "shared-varchar")
        return FieldType::SECURE_STRING;

    throw std::invalid_argument("Unsupported type " + field_str);
}

// size is in bits
size_t TypeUtilities::getTypeSize(const FieldType & id) {
    switch (id) {
        case FieldType::SECURE_BOOL:
            return 1;
        case FieldType::BOOL:
            return 8; // stored size when we serialize it

        case FieldType::SECURE_FLOAT:
        case FieldType::SECURE_INT:
        case FieldType::INT:
        case FieldType::FLOAT:
        case FieldType::DATE:
            return 32;

        case FieldType::SECURE_LONG:
        case FieldType::LONG:
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
        case FieldType::INT:
            return FieldType::SECURE_INT;
        case FieldType::DATE:
        case FieldType::LONG:
            return FieldType::SECURE_LONG;
        case FieldType::STRING:
            return FieldType::SECURE_STRING;
        case FieldType::FLOAT:
            return FieldType::SECURE_FLOAT;
        default: // already secure
            return plainType;

    }
}

FieldType TypeUtilities::toPlain(const FieldType &secureType) {
    switch(secureType) {
        case FieldType::SECURE_BOOL:
            return FieldType::BOOL;
        case FieldType::SECURE_INT:
            return FieldType::INT;
        case FieldType::SECURE_LONG:
            return FieldType::LONG;
        case FieldType::SECURE_FLOAT:
            return FieldType::FLOAT;
        case FieldType::SECURE_STRING:
            return FieldType::STRING;
        default: // already plain
            return secureType;
    }
}

bool TypeUtilities::isEncrypted(const FieldType &type) {
    switch(type) {
        case FieldType::SECURE_BOOL:
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_FLOAT:
        case FieldType::SECURE_STRING:
            return true;
        default:
            return false;
    };

 }

bool TypeUtilities::types_equivalent(const FieldType & lhs, const FieldType & rhs) {
     if(lhs == rhs)
         return true;

     if(lhs == FieldType::DATE && rhs == FieldType::LONG) return true;
     if(rhs == FieldType::DATE && lhs == FieldType::LONG) return true;

     return false;
 }

string TypeUtilities::getJSONTypeString(const FieldType &type_id) {
    switch(type_id) {
        case FieldType::BOOL:
        case FieldType::SECURE_BOOL:
            return "BOOLEAN";
        case FieldType::INT:
        case FieldType::SECURE_INT:
            return "INTEGER";
        case FieldType::LONG:
        case FieldType::SECURE_LONG:
            return "LONG";
        case FieldType::FLOAT:
        case FieldType::SECURE_FLOAT:
            return "FLOAT";
        case FieldType::STRING:
        case FieldType::SECURE_STRING:
            return "CHAR";
        case FieldType::DATE:
            return "DATE";
        default:
            return "INVALID";

    }
}

template<>
FieldType TypeUtilities::getBoolType<bool>() {
    return FieldType::BOOL;
}

template<>
FieldType TypeUtilities::getBoolType<emp::Bit>() {
    return FieldType::SECURE_BOOL;
}

