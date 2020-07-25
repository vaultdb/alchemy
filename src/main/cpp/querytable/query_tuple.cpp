//
// Created by madhav on 1/13/20.
//

#include "query_tuple.h"
#include <querytable/expression/expression.h>

using namespace vaultdb;

const QueryField *QueryTuple::GetField(int ordinal) const {
  return &this->fields_[ordinal];
}

QueryField *QueryTuple::GetMutableField(int ordinal) {
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
  dummy_flag_.SetValue(v);
}

void QueryTuple::SetDummyFlag(bool aFlag) {
    dummy_flag_.SetValue(types::TypeId::BOOLEAN, aFlag);
}

void QueryTuple::InitDummy() {
  if (is_encrypted_) {
    dummy_flag_.SetValue(types::TypeId::ENCRYPTED_BOOLEAN, emp::Bit(false));
  } else {
    dummy_flag_.SetValue(types::TypeId::BOOLEAN, false);
  }
}

const vaultdb::types::Value *QueryTuple::GetDummyFlag() {
  return &this->dummy_flag_;
}
vaultdb::types::Value *QueryTuple::GetMutableDummyFlag() {
  return &this->dummy_flag_;
}

void QueryTuple::SetIsEncrypted(bool isEncrypted) {
  is_encrypted_ = isEncrypted;
}


QueryTuple* QueryTuple::reveal(EmpParty party) const {
    // TODO: set it so that when it is XOR-encoded, it is encrypted
    // this has downstream effects that need to be figured out first

   /* QueryTuple *result = new QueryTuple(false);

    bool revealedDummyFlag = (is_encrypted_) ? dummy_flag_.GetEmpBit()->reveal<bool>((int) party)
            : dummy_flag_.GetBool();

    result->SetDummyFlag(revealedDummyFlag);

    for(int i = 0; i < num_fields_; ++i) {
        result->fields_[i] = this->fields_[i].reveal(party);
    }

    return result;*/

   return nullptr;

}
