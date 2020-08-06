#include <util/emp_manager.h>
#include "query_tuple.h"
#include "query_schema.h"

using namespace vaultdb;

QueryTuple::QueryTuple(size_t aFieldCount) {
    dummy_tag_.setValue(false);
    fieldCount_ = aFieldCount;
    fields_ =
        std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);
};

QueryTuple::QueryTuple(size_t fieldCount, bool is_encrypted) : is_encrypted_(is_encrypted), fieldCount_(fieldCount) {
    if (is_encrypted_) {
        dummy_tag_.setValue(emp::Bit(false));
    } else {
        dummy_tag_.setValue(false);
    }

    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);

}


QueryTuple::QueryTuple(QueryTuple &src) :
        is_encrypted_(src.is_encrypted_),
        dummy_tag_(src.dummy_tag_),
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
  fields_[ordinal].SetValue(f->GetValue());
}

void QueryTuple::SetDummyTag(vaultdb::types::Value *v) {
    dummy_tag_.setValue(v);
}

void QueryTuple::SetDummyTag(bool aTag) {
    dummy_tag_.setValue(aTag);
}

void QueryTuple::InitDummy() {
  if (is_encrypted_) {
      dummy_tag_.setValue(emp::Bit(false));
  } else {
      dummy_tag_.setValue(false);
  }
}

const vaultdb::types::Value *QueryTuple::GetDummyTag() {
  return &this->dummy_tag_;
}
vaultdb::types::Value *QueryTuple::GetMutableDummyTag() {
  return &this->dummy_tag_;
}

void QueryTuple::SetIsEncrypted(bool isEncrypted) {
  is_encrypted_ = isEncrypted;
}


/*QueryTuple QueryTuple::reveal(EmpParty party) const {
    // TODO: set it so that when it is XOR-encoded, it is encrypted
    // this has downstream effects that need to be figured out first

   /* QueryTuple *result = new QueryTuple(false);

    bool revealedDummyFlag = (is_encrypted_) ? dummy_tag_.getEmpBit()->reveal<bool>((int) party)
            : dummy_tag_.getBool();

    result->SetDummyTag(revealedDummyFlag);

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

    strm << ") (dummy=" << aTuple.dummy_tag_.getValueString() + ")";

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

// only works for unencrypted tables
void QueryTuple::serialize(bool *dst, QuerySchema *schema) {

    assert(!is_encrypted_);

    bool *cursor = dst;

    for(int fieldIdx = 0; fieldIdx < fieldCount_; ++fieldIdx) {
        fields_[fieldIdx].serialize(cursor);
        cursor += schema->GetField(fieldIdx)->size();
    }

    *cursor = dummy_tag_.getBool();

}

size_t QueryTuple::getFieldCount() const {
    return fieldCount_;
}


QueryTuple& QueryTuple::operator=(const QueryTuple& src) {
    if(&src == this)
        return *this;


    this->is_encrypted_ = src.is_encrypted_;
    this->dummy_tag_ = src.dummy_tag_;
    this->fieldCount_ = src.fieldCount_;

    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);


    for(int i = 0; i < fieldCount_; ++i) {
        fields_[i].initialize(src.fields_[i]);
    }

    return *this;

}

QueryTuple QueryTuple::reveal(EmpParty party) const {
    QueryTuple dstTuple(fieldCount_, false);

    for(int i = 0; i < fieldCount_; ++i) {
        QueryField dstField = fields_[i].reveal(party);
        dstTuple.PutField(i, &dstField);
    }

    EmpManager *empManager = EmpManager::getInstance();
    empManager->flush();


    emp::Bit *dummyTag = dummy_tag_.getEmpBit();
    bool revealedBit = dummyTag->reveal((int) party);


    dstTuple.SetDummyTag(revealedBit);
    return dstTuple;

}


