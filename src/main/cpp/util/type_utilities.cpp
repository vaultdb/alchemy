#include "util/type_utilities.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include <string>
 
 using namespace vaultdb::types;

 std::string TypeUtilities::getTypeIdString(TypeId typeId) {
    switch(typeId) {
        case TypeId::BOOLEAN:
            return std::string("bool");
        case TypeId::DATE:
            return std::string("date");
        case TypeId::INTEGER32:
            return std::string("int32");
        case TypeId::INTEGER64:
            return std::string("int64");
        case TypeId::FLOAT32:
            return std::string("float");
        case TypeId::NUMERIC:
            return std::string("numeric");
        case TypeId::VARCHAR:
            return std::string("varchar");
        case TypeId::ENCRYPTED_INTEGER32:
            return std::string("encrypted_int32");
        case TypeId::ENCRYPTED_INTEGER64:
            return std::string("encrypted_int64") ;
        case TypeId::ENCRYPTED_BOOLEAN:
            return std::string("encrypted_bool" );
        case TypeId::ENCRYPTED_FLOAT32:
            return std::string("encrypted_float");

        case TypeId::ENCRYPTED_VARCHAR:
            return std::string("encrypted varchar");
        default:
            std::string typeIdStr = std::to_string((int) typeId);
            return std::string("unsupported type! " + typeIdStr);
    }


}


// size is in bits
size_t TypeUtilities::getTypeSize(TypeId id) {
    switch (id) {
        case TypeId::BOOLEAN:
        case TypeId::ENCRYPTED_BOOLEAN:
            return 1;
        case TypeId::INTEGER32:
        case TypeId::FLOAT32:
        case TypeId::NUMERIC:
        case TypeId::ENCRYPTED_INTEGER32:
        case TypeId::ENCRYPTED_FLOAT32:
            return 32;

        case TypeId::INTEGER64:
        case TypeId::ENCRYPTED_INTEGER64:
            return 64;

        case TypeId::VARCHAR: // to be multiplied by length in schema for true field size
            return 8;

        default: // unsupported type
            throw;
    }
}

bool TypeUtilities::typesEqual(const TypeId &lhs, const TypeId &rhs) {

    if(lhs == rhs)
        return true;

    // interchangeable types:
    if((lhs ==  TypeId::INTEGER32 && rhs ==  TypeId::DATE) ||
            ( lhs ==  TypeId::DATE && rhs ==  TypeId::INTEGER32)) {
        return true;
    }

    if((lhs ==  TypeId::NUMERIC && rhs ==  TypeId::FLOAT32) ||
       ( lhs ==  TypeId::FLOAT32 && rhs ==  TypeId::NUMERIC)) {
        return true;
    }

    return false;

}

Value TypeUtilities::decodeStringValue(const string &strValue, const QueryFieldDesc &fieldSpec) {


    switch (fieldSpec.getType()) {
        case TypeId::INTEGER32: {
            int32_t intValue = std::atoi(strValue.c_str());
            return Value( intValue);
        }
        case TypeId::INTEGER64: {
            int64_t intValue = std::atol(strValue.c_str());
            return Value(intValue);
        }
        case TypeId::BOOLEAN: {
            bool boolValue = (strValue == "1") ? true : false;
            return Value(boolValue);
        }
        case TypeId::VARCHAR: {
            std::string fieldStr = strValue;
            while(fieldStr.length() < fieldSpec.getStringLength()) {
                fieldStr += " ";
            }
            return Value(fieldStr);
        }
        case TypeId::FLOAT32:
        case TypeId::NUMERIC: {
            float_t floatValue = std::atof(strValue.c_str());
            return Value(floatValue);
        }
        case TypeId::DATE: {
            boost::gregorian::date date(boost::gregorian::from_string(strValue));
            boost::gregorian::date epochStart(1970, 1, 1);
            int64_t epochTime = (date - epochStart).days() * 24 * 3600;
            return Value(epochTime);
        }
        default:
            throw std::invalid_argument("Unsupported type for string decoding: " + TypeUtilities::getTypeIdString(fieldSpec.getType()) + "\n");


    };
}

