//
// Created by madhav on 1/15/20.
//

#ifndef TESTING_INTEGER_TYPE_H
#define TESTING_INTEGER_TYPE_H

#pragma once
#include "querytable/types/type.h"
#include "querytable/types/value.h"
namespace vaultdb::types {

class IntegerType : public Type {
public:
  IntegerType();
  static IntegerType &shared_instance() {
    static IntegerType type;
    return type;
  }

  [[nodiscard]] Value CompareEquals(const vaultdb::types::Value &left,
                                    const Value &right) const override;

  [[nodiscard]] Value CompareNotEquals(const Value &left,
                                       const Value &right) const override;
  [[nodiscard]] Value CompareLessThanOrEqual(const Value &left,
                                             const Value &right) const override;
  [[nodiscard]] Value CompareLessThan(const Value &left,
                                      const Value &right) const override;
  [[nodiscard]] Value CompareGreaterThan(const Value &left,
                                         const Value &right) const override;
  Value And(const Value &left, const Value &right) const override;
  Value Or(const Value &left, const Value &right) const override;
  void Swap(const Value &compareBit, Value &left, Value &right) override;
};
} // namespace vaultdb::types

#endif // TESTING_INTEGER_TYPE_H
