//
// Created by madhav on 1/15/20.
//

#pragma once
#include "bool_result.h"
#include "type_id.h"
#include "value.h"
#include <memory>

namespace vaultdb::types {
class Value;

class Type {
public:
  static Type &GetInstance(TypeId type_id);

  [[nodiscard]] virtual Value CompareEquals(const Value &left,
                                                    const Value &right) const = 0;
  [[nodiscard]] virtual Value
  CompareNotEquals(const Value &left, const Value &right) const = 0;

  [[nodiscard]] virtual Value
  And(const Value &left, const Value &right) const = 0;

protected:
  TypeId type_id_;
};

} // namespace vaultdb::types
