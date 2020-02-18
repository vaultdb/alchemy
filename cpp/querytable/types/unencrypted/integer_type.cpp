//
// Created by madhav on 1/15/20.
//

#include "integer_type.h"

namespace vaultdb::types {
#define INT_CMP(OP)                                                            \
  do {                                                                         \
    switch (left.type_) {                                                      \
    case TypeId::INTEGER32: {                                                  \
      return Value(TypeId::BOOLEAN,                                            \
                   left.value_.unencrypted_val.int32_val OP                    \
                       right.value_.unencrypted_val.int32_val);                \
    }                                                                          \
    case TypeId::INTEGER64: {                                                  \
      return Value(TypeId::BOOLEAN,                                            \
                   left.value_.unencrypted_val.int64_val OP                    \
                       right.value_.unencrypted_val.int64_val);                \
      break;                                                                   \
    }                                                                          \
    default:                                                                   \
      throw;                                                                   \
    }                                                                          \
  } while (0)

Value vaultdb::types::IntegerType::CompareEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  INT_CMP(==);
}
Value vaultdb::types::IntegerType::CompareNotEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  INT_CMP(!=);
}

Value IntegerType::And(const Value &left, const Value &right) const { throw; }
IntegerType::IntegerType() {}
} // namespace vaultdb::types
