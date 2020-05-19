//
// Created by salome on 5/15/20.
//

#include "float_type.h"
#include "common/macros.h"
#include "querytable/types/type_id.h"

namespace vaultdb::types {
#define FLOAT_BINARY(OP)                                                        \
  do {                                                                         \
                                                                               \
    return Value(TypeId::BOOLEAN, left.value_.unencrypted_val.double_val OP           \
                                 right.value_.unencrypted_val.double_val);       \
  } while (0)

Value vaultdb::types::FloatType::CompareEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  FLOAT_BINARY(==);
}
Value vaultdb::types::FloatType::CompareNotEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  FLOAT_BINARY(!=);
}
Value FloatType::CompareLessThanOrEqual(const Value &left,
                                          const Value &right) const {
  FLOAT_BINARY(<=);
}
Value FloatType::CompareLessThan(const Value &left,
                                   const Value &right) const {
  FLOAT_BINARY(<);
}
Value FloatType::CompareGreaterThan(const Value &left,
                                      const Value &right) const {
  FLOAT_BINARY(>);
}

Value FloatType::And(const Value &left, const Value &right) const { throw; }

Value FloatType::Or(const Value &left, const Value &right) const { throw; }

void FloatType::Swap(const Value &compareBit, Value &left, Value &right) {
  VAULTDB_ASSERT(compareBit.GetType() == vaultdb::types::TypeId::BOOLEAN);
  if (compareBit.value_.unencrypted_val.bool_val) {
    double tmp = left.value_.unencrypted_val.double_val;
    left.value_.unencrypted_val.double_val =
        right.value_.unencrypted_val.double_val;
    right.value_.unencrypted_val.double_val = tmp;
  }
}

FloatType::FloatType() {}
} // namespace vaultdb::types
