#include "integer_type.h"
#include "common/macros.h"
#include "querytable/types/type_id.h"

namespace vaultdb::types {
#define INT_CMP(OP)                                                            \
  do {                                                                         \
    switch (left.getType()) {                                                      \
    case TypeId::INTEGER32: {                                                  \
        bool result = left.getInt32() OP  right.getInt32(); \
      return Value(result);                \
    }                                                                          \
    case TypeId::INTEGER64: {                                                  \
    bool result = left.getInt64() OP right.getInt64(); \
      return Value(result);                \
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
Value IntegerType::CompareLessThanOrEqual(const Value &left,
                                          const Value &right) const {
  INT_CMP(<=);
}
Value IntegerType::CompareLessThan(const Value &left,
                                   const Value &right) const {
  INT_CMP(<);
}
Value IntegerType::CompareGreaterThan(const Value &left,
                                      const Value &right) const {
  INT_CMP(>);
}

Value IntegerType::And(const Value &left, const Value &right) const { throw; }

Value IntegerType::Or(const Value &left, const Value &right) const { throw; }

void IntegerType::Swap(const Value &compareBit, Value &left, Value &right) {
  assert(compareBit.getType() == vaultdb::types::TypeId::ENCRYPTED_BOOLEAN);

  emp::Bit cmp = *(compareBit.getEmpBit());
  emp::Integer lhs = *(left.getEmpInt());
  emp::Integer rhs = *(right.getEmpInt());

  emp::swap(cmp, lhs, rhs);
  left.setValue(&lhs);
  right.setValue(&rhs);

}

IntegerType::IntegerType() {}
} // namespace vaultdb::types
