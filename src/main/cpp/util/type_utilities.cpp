#include "util/type_utilities.h"

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
