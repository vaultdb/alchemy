//
// Created by madhav on 2/5/20.
//

#ifndef TESTING_BOOLEAN_TYPE_H
#define TESTING_BOOLEAN_TYPE_H

#include <querytable/types/type.h>
namespace vaultdb::types {

class BooleanType : public Type {

public:
  static BooleanType &shared_instance() {
    static BooleanType type;
    return type;
  }
  [[nodiscard]] Value CompareEquals(const Value &left,
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

#endif // TESTING_BOOLEAN_TYPE_H
