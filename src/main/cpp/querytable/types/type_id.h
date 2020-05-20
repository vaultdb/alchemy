//
// Created by madhav on 1/15/20.
//

#ifndef TESTING_TYPE_ID_H
#define TESTING_TYPE_ID_H
namespace vaultdb::types {
enum class TypeId {
  INVALID = 0,
  BOOLEAN,
  INTEGER32,
  INTEGER64,
  FLOAT32,
  FLOAT64,
  VAULT_DOUBLE,
  NUMERIC,
  TIMESTAMP,
  TIME,
  DATE,
  VARCHAR,
  ENCRYPTED_INTEGER32,
  ENCRYPTED_INTEGER64,
  ENCRYPTED_BOOLEAN,
  ENCRYPTED_FLOAT32
};

} // namespace vaultdb::types

#endif // TESTING_TYPE_ID_H
