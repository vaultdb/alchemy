//
// Created by madhav on 1/15/20.
//

#include "encrypted_integer_type.h"
#include <common/macros.h>
#include <emp-tool/circuits/swappable.h>
namespace vaultdb::types {
#define EMP_INT_CMP(OP)                                                        \
  do {                                                                         \
    emp::Bit b = *(left.getEmpInt()) OP *(right.getEmpInt());     \
    return Value( b);                                \
  } while (0)

#define EMP_INT_BINARY(OP)                                                     \
  do {                                                                         \
    emp::Integer b = *(left.getEmpInt()) OP *(right.getEmpInt(); \
    return Value(left.getType(), b);                                    \
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
    emp::Integer lhs = *left.getEmpInt();
    emp::Integer rhs = *right.getEmpInt();
    emp::Integer result = lhs & rhs;
    return Value(left.getType(), result);

}

Value EncryptedIntegerType::Or(const Value &left, const Value &right) const {
    emp::Integer lhs = *left.getEmpInt();
    emp::Integer rhs = *right.getEmpInt();
    emp::Integer result = lhs | rhs;
    return Value(left.getType(), result);

}
Value EncryptedIntegerType::CompareLessThanOrEqual(const Value &left,
						   const Value &right) const {
  EMP_INT_CMP(<=);
}
Value EncryptedIntegerType::CompareLessThan(const Value &left,
					    const Value &right) const {
  EMP_INT_CMP(<);
}
Value EncryptedIntegerType::CompareGreaterThan(const Value &left,
					       const Value &right) const {
  EMP_INT_CMP(>);
}

void EncryptedIntegerType::Swap(const Value &compareBit, Value &left,
				Value &right) {
  VAULTDB_ASSERT(compareBit.getType() ==
                 vaultdb::types::TypeId::ENCRYPTED_BOOLEAN);
  emp::Integer lhs = *(left.getEmpInt());
  emp::Integer rhs = *(right.getEmpInt());
  emp::Bit choiceBit = *(compareBit.getEmpBit());


  emp::swap(choiceBit, lhs, rhs);
  left.setValue(left.getType(), lhs);
  right.setValue(right.getType(), rhs);

}

} // namespace vaultdb::types
