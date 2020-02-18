//
// Created by madhav on 1/13/20.
//

#ifndef TESTING_DUMMY_FLAG_H
#define TESTING_DUMMY_FLAG_H

#include "emp-tool/emp-tool.h"

class DummyFlag {
private:
  bool unencrypted_flag_;
  std::unique_ptr<emp::Bit> encrypted_flag_;
  bool is_encrypted_;

public:
};

#endif // TESTING_DUMMY_FLAG_H
