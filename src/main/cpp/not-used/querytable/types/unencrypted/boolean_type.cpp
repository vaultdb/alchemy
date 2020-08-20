//
// Created by madhav on 2/5/20.
//

#include "boolean_type.h"
#include <common/macros.h>
namespace vaultdb::types {
#define BOOL_BINARY(OP)                                                        \
  do {                                                                         \
           bool result = left.getBool() OP  right.getBool();                                                                     \
           return Value(result );       \
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
  bool b1 = (!left.getBool()) &
            right.getBool();
  bool b2 = left.getBool() ==
            right.getBool();
  return Value((b1 | b2));
}
Value BooleanType::CompareLessThan(const Value &left,
                                   const Value &right) const {
  bool b = (!left.getBool()) &
           right.getBool();
  return Value(b);
  throw;
}
Value BooleanType::CompareGreaterThan(const Value &left,
                                      const Value &right) const {
  bool b = (left.getBool()) &
           (!right.getBool());
  return Value(b);
}
void BooleanType::Swap(const Value &compareBit, Value &left, Value &right) {
  VAULTDB_ASSERT(compareBit.getType() == vaultdb::types::TypeId::BOOLEAN);
  if (compareBit.getBool()) {
    bool tmp = left.getBool();
    left.setValue(right.getBool());
    right.setValue(tmp);
  }
};
} // namespace vaultdb::types
