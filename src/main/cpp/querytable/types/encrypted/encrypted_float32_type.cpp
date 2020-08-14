//
// Created by shashank on 8/7/20.
//

#include "encrypted_float32_type.h"
#include <common/macros.h>
#include <emp-tool/circuits/swappable.h>
namespace vaultdb::types {

Value EncryptedFloat32Type::CompareEquals(const Value &left,
                                          const Value &right) const {
  emp::Float32 *lhs = left.getEmpFloat32();
  emp::Float32 *rhs = right.getEmpFloat32();
  return lhs->equal(*rhs);
}

Value EncryptedFloat32Type::CompareNotEquals(const Value &left,
                                             const Value &right) const {
  emp::Float32 *lhs = left.getEmpFloat32();
  emp::Float32 *rhs = right.getEmpFloat32();
  return !lhs->equal(*rhs);
}

EncryptedFloat32Type::EncryptedFloat32Type() {}
Value EncryptedFloat32Type::And(const Value &left, const Value &right) const {
  emp::Float32 lhs = *left.getEmpFloat32();
  emp::Float32 rhs = *right.getEmpFloat32();
  emp::Float32 result = lhs & rhs;
  return Value(result);
}

Value EncryptedFloat32Type::Or(const Value &left, const Value &right) const {
  throw;
}

Value EncryptedFloat32Type::CompareLessThanOrEqual(const Value &left,
                                                   const Value &right) const {
  emp::Float32 *lhs = left.getEmpFloat32();
  emp::Float32 *rhs = right.getEmpFloat32();
  return lhs->less_equal(*rhs);
}

Value EncryptedFloat32Type::CompareLessThan(const Value &left,
                                            const Value &right) const {
  emp::Float32 *lhs = left.getEmpFloat32();
  emp::Float32 *rhs = right.getEmpFloat32();
  return lhs->less_than(*rhs);
}

Value EncryptedFloat32Type::CompareGreaterThan(const Value &left,
                                               const Value &right) const {
  emp::Float32 *lhs = left.getEmpFloat32();
  emp::Float32 *rhs = right.getEmpFloat32();
  return !lhs->less_than(*rhs);
}

void EncryptedFloat32Type::Swap(const Value &compareBit, Value &left,
                                Value &right) {
  VAULTDB_ASSERT(compareBit.getType() ==
                 vaultdb::types::TypeId::ENCRYPTED_BOOLEAN);

  emp::Float32 lhs = *(left.getEmpFloat32());
  emp::Float32 rhs = *(right.getEmpFloat32());
  emp::Bit choiceBit = *(compareBit.getEmpBit());


  emp::swap(choiceBit, lhs, rhs);
  left.setValue(lhs);
  right.setValue(rhs);

}

} // namespace vaultdb::types

