//
// Created by shashank on 8/7/20.
//

#ifndef VAULTDB_EMP_ENCRYPTED_FLOAT32_TYPE_H
#define VAULTDB_EMP_ENCRYPTED_FLOAT32_TYPE_H

#include "querytable/types/type.h"
#include "querytable/types/unencrypted/integer_type.h"
#include <memory>
#include <querytable/types/value.h>


namespace vaultdb::types {
class EncryptedFloat32Type : public Type {
public:
  EncryptedFloat32Type();
  static EncryptedFloat32Type &shared_instance() {
    static EncryptedFloat32Type type;
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

