//
// Created by madhav on 1/15/20.
//

#ifndef ENCRYPTED_INTEGER_TYPE_H
#define ENCRYPTED_INTEGER_TYPE_H

#include "querytable/types/type.h"
#include "querytable/types/unencrypted/integer_type.h"
#include <memory>
#include <querytable/types/value.h>


namespace vaultdb::types {
class EncryptedIntegerType : public Type {
public:
  EncryptedIntegerType();
  static EncryptedIntegerType &shared_instance() {
    static EncryptedIntegerType type;
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

#endif // TESTING_ENCRYPTED_INTEGER_TYPE_H
