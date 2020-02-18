//
// Created by madhav on 1/13/20.
//

#ifndef TESTING_QUERY_FIELD_H
#define TESTING_QUERY_FIELD_H

#include "emp-tool/emp-tool.h"
#include "query_field_desc.h"
#include "types/value.h"
#include <memory>
#include <vector>

namespace vaultdb {

class QueryField {
private:
  int field_num_;
  types::Value value_;

public:
  QueryField();
  QueryField(const QueryField &qf);

  QueryField(int64_t val, int field_num);

  QueryField(int32_t val, int field_num);

  QueryField(emp::Integer, int length, int field_num);

  QueryField(double val, int field_num);

  QueryField(char *val, int field_num);

  QueryField(std::string val, int field_num);

  void SetQueryField(int64_t val, int field_num);

  void SetQueryField(int32_t val, int field_num);

  void SetQueryField(emp::Integer, int length, int field_num);

  [[nodiscard]] const types::Value *GetValue() const;
  void SetValue(const types::Value *val);
};

} // namespace vaultdb
#endif // TESTING_QUERY_FIELD_H
