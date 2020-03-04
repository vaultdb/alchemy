//
// Created by madhav on 1/15/20.
//

#include "encrypted_integer_type.h"
#include <common/macros.h>
#include <emp-tool/circuits/swappable.h>
namespace vaultdb::types {
#define EMP_INT_CMP(OP)                                                        \
  do {                                                                         \
    emp::Bit b = *left.value_.emp_integer_ OP * right.value_.emp_integer_;     \
    return Value(TypeId::ENCRYPTED_BOOLEAN, b);                                \
  } while (0)

#define EMP_INT_BINARY(OP)                                                     \
  do {                                                                         \
    emp::Integer b = *left.value_.emp_integer_ OP * right.value_.emp_integer_; \
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
  EMP_INT_BINARY (&);
}

Value EncryptedIntegerType::Or(const Value &left, const Value &right) const {
  EMP_INT_BINARY (|);
}
Value EncryptedIntegerType::CompareLessThanOrEqual(const Value &left,
						   const Value &right) const {
  EMP_INT_CMP(<=);
}
Value EncryptedIntegerType::CompareGreaterThan(const Value &left,
					       const Value &right) const {
  EMP_INT_CMP(>);
}

void EncryptedIntegerType::Swap(const Value &compareBit, Value &left,
				Value &right) {
  VAULTDB_ASSERT(compareBit.GetType() ==
		 vaultdb::types::TypeId::ENCRYPTED_BOOLEAN);
  emp::swap(*compareBit.value_.emp_bit_, *left.value_.emp_integer_,
	    *right.value_.emp_integer_);
}

} // namespace vaultdb::types
