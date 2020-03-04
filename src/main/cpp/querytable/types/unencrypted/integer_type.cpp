//
// Created by madhav on 1/15/20.
//

#include "integer_type.h"
#include "common/macros.h"
#include "querytable/types/type_id.h"

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
Value IntegerType::CompareGreaterThan(const Value &left,
					       const Value &right) const {
  INT_CMP(>);
}

Value IntegerType::And(const Value &left, const Value &right) const { throw; }

Value IntegerType::Or(const Value &left, const Value &right) const { throw; }

void IntegerType::Swap(const Value &compareBit, Value &left,
					Value &right) {
  VAULTDB_ASSERT(compareBit.GetType() == vaultdb::types::TypeId::BOOLEAN);
  //TODO(madhavsuresh): make this oblivious
  if (compareBit.value_.unencrypted_val.bool_val) {
    switch (left.type_) {
    case TypeId::INTEGER32: {
      int32_t tmp = left.value_.unencrypted_val.int32_val;
      left.value_.unencrypted_val.int32_val =
	  right.value_.unencrypted_val.int32_val;
      right.value_.unencrypted_val.int32_val = tmp;
      break;
    }
    case TypeId::INTEGER64: {
      int64_t tmp = left.value_.unencrypted_val.int64_val;
      left.value_.unencrypted_val.int64_val =
	  right.value_.unencrypted_val.int64_val;
      right.value_.unencrypted_val.int64_val = tmp;
      break;
    }
    default:
      throw;
    }
  }
}

IntegerType::IntegerType() {}
} // namespace vaultdb::types
