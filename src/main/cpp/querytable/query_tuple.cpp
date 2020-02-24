//
// Created by madhav on 1/13/20.
//

#include "query_tuple.h"
#include <querytable/expression/expression.h>

using namespace vaultdb;

const QueryField *QueryTuple::GetField(int ordinal) const {
  return &this->fields_[ordinal];
}

void QueryTuple::PutField(int ordinal, std::unique_ptr<QueryField> f) {
  if (ordinal >= 10) {
    throw;
  }
  fields_[ordinal].SetValue(f->GetValue());
}

void QueryTuple::PutField(int ordinal, const QueryField *f) {
  if (ordinal >= 10) {
    throw;
  }
  fields_[ordinal].SetValue(f->GetValue());
  // fields_3[ordinal] = new QueryField(*f);
}

void QueryTuple::SetDummyFlag(vaultdb::types::Value *v) {
  vaultdb::expression::Expression ex(v, &dummy_flag_,
                                     vaultdb::expression::ExpressionId::EQUAL);
  auto result = ex.execute();
  dummy_flag_.SetValue(&result);
}
void QueryTuple::InitDummy() {
  if (is_encrypted_) {
    dummy_flag_.SetValue(types::TypeId::ENCRYPTED_BOOLEAN, emp::Bit(false));
  } else {
    dummy_flag_.SetValue(types::TypeId::BOOLEAN, false);
  }
}
void QueryTuple::SetIsEncrypted(bool isEncrypted) {
  is_encrypted_ = isEncrypted;
}
