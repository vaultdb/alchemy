//
// Created by madhav on 2/5/20.
//

#ifndef TESTING_ENCRYPTED_BOOLEAN_TYPE_H
#define TESTING_ENCRYPTED_BOOLEAN_TYPE_H

#include <querytable/types/type.h>
namespace vaultdb::types {
class EncryptedBooleanType : public Type {

public:
  static EncryptedBooleanType &shared_instance() {
    static EncryptedBooleanType type;
    return type;
  }
  [[nodiscard]] Value CompareEquals(const Value &left,
                                    const Value &right) const override;
  [[nodiscard]] Value CompareNotEquals(const Value &left,
                                       const Value &right) const override;

  [[nodiscard]] virtual Value CompareLessThanOrEqual(const Value &left,
					       const Value &right) const override;;

  [[nodiscard]] virtual Value CompareGreaterThan(const Value &left,
					       const Value &right) const override;
  [[nodiscard]] Value And(const Value &left, const Value &right) const override;
  [[nodiscard]] Value Or(const Value &left, const Value &right) const override;

  void Swap(const Value &compareBit, Value &left,
				   Value &right) override;
};
} // namespace vaultdb::types

#endif // TESTING_ENCRYPTED_BOOLEAN_TYPE_H
