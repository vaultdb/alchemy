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

types::Value *QueryField::GetMutableValue() { return &value_; }

const types::Value *QueryField::GetValue() const { return &value_; }

QueryField::QueryField() {}

void QueryField::SetValue(const types::Value *val) { value_.SetValue(val); }
