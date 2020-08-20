#include "encrypted_boolean_type.h"
#include <common/macros.h>
namespace vaultdb::types {

#define EMP_BOOL_CMP(OP)                                                       \
  do {                                                                         \
    emp::Bit b = (*left.getEmpBit()) OP (*right.getEmpBit());             \
    return Value(b);                                               \
  } while (0)

#define EMP_BOOL_BINARY(OP)                                                    \
  do {                                                                         \
    emp::Bit b = (*left.getEmpBit()) OP (*right.getEmpBit());             \
    return Value(b);                                               \
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
  emp::Bit b1 = (!*(left.getEmpBit()) & *(right.getEmpBit()));
  emp::Bit b2 = *(left.getEmpBit()) == *(right.getEmpBit());
  return Value((b1 | b2));
}
Value EncryptedBooleanType::CompareLessThan(const Value &left,
                                            const Value &right) const {
  emp::Bit b = (!*(left.getEmpBit())) & *(right.getEmpBit());
  return Value(b);
}
Value EncryptedBooleanType::CompareGreaterThan(const Value &left,
                                               const Value &right) const {
  emp::Bit b = *(left.getEmpBit()) & !*(right.getEmpBit());
  return Value(b);
}
Value EncryptedBooleanType::And(const Value &left, const Value &right) const {
  EMP_BOOL_BINARY(&);
}

Value EncryptedBooleanType::Or(const Value &left, const Value &right) const {
  EMP_BOOL_BINARY(|);
}
void EncryptedBooleanType::Swap(const Value &compareBit, Value &left,
                                Value &right) {
  VAULTDB_ASSERT(compareBit.getType() ==
                 vaultdb::types::TypeId::ENCRYPTED_BOOLEAN);
  emp::swap(*(compareBit.getEmpBit()), *(left.getEmpBit()),
            *(right.getEmpBit()));
};
} // namespace vaultdb::types
