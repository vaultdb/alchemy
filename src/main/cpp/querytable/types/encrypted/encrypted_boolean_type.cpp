//
// Created by madhav on 2/5/20.
//

#include "encrypted_boolean_type.h"
namespace vaultdb::types {

#define EMP_BOOL_CMP(OP)                                                       \
  do {                                                                         \
    emp::Bit b = *left.value_.emp_bit_ OP * right.value_.emp_bit_;             \
    return Value(left.type_, b);                                               \
  } while (0)

#define EMP_BOOL_BINARY(OP)                                                    \
  do {                                                                         \
    emp::Bit b = *left.value_.emp_bit_ OP * right.value_.emp_bit_;             \
    return Value(left.type_, b);                                               \
  } while (0)

Value vaultdb::types::EncryptedBooleanType::CompareEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  EMP_BOOL_CMP(==);
}
Value EncryptedBooleanType::CompareNotEquals(const Value &left,
					     const Value &right) const {
  EMP_BOOL_CMP(!=);
}
Value EncryptedBooleanType::CompareLessThanOrEqual(const Value &left,
						   const Value &right) const {
  throw;
}
Value EncryptedBooleanType::CompareGreaterThan(const Value &left,
					       const Value &right) const {
  throw;
}
Value EncryptedBooleanType::And(const Value &left, const Value &right) const {
  EMP_BOOL_BINARY (&);
}

Value EncryptedBooleanType::Or(const Value &left, const Value &right) const {
  EMP_BOOL_BINARY (|);
}
void EncryptedBooleanType::Swap(const Value &compareBit, Value &left, Value &right){throw;};
} // namespace vaultdb::types
