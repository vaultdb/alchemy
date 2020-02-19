//
// Created by madhav on 1/15/20.
//

#include "bool_result.h"
vaultdb::types::BoolResult::BoolResult(emp::Bit bit) : is_encrypted_(true) {
  encrypted_bit_ = std::make_unique<emp::Bit>(bit.bit);
}
vaultdb::types::BoolResult::BoolResult(bool bit) : is_encrypted_(false) {
  unencrypted_bit_ = bit;
}
