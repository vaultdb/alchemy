#include "util/type_utilities.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include <string>

 std::string TypeUtilities::getTypeIdString(vaultdb::types::TypeId typeId) {
    switch(typeId) {
        case vaultdb::types::TypeId::BOOLEAN:
            return std::string("bool");
        case vaultdb::types::TypeId::DATE:
            return std::string("date");
        case vaultdb::types::TypeId::INTEGER32:
            return std::string("int32");
        case vaultdb::types::TypeId::INTEGER64:
            return std::string("int64");
        case vaultdb::types::TypeId::FLOAT32:
            return std::string("float");
        case vaultdb::types::TypeId::NUMERIC:
            return std::string("numeric");
        case vaultdb::types::TypeId::VARCHAR:
            return std::string("varchar");
        case vaultdb::types::TypeId::ENCRYPTED_INTEGER32:
            return std::string("encrypted_int32");
        case vaultdb::types::TypeId::ENCRYPTED_INTEGER64:
            return std::string("encrypted_int64") ;
        case vaultdb::types::TypeId::ENCRYPTED_BOOLEAN:
            return std::string("encrypted_bool" );
        case vaultdb::types::TypeId::ENCRYPTED_FLOAT32:
            return std::string("encrypted_float");

        case vaultdb::types::TypeId::ENCRYPTED_VARCHAR:
            return std::string("encrypted varchar");
        default:
            std::string typeIdStr = std::to_string((int) typeId);
            return std::string("unsupported type! " + typeIdStr);
    }


}


// size is in bits
size_t TypeUtilities::getTypeSize(types::TypeId id) {
    switch (id) {
        case vaultdb::types::TypeId::BOOLEAN:
        case vaultdb::types::TypeId::ENCRYPTED_BOOLEAN:
            return 1;
        case vaultdb::types::TypeId::INTEGER32:
        case vaultdb::types::TypeId::FLOAT32:
        case vaultdb::types::TypeId::NUMERIC:
        case vaultdb::types::TypeId::ENCRYPTED_INTEGER32:
        case vaultdb::types::TypeId::ENCRYPTED_FLOAT32:
            return 32;

        case vaultdb::types::TypeId::INTEGER64:
        case vaultdb::types::TypeId::ENCRYPTED_INTEGER64:
            return 64;

        case vaultdb::types::TypeId::VARCHAR: // to be multiplied by length in schema for true field size
            return 8;

        default: // unsupported type
            throw;
    }
}

bool TypeUtilities::typesEqual(const types::TypeId &lhs, const types::TypeId &rhs) {

    if(lhs == rhs)
        return true;

    // interchangeable types:
    if((lhs ==  vaultdb::types::TypeId::INTEGER32 && rhs ==  vaultdb::types::TypeId::DATE) ||
            ( lhs ==  vaultdb::types::TypeId::DATE && rhs ==  vaultdb::types::TypeId::INTEGER32)) {
        return true;
    }

    if((lhs ==  vaultdb::types::TypeId::NUMERIC && rhs ==  vaultdb::types::TypeId::FLOAT32) ||
       ( lhs ==  vaultdb::types::TypeId::FLOAT32 && rhs ==  vaultdb::types::TypeId::NUMERIC)) {
        return true;
    }

    return false;

}

types::Value TypeUtilities::decodeStringValue(const string &strValue, const QueryFieldDesc &fieldSpec) {


    switch (fieldSpec.getType()) {
        case types::TypeId::INTEGER32: {
            int32_t intValue = std::atoi(strValue.c_str());
            return types::Value(intValue);
        }
        case types::TypeId::INTEGER64: {
            int64_t intValue = std::atol(strValue.c_str());
            return types::Value(intValue);
        }
        case types::TypeId::BOOLEAN: {
            bool boolValue = (strValue == "1") ? true : false;
            return types::Value(boolValue);
        }
        case types::TypeId::VARCHAR: {
            std::string fieldStr = strValue;
            while(fieldStr.length() < fieldSpec.getStringLength()) {
                fieldStr += " ";
            }
            return types::Value(fieldStr);
        }
        case types::TypeId::FLOAT32:
        case types::TypeId::NUMERIC: {
            float_t floatValue = std::atof(strValue.c_str());
            return types::Value(floatValue);
        }
        case types::TypeId::DATE: {
            boost::gregorian::date date(boost::gregorian::from_string(strValue));
            boost::gregorian::date epochStart(1970, 1, 1);
            int64_t epochTime = (date - epochStart).days() * 24 * 3600;
            return types::Value(epochTime);
        }
        default:
            throw "Unsupported type for string decoding: " + TypeUtilities::getTypeIdString(fieldSpec.getType()) + "\n";


    };
}
