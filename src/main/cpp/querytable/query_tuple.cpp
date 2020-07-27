//
// Created by madhav on 1/13/20.
//

#include "query_tuple.h"
#include <querytable/expression/expression.h>

using namespace vaultdb;

QueryTuple::QueryTuple(size_t aFieldCount) {
    dummy_flag_.setValue(false);
    fieldCount_ = aFieldCount;
    fields_ =
        std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);
};

QueryTuple::QueryTuple(size_t fieldCount, bool is_encrypted) : is_encrypted_(is_encrypted), fieldCount_(fieldCount) {
    if (is_encrypted_) {
        dummy_flag_.setValue(emp::Bit(false));
    } else {
        dummy_flag_.setValue(false);
    }

    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);

}


QueryTuple::QueryTuple(QueryTuple &src) :
    is_encrypted_(src.is_encrypted_),
    dummy_flag_(src.dummy_flag_),
    fieldCount_(src.fieldCount_)
{
    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);


    for(int i = 0; i < fieldCount_; ++i) {
        fields_[i].initialize(src.fields_[i]);
    }

}

const QueryField *QueryTuple::GetField(int ordinal) const {
  return &this->fields_[ordinal];
}

QueryField *QueryTuple::GetMutableField(int ordinal) {
  return &this->fields_[ordinal];
}

void QueryTuple::PutField(int ordinal, std::unique_ptr<QueryField> f) {
  fields_[ordinal].SetValue(f->GetValue());

}

void QueryTuple::PutField(int ordinal, const QueryField *f) {
  if (ordinal >= 10) {
    throw;
  }
  fields_[ordinal].SetValue(f->GetValue());
}

void QueryTuple::SetDummyFlag(vaultdb::types::Value *v) {
    dummy_flag_.setValue(v);
}

void QueryTuple::SetDummyFlag(bool aFlag) {
    dummy_flag_.setValue(aFlag);
}

void QueryTuple::InitDummy() {
  if (is_encrypted_) {
      dummy_flag_.setValue(emp::Bit(false));
  } else {
      dummy_flag_.setValue(false);
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


/*QueryTuple QueryTuple::reveal(EmpParty party) const {
    // TODO: set it so that when it is XOR-encoded, it is encrypted
    // this has downstream effects that need to be figured out first

   /* QueryTuple *result = new QueryTuple(false);

    bool revealedDummyFlag = (is_encrypted_) ? dummy_flag_.getEmpBit()->reveal<bool>((int) party)
            : dummy_flag_.getBool();

    result->SetDummyFlag(revealedDummyFlag);

    for(int i = 0; i < num_fields_; ++i) {
        result->fields_[i] = this->fields_[i].reveal(party);
    }

    return result;

   QueryTuple placeholder;
   return placeholder;

}*/

std::ostream &vaultdb::operator<<(std::ostream &strm,  const QueryTuple &aTuple) {
    strm << "(" << *(aTuple.GetField(0));

    for(int i = 1; i < aTuple.fieldCount_; ++i)
        strm << ", " << *(aTuple.GetField(i));

    strm << ")";

    return strm;



}

QueryTuple::QueryTuple() {
    fieldCount_  = 0;


}

void QueryTuple::setFieldCount(size_t fieldCount) {
    fieldCount_ = fieldCount;

    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);
}



