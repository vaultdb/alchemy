//
// Created by madhav on 1/15/20.
//

#include "type.h"
#include <querytable/types/encrypted/encrypted_boolean_type.h>
#include <querytable/types/encrypted/encrypted_integer_type.h>
#include <querytable/types/unencrypted/boolean_type.h>
#include <querytable/types/unencrypted/integer_type.h>

vaultdb::types::Type &vaultdb::types::Type::GetInstance(TypeId type_id) {
  switch (type_id) {

  case TypeId::INVALID:
    throw;
  case TypeId::BOOLEAN:
    return BooleanType::shared_instance();
  case TypeId::INTEGER32:
    return IntegerType::shared_instance();
  case TypeId::INTEGER64:
    return IntegerType::shared_instance();
  case TypeId::FLOAT32:
    break;
  case TypeId::FLOAT64:
    break;
  case TypeId::DOUBLE:
    break;
  case TypeId::NUMERIC:
    break;
  case TypeId::TIMESTAMP:
    break;
  case TypeId::TIME:
    break;
  case TypeId::DATE:
    break;
  case TypeId::VARCHAR:
    break;
  case TypeId::ENCRYPTED_INTEGER32:
    break;
  case TypeId::ENCRYPTED_INTEGER64:
    return EncryptedIntegerType::shared_instance();
  case TypeId::ENCRYPTED_BOOLEAN:
    return EncryptedBooleanType::shared_instance();
  }
}
