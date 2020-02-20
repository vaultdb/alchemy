//
// Created by madhav on 1/15/20.
//

#ifndef TESTING_BOOL_RESULT_H
#define TESTING_BOOL_RESULT_H

#include "emp-tool/emp-tool.h"
#include <memory>

namespace vaultdb::types {
class BoolResult {
public:
  explicit BoolResult(emp::Bit bit);
  explicit BoolResult(bool bit);

protected:
  const bool is_encrypted_;
  std::unique_ptr<emp::Bit> encrypted_bit_;
  bool unencrypted_bit_{};
};
} // namespace vaultdb::types

#endif // TESTING_BOOL_RESULT_H
