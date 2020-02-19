//
// Created by madhav on 1/15/20.
//

#include "encrypted_integer_type.h"
#include <common/macros.h>
namespace vaultdb::types {
#define EMP_INT_CMP(OP)                                                        \
  do {                                                                         \
    emp::Bit b =                                                               \
        *left.value_.emp_integer_ OP * right.value_.emp_integer_;  \
    return Value(TypeId::ENCRYPTED_BOOLEAN, b);                                \
  } while (0)

#define EMP_INT_BINARY(OP)                                                     \
  do {                                                                         \
    emp::Integer b =                                                           \
        *left.value_.emp_integer_ OP * right.value_.emp_integer_;  \
    return Value(left.type_, b, left.len_);                                    \
  } while (0)

Value EncryptedIntegerType::CompareEquals(const Value &left,
                                          const Value &right) const {
  EMP_INT_CMP(==);
}
Value EncryptedIntegerType::CompareNotEquals(const Value &left,
                                             const Value &right) const {
  EMP_INT_CMP(!=);
}
EncryptedIntegerType::EncryptedIntegerType() {}
Value EncryptedIntegerType::And(const Value &left, const Value &right) const {
  EMP_INT_BINARY(^);
}
} // namespace vaultdb::types
