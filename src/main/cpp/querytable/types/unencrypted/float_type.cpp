//
// Created by salome on 5/15/20.
//

#include "float_type.h"
#include "common/macros.h"
#include "querytable/types/type_id.h"

namespace vaultdb::types {
#define FLOAT_BINARY(OP)                                                        \
  do {                                                                          \
        bool result = 0; \
         switch(left.getType()) {                                               \
            case TypeId::FLOAT32:                                                                    \
                result =  left.getFloat32() OP   right.getFloat32();                            \
                 break;                   \
              case TypeId::FLOAT64:                                             \
                  result = left.getFloat64() OP right.getFloat64();             \
                  break;                                                        \
                  default:                                                      \
                    throw;\
                  }\
    return Value(result);       \
  } while (0)

Value vaultdb::types::FloatType::CompareEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  FLOAT_BINARY(==);
}
Value vaultdb::types::FloatType::CompareNotEquals(
    const vaultdb::types::Value &left,
    const vaultdb::types::Value &right) const {
  FLOAT_BINARY(!=);
}
Value FloatType::CompareLessThanOrEqual(const Value &left,
                                          const Value &right) const {
  FLOAT_BINARY(<=);
}
Value FloatType::CompareLessThan(const Value &left,
                                   const Value &right) const {
  FLOAT_BINARY(<);
}
Value FloatType::CompareGreaterThan(const Value &left,
                                      const Value &right) const {
  FLOAT_BINARY(>);
}

Value FloatType::And(const Value &left, const Value &right) const { throw; }

Value FloatType::Or(const Value &left, const Value &right) const { throw; }

void FloatType::Swap(const Value &compareBit, Value &lhs, Value &rhs) {
  VAULTDB_ASSERT(compareBit.getType() == vaultdb::types::TypeId::BOOLEAN);
  bool cmp = compareBit.getBool();

    if(lhs.getType() == TypeId::FLOAT32) {
        if(cmp) {
            float tmp = lhs.getFloat32();
            lhs.setValue(rhs.getFloat32());
            rhs.setValue(tmp);
        }
    }
    else if(lhs.getType() == TypeId::FLOAT64) {
        if(cmp) {
            double tmp = lhs.getFloat64();
            lhs.setValue(rhs.getFloat64());
            rhs.setValue(tmp);
        }
    }
    else {
        throw;
    }
}

FloatType::FloatType() {}
} // namespace vaultdb::types
