//
// Created by madhav on 2/5/20.
//

#include "boolean_type.h"
namespace vaultdb::types {
#define BOOL_BINARY(OP)                                                        \
  do {                                                                         \
                                                                               \
    return Value(left.type_, left.value_.unencrypted_val.bool_val OP           \
                                 right.value_.unencrypted_val.bool_val);       \
  } while (0)

Value vaultdb::types::BooleanType::CompareEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  BOOL_BINARY(==);
}
Value vaultdb::types::BooleanType::CompareNotEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  BOOL_BINARY(!=);
}
Value vaultdb::types::BooleanType::And(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  BOOL_BINARY(^);
}
} // namespace vaultdb::types
