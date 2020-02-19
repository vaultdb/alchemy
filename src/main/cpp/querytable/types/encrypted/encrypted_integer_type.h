//
// Created by madhav on 1/15/20.
//

#ifndef TESTING_ENCRYPTED_INTEGER_TYPE_H
#define TESTING_ENCRYPTED_INTEGER_TYPE_H

#include "querytable/types/type.h"
#include "querytable/types/unencrypted/integer_type.h"
#include <memory>

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
  Value And(const Value &left, const Value &right) const override;
};

} // namespace vaultdb::types

#endif // TESTING_ENCRYPTED_INTEGER_TYPE_H
