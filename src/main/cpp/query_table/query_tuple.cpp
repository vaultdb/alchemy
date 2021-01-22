#include <util/emp_manager.h>
#include <util/type_utilities.h>
#include "query_tuple.h"
#include "query_schema.h"

using namespace vaultdb;

QueryTuple::QueryTuple(const  size_t & aFieldCount) {
    dummy_tag_.setValue(false); // not a dummy
    fields_.resize(aFieldCount);

    for(uint32_t i = 0; i < aFieldCount; ++i) {
        fields_[i].setOrdinal(i); // initialize ordinal
    }

};

QueryTuple::QueryTuple(const size_t & fieldCount, const  bool & is_encrypted)  {
    if (is_encrypted) {
        dummy_tag_.setValue(emp::Bit(false));
    } else {
        dummy_tag_.setValue(false);
    }

    fields_.resize(fieldCount);

    for(uint32_t i = 0; i < fieldCount; ++i) {
        fields_[i].setOrdinal(i); // initialize ordinal
    }

}


QueryTuple::QueryTuple(const QueryTuple &src) {
    fields_.resize(src.getFieldCount());


    for(int i = 0; i < src.getFieldCount(); ++i) {
        fields_[i] = src.fields_[i];
    }

    dummy_tag_ = src.getDummyTag();
}

const QueryField QueryTuple::getField(int ordinal) const {
    return  fields_[ordinal];
}



void QueryTuple::putField(const QueryField &f) {
    uint32_t ordinal = f.getOrdinal();
    const types::Value src = f.getValue();
    fields_[ordinal] = QueryField(ordinal, src);
}

void QueryTuple::setDummyTag(const types::Value &v) {
    dummy_tag_ = v;
}


const vaultdb::types::Value QueryTuple::getDummyTag() const {
  return this->dummy_tag_;
}






std::ostream &vaultdb::operator<<(std::ostream &strm,  const QueryTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}



std::string QueryTuple::toString(const bool &showDummies) const {
    std::stringstream sstream;

    if(showDummies
       ||    (!isEncrypted() && !(dummy_tag_.getBool())) // if it is real
           || isEncrypted()) { // or its status is unknown
         sstream <<   "(" <<  getField(0);

        for (int i = 1; i < getFieldCount(); ++i)
            sstream << ", " << getField(i);

        sstream << ")";
    }

    if(showDummies) {
       sstream <<  " (dummy=" << dummy_tag_.toString() + ")";
    }

    return sstream.str();

}



void QueryTuple::setFieldCount(size_t fieldCount) {
    fields_.resize(fieldCount);

}

void QueryTuple::serialize(int8_t *dst, const QuerySchema &schema) {

    assert(!isEncrypted());

    int8_t *cursor = dst;


    for(int fieldIdx = 0; fieldIdx < getFieldCount(); ++fieldIdx) {
        fields_[fieldIdx].serialize(cursor);
        cursor += schema.getField(fieldIdx).size()/8;
    }

    *cursor = dummy_tag_.getBool();

}

size_t QueryTuple::getFieldCount() const {
    return fields_.size();
}


QueryTuple& QueryTuple::operator=(const QueryTuple& src) {
    if(&src == this)
        return *this;




    this->dummy_tag_.setValue(src.dummy_tag_);


    fields_.resize(src.getFieldCount());


    for(int i = 0; i < src.getFieldCount(); ++i) {
        fields_[i] = src.fields_[i];
    }

    return *this;

}

QueryTuple QueryTuple::reveal(const int &empParty) const {
    QueryTuple dstTuple(fields_.size(), false);

    for(int i = 0; i < fields_.size(); ++i) {
        QueryField dstField = fields_[i].reveal(empParty);
        dstTuple.putField(dstField);
    }



    emp::Bit dummyTag = dummy_tag_.getEmpBit();
    bool revealedBit = dummyTag.reveal(empParty);
    types::Value revealedValue(revealedBit);


    dstTuple.setDummyTag(revealedValue);
    return dstTuple;

}



void QueryTuple::compareAndSwap(QueryTuple *lhs, QueryTuple *rhs, const emp::Bit & cmp) {

    assert(lhs->getFieldCount() == rhs->getFieldCount());

    for(int i = 0; i < lhs->getFieldCount(); ++i) {
        types::Value lhsValue = lhs->getField(i).getValue();
        types::Value rhsValue = rhs->getField(i).getValue();


        types::Value::compareAndSwap(lhsValue, rhsValue, cmp);

        lhs->putField(QueryField(i, lhsValue));
        rhs->putField(QueryField(i, rhsValue));
    }

    types::Value lhsDummyTag = lhs->getDummyTag();
    types::Value rhsDummyTag = rhs->getDummyTag();

    types::Value::compareAndSwap(lhsDummyTag, rhsDummyTag, cmp);
    lhs->setDummyTag(lhsDummyTag);
    rhs->setDummyTag(rhsDummyTag);

}

bool QueryTuple::operator==(const QueryTuple &other) {
    if(getFieldCount() != other.getFieldCount()) { return false; }

    if(isEncrypted() != other.isEncrypted()) { return false; }

    if(!isEncrypted()) {
        //std::cout << "Comparing dummy tags: " << dummy_tag_ << " vs ";
        // std::cout  << other.dummy_tag_ << std::endl;
        bool lhs = dummy_tag_.getBool();
        bool rhs = dummy_tag_.getBool();
        if(lhs != rhs) {// if we are in the clear and their dummy tags are not equal
            return false;
        }
    }

    for(int i = 0; i < getFieldCount(); ++i) {
        QueryField *thisField = getFieldPtr(i);
        QueryField *otherField = other.getFieldPtr(i);
        //std::cout << "Comparing field: " << *thisField << " to " << *otherField << std::endl;
        if(*thisField != *otherField) {  return false; }
    }

    return true;
}

vaultdb::QueryField *QueryTuple::getFieldPtr(const uint32_t &ordinal) const {
    return ((QueryField *) fields_.data()) + ordinal;
}

QueryTuple QueryTuple::deserialize(const QuerySchema &schema, int8_t *tupleBits) {
    int fieldCount = schema.getFieldCount();
    QueryTuple result(fieldCount);
    int8_t *cursor = tupleBits;

    for(int i = 0; i < fieldCount; ++i) {
        QueryField aField = QueryField::deserialize(schema.getField(i), cursor);
        result.putField(aField);
        cursor += schema.getField(i).size()/8;
    }

    if(TypeUtilities::isEncrypted(schema.getField(0).getType())) {
        // deserialize an emp::Bit
        QueryFieldDesc dummyField(-1, "dummy", "dummy", types::TypeId::ENCRYPTED_BOOLEAN);
        result.dummy_tag_ = types::Value::deserialize(dummyField, cursor);
    }
    else {
        // deserialize a bool
        QueryFieldDesc dummyField(-1, "dummy", "dummy", types::TypeId::BOOLEAN);
        result.dummy_tag_ = types::Value::deserialize(dummyField, cursor);
    }
    std::cout << "Deserialized tuple: " << result << std::endl;
    return result;

}





