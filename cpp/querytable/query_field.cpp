//
// Created by madhav on 1/13/20.
//

#include "query_field.h"
using namespace vaultdb;

QueryField::QueryField(const QueryField &qf)
    : field_num_(qf.field_num_), value_(qf.value_) {}

QueryField::QueryField(int64_t val, int fn)
    : field_num_(fn), value_(types::TypeId::INTEGER64, val) {}

QueryField::QueryField(int32_t val, int fn)
    : field_num_(fn), value_(types::TypeId::INTEGER32, val) {}

QueryField::QueryField(emp::Integer val, int length, int field_num)
    : field_num_(field_num),
      value_(types::TypeId::ENCRYPTED_INTEGER64, val, length) {}

const types::Value *QueryField::GetValue() const { return &value_; }
QueryField::QueryField() {}
/*
void QueryField::SetQueryField(int64_t val, int field_num) {
  value_3.SetValue(types::TypeId::INTEGER64, val);
  field_num_ = field_num;
}
void QueryField::SetQueryField(int32_t val, int field_num) {
  value_3.SetValue(types::TypeId::INTEGER32, val);
  field_num_ = field_num;
}
void QueryField::SetQueryField(emp::Integer val, int length, int field_num) {
  // TODO(madhavsuresh): there could be a bug here, automatic upgrade to longer
  // length
  value_3.SetValue(types::TypeId::ENCRYPTED_INTEGER64, val, length);
  field_num_ = field_num;
}
 */

void QueryField::SetValue(const types::Value *val) { value_.SetValue(val); }
