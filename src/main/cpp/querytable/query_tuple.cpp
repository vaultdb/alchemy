#include <util/emp_manager.h>
#include <util/type_utilities.h>
#include "query_tuple.h"
#include "query_schema.h"

using namespace vaultdb;

QueryTuple::QueryTuple(const  size_t & aFieldCount) {
    dummy_tag_.setValue(false); // not a dummy
    is_encrypted_ = false; // by default
    fieldCount_ = aFieldCount;
    fields_ =
        std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);

    for(uint32_t i = 0; i < fieldCount_; ++i) {
        fields_[i].setOrdinal(i); // initialize ordinal
    }

};

QueryTuple::QueryTuple(const size_t & fieldCount, const  bool & is_encrypted) : is_encrypted_(is_encrypted), fieldCount_(fieldCount) {
    if (is_encrypted_) {
        dummy_tag_.setValue(emp::Bit(false));
    } else {
        dummy_tag_.setValue(false);
    }

    fields_ =
            std::unique_ptr<QueryField[]>(new QueryField[fieldCount_]);

    for(uint32_t i = 0; i < fieldCount_; ++i) {
        fields_[i].setOrdinal(i); // initialize ordinal
    }

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
    return  fields_[ordinal];
}



void QueryTuple::putField(const QueryField &f) {
    uint32_t ordinal = f.getOrdinal();
    const types::Value src = f.getValue();
    fields_[ordinal].setValue(src);
}

void QueryTuple::setDummyTag(const types::Value &v) {
    dummy_tag_ = v;
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

    strm << aTuple.toString(false);

    return strm;
/*
    if((!aTuple.is_encrypted_ && !(aTuple.dummy_tag_.getBool())) // if it is real
       || aTuple.is_encrypted_) { // or its status is unknown
        strm << "(" << aTuple.getField(0);

        for (int i = 1; i < aTuple.fieldCount_; ++i)
            strm << ", " << aTuple.getField(i);

        strm << ")"; //  (dummy=" << aTuple.dummy_tag_.getValueString() + ")";
    }
    return strm;

    */



}



std::string QueryTuple::toString(const bool &showDummies) const {
    std::stringstream sstream;

    if(showDummies
       ||    (!is_encrypted_ && !(dummy_tag_.getBool())) // if it is real
       || is_encrypted_) { // or its status is unknown
         sstream <<   "(" <<  getField(0);

        for (int i = 1; i < fieldCount_; ++i)
            sstream << ", " << getField(i);

        sstream << ")";
    }

    if(showDummies) {
       sstream <<  " (dummy=" << dummy_tag_.getValueString() + ")";
    }

    return sstream.str();

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
        dstTuple.putField(dstField);
    }

    EmpManager *empManager = EmpManager::getInstance();
    empManager->flush();


    emp::Bit dummyTag = dummy_tag_.getEmpBit();
    bool revealedBit = dummyTag.reveal((int) empParty);
    types::Value revealedValue(revealedBit);


    dstTuple.setDummyTag(revealedValue);
    return dstTuple;

}

void QueryTuple::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs, const emp::Bit & cmp) {

    assert(lhs.getFieldCount() == rhs.getFieldCount());

    for(int i = 0; i < lhs.getFieldCount(); ++i) {
        types::Value lhsValue = lhs.getField(i).getValue();
        types::Value rhsValue = rhs.getField(i).getValue();


        types::Value::compareAndSwap(lhsValue, rhsValue, cmp);

        lhs.putField(QueryField(i, lhsValue));
        rhs.putField(QueryField(i, rhsValue));
    }

}

bool QueryTuple::operator==(const QueryTuple &other) {
    if(fieldCount_ != other.getFieldCount()) { return false; }

    if(is_encrypted_ != other.is_encrypted_) { return false; }

    if(!is_encrypted_) {
       // std::cout << "Comparing dummy tags: " << dummy_tag_ << " vs "  << other.dummy_tag_ << std::endl;
        types::Value cmp = (dummy_tag_ == other.dummy_tag_);
        if(!cmp.getBool()) {// if we are in the clear and their dummy tags are not equal
            return false;
        }
    }

    for(int i = 0; i < fieldCount_; ++i) {
        QueryField thisField = getField(i);
        QueryField otherField = other.getField(i);
        //std::cout << "Comparing field: " << thisField << " to " << otherField << std::endl;
        if(thisField != otherField) {  return false; }
    }

    return true;
}

vaultdb::QueryField *QueryTuple::getFieldPtr(const uint32_t &ordinal) const {
    return fields_.get() + ordinal;
}



