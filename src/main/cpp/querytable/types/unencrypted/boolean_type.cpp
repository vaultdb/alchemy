//
// Created by madhav on 2/5/20.
//

#include "boolean_type.h"
#include <common/macros.h>
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
  BOOL_BINARY(&);
}
Value vaultdb::types::BooleanType::Or(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  BOOL_BINARY(|);
}

Value BooleanType::CompareLessThanOrEqual(const Value &left,
                                          const Value &right) const {
  bool b1 = (!left.value_.unencrypted_val.bool_val) &
            right.value_.unencrypted_val.bool_val;
  bool b2 = left.value_.unencrypted_val.bool_val ==
            right.value_.unencrypted_val.bool_val;
  return Value(left.GetType(), (b1 | b2));
}
Value BooleanType::CompareLessThan(const Value &left,
                                   const Value &right) const {
  bool b = (!left.value_.unencrypted_val.bool_val) &
           right.value_.unencrypted_val.bool_val;
  return Value(left.GetType(), b);
  throw;
}
Value BooleanType::CompareGreaterThan(const Value &left,
                                      const Value &right) const {
  bool b = (left.value_.unencrypted_val.bool_val) &
           (!right.value_.unencrypted_val.bool_val);
  return Value(left.GetType(), b);
}
void BooleanType::Swap(const Value &compareBit, Value &left, Value &right) {
  VAULTDB_ASSERT(compareBit.GetType() == vaultdb::types::TypeId::BOOLEAN);
  if (compareBit.value_.unencrypted_val.bool_val) {
    bool tmp = left.value_.unencrypted_val.bool_val;
    left.value_.unencrypted_val.bool_val =
        right.value_.unencrypted_val.bool_val;
    right.value_.unencrypted_val.bool_val = tmp;
  }
};
} // namespace vaultdb::types
