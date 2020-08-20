#include <util/emp_manager.h>
#include "query_tuple.h"
#include "query_schema.h"

using namespace vaultdb;

QueryTuple::QueryTuple(const  size_t & aFieldCount) {
    dummy_tag_.setValue(false); // not a dummy
    is_encrypted_ = false; // by default
    fieldCount_ = aFieldCount;
    fields_ =
        std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);
};

QueryTuple::QueryTuple(const size_t & fieldCount, const  bool & is_encrypted) : is_encrypted_(is_encrypted), fieldCount_(fieldCount) {
    if (is_encrypted_) {
        dummy_tag_.setValue(emp::Bit(false));
    } else {
        dummy_tag_.setValue(false);
    }

    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);

}


QueryTuple::QueryTuple(const QueryTuple &src) :
        is_encrypted_(src.is_encrypted_),
        dummy_tag_(src.dummy_tag_),
        fieldCount_(src.fieldCount_)
{
    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);


    for(int i = 0; i < fieldCount_; ++i) {
        fields_[i] = src.fields_[i];
    }

}

const QueryField QueryTuple::getField(int ordinal) const {
    QueryField aField = fields_[ordinal];
    return aField;
}



void QueryTuple::putField(int ordinal, const QueryField &f) {
    const types::Value src = f.getValue();
    fields_[ordinal].setValue(src);
}

void QueryTuple::setDummyTag(types::Value &v) {
    dummy_tag_.setValue(v);
}

void QueryTuple::initDummy() {
  if (is_encrypted_) {
      dummy_tag_.setValue(emp::Bit(false));
  } else {
      dummy_tag_.setValue(false);
  }
}

const vaultdb::types::Value QueryTuple::getDummyTag() {
  return this->dummy_tag_;
}


void QueryTuple::setIsEncrypted(bool isEncrypted) {
  is_encrypted_ = isEncrypted;
}



std::ostream &vaultdb::operator<<(std::ostream &strm,  const QueryTuple &aTuple) {

    if((!aTuple.is_encrypted_ && !(aTuple.dummy_tag_.getBool())) // if it is real
       || aTuple.is_encrypted_) { // or its status is unknown
        strm << "(" << aTuple.getField(0);

        for (int i = 1; i < aTuple.fieldCount_; ++i)
            strm << ", " << aTuple.getField(i);

        strm << ")"; //  (dummy=" << aTuple.dummy_tag_.getValueString() + ")";
    }
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
void QueryTuple::serialize(bool *dst, const QuerySchema &schema) {

    assert(!is_encrypted_);

    bool *cursor = dst;

    for(int fieldIdx = 0; fieldIdx < fieldCount_; ++fieldIdx) {
        fields_[fieldIdx].serialize(cursor);
        cursor += schema.getField(fieldIdx).size();
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
        fields_[i] = src.fields_[i];
    }

    return *this;

}

QueryTuple QueryTuple::reveal(const int &empParty) const {
    QueryTuple dstTuple(fieldCount_, false);

    for(int i = 0; i < fieldCount_; ++i) {
        QueryField dstField = fields_[i].reveal(empParty);
        dstTuple.putField(i, dstField);
    }

    EmpManager *empManager = EmpManager::getInstance();
    empManager->flush();


    std::shared_ptr<emp::Bit> dummyTag = dummy_tag_.getEmpBit();
    bool revealedBit = dummyTag->reveal((int) empParty);
    types::Value revealedValue(revealedBit);


    dstTuple.setDummyTag(revealedValue);
    return dstTuple;

}


