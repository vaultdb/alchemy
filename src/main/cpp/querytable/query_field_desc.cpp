//
// Created by madhav on 1/13/20.
//

#include "query_field_desc.h"

int QueryFieldDesc::GetColumnNumber() const {
  return QueryFieldDesc::column_number_;
}

bool QueryFieldDesc::GetIsPrivate() const {
  return QueryFieldDesc::is_private_;
}

const std::string &QueryFieldDesc::GetName() const {
  return QueryFieldDesc::name_;
}

vaultdb::types::TypeId QueryFieldDesc::GetType() const { return QueryFieldDesc::type_; }
const std::string &QueryFieldDesc::GetTableName() const {
  return QueryFieldDesc::table_name;
}