Value TypeUtilities::getZero(TypeId &aType) {
    switch(aType) {
        case TypeId::BOOLEAN:
            return Value(false);
        case TypeId::INTEGER32:
            return Value((int32_t) 0);
        case TypeId::DATE:
        case TypeId::INTEGER64:
            return Value((int64_t) 0);
        case TypeId::NUMERIC:
        case TypeId::FLOAT32:
            return Value((float_t) 0.0);
        case TypeId::VARCHAR:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 0;
            return Value(std::string(oneChar));
        case TypeId::ENCRYPTED_BOOLEAN:
            return Value(emp::Bit(0, emp::PUBLIC));
        case TypeId::ENCRYPTED_INTEGER32:
            return Value(TypeId::ENCRYPTED_INTEGER32, emp::Integer(32, 0, emp::PUBLIC));
        case TypeId::ENCRYPTED_INTEGER64:
            return Value(TypeId::ENCRYPTED_INTEGER64, emp::Integer(64, 0, emp::PUBLIC));
        case TypeId::ENCRYPTED_FLOAT32:
            return Value(emp::Float(0.0));
        case TypeId::ENCRYPTED_VARCHAR:
            return Value(TypeId::ENCRYPTED_VARCHAR, emp::Integer(8, 0));
        default:
            throw std::invalid_argument("Type unsupported in getZero(): " + TypeUtilities::getTypeIdString(aType));

    };
}

Value TypeUtilities::getOne(TypeId &aType) {
    switch(aType) {
        case TypeId::BOOLEAN:
            return Value(true);
        case TypeId::INTEGER32:
            return Value((int32_t) 1);
        case TypeId::DATE:
        case TypeId::INTEGER64:
            return Value((int64_t) 1);
        case TypeId::NUMERIC:
        case TypeId::FLOAT32:
            return Value((float_t) 1.0);
        case TypeId::VARCHAR:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 1;
            return Value(std::string(oneChar));
        case TypeId::ENCRYPTED_BOOLEAN:
            return Value(emp::Bit(1, emp::PUBLIC));
        case TypeId::ENCRYPTED_INTEGER32:
            return Value(TypeId::ENCRYPTED_INTEGER32, emp::Integer(32, 1, emp::PUBLIC));
        case TypeId::ENCRYPTED_INTEGER64:
            return Value(TypeId::ENCRYPTED_INTEGER64, emp::Integer(64, 1, emp::PUBLIC));
        case TypeId::ENCRYPTED_FLOAT32:
            return Value(emp::Float(1.0));
        case TypeId::ENCRYPTED_VARCHAR:
            return Value(TypeId::ENCRYPTED_VARCHAR, emp::Integer(8, (int)'1'));
        default:
            throw std::invalid_argument("Type unsupported in getOne(): " + TypeUtilities::getTypeIdString(aType));

    };
}

TypeId TypeUtilities::toSecure(const TypeId &plainType) {
    switch(plainType) {
        case TypeId::BOOLEAN:
            return TypeId::ENCRYPTED_BOOLEAN;
        case TypeId::INTEGER32:
            return TypeId::ENCRYPTED_INTEGER32;
        case TypeId::DATE:
        case TypeId::INTEGER64:
            return TypeId::ENCRYPTED_INTEGER64;
        case TypeId::VARCHAR:
            return TypeId::ENCRYPTED_VARCHAR;
        case TypeId::NUMERIC:
        case TypeId::FLOAT32:
            return TypeId::ENCRYPTED_FLOAT32;
        default: // already secure
            return plainType;

    }
}

TypeId TypeUtilities::toPlain(const TypeId &secureType) {
    switch(secureType) {
        case TypeId::ENCRYPTED_BOOLEAN:
            return TypeId::BOOLEAN;
        case TypeId::ENCRYPTED_INTEGER32:
            return TypeId::INTEGER32;
        case TypeId::ENCRYPTED_INTEGER64:
            return TypeId::INTEGER64;
        case TypeId::ENCRYPTED_FLOAT32:
            return TypeId::FLOAT32;
        case TypeId::ENCRYPTED_VARCHAR:
            return TypeId::VARCHAR;
        default: // already plain
            return secureType;
    }
}

