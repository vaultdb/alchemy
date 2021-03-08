#include <util/type_utilities.h>
#include "query_tuple.h"
#include "query_schema.h"

using namespace vaultdb;

QueryTuple::QueryTuple(const  size_t & aFieldCount) {
    dummy_tag_ = Field(FieldType::BOOL);
    dummy_tag_.setValue<bool>(false); // not a dummy
    fields_.resize(aFieldCount);

};

QueryTuple::QueryTuple(const size_t & fieldCount, const  bool & is_encrypted)  {
    if (is_encrypted) {
        dummy_tag_ = Field(FieldType::SECURE_BOOL);
        dummy_tag_.setValue<emp::Bit>(emp::Bit(false)); // not a dummy
    } else {
        dummy_tag_ = Field(FieldType::BOOL);
        dummy_tag_.setValue<bool>(false); // not a dummy

    }

    fields_.resize(fieldCount);


}


QueryTuple::QueryTuple(const QueryTuple &src) {
    fields_.resize(src.getFieldCount());


    for(size_t i = 0; i < src.getFieldCount(); ++i) {
        fields_[i] = src.fields_[i];
    }

    dummy_tag_ = src.getDummyTag();
}

const Field QueryTuple::getField(const int &ordinal) const {
    return  fields_[ordinal];
}



void QueryTuple::putField(const int &idx, const Field &f) {

    fields_[idx] = f;
}

void QueryTuple::setDummyTag(const Field & v) {
    dummy_tag_ = v;
}


const Field QueryTuple::getDummyTag() const {
    return dummy_tag_;
}



string QueryTuple::toString(const bool &showDummies) const {
    std::stringstream sstream;

    if(showDummies
       ||    (!isEncrypted() && !(dummy_tag_.getValue<bool>())) // if it is real
           || isEncrypted()) { // or its status is unknown
         sstream <<   "(" <<  fields_[0];

        for (size_t i = 1; i < getFieldCount(); ++i)
            sstream << ", " << fields_[i];

        sstream << ")";
    }

    if(showDummies) {
       sstream <<  " (dummy=" << dummy_tag_ <<  + ")";
    }

    return sstream.str();

}



void QueryTuple::setFieldCount(size_t fieldCount) {
    fields_.resize(fieldCount);

}

void QueryTuple::serialize(int8_t *dst, const QuerySchema &schema) {

    assert(!isEncrypted());

    int8_t *cursor = dst;


    for(size_t fieldIdx = 0; fieldIdx < getFieldCount(); ++fieldIdx) {
        fields_[fieldIdx].serialize(cursor);
        cursor += schema.getField(fieldIdx).size()/8;
    }

    bool dummyTag = dummy_tag_.getValue<bool>();
    *cursor = dummyTag;

}

size_t QueryTuple::getFieldCount() const {
    return fields_.size();
}


QueryTuple& QueryTuple::operator=(const QueryTuple& src) {
    if(&src == this)
        return *this;




    this->dummy_tag_ = src.dummy_tag_;


    fields_.resize(src.getFieldCount());


    for(size_t i = 0; i < src.getFieldCount(); ++i) {
        fields_[i] = src.fields_[i];
    }

    return *this;

}

QueryTuple QueryTuple::reveal(const int &empParty) const {
    QueryTuple dstTuple(fields_.size(), false);

    for(size_t i = 0; i < fields_.size(); ++i) {
        dstTuple.fields_.emplace_back(fields_[i].reveal(empParty));
    }



    emp::Bit dummyTag = dummy_tag_.getValue<emp::Bit>();
    dstTuple.dummy_tag_  = Field::createBool(dummyTag.reveal(empParty));

    return dstTuple;

}



void QueryTuple::compareAndSwap(QueryTuple *lhs, QueryTuple *rhs, const emp::Bit & cmp) {

    assert(lhs->getFieldCount() == rhs->getFieldCount());

    for(size_t i = 0; i < lhs->getFieldCount(); ++i) {

        Field::compareAndSwap(cmp, lhs->fields_[i], rhs->fields_[i]);

    }

    Field::compareAndSwap(cmp, lhs->dummy_tag_, rhs->dummy_tag_);

}

bool  QueryTuple::operator==(const QueryTuple &other) const {
    if(getFieldCount() != other.getFieldCount()) { return false; }

    if(isEncrypted() != other.isEncrypted()) { return false; }

    if(dummy_tag_ != other.dummy_tag_)  return false;


    for(size_t i = 0; i < getFieldCount(); ++i) {
        //std::cout << "Comparing field: |" << fields_[i] << "|" << std::endl
        //         << " to              |" << other.fields_[i] << "|" <<  std::endl;
        if(fields_[i] != other.fields_[i]) {  return false; }
    }

    return true;
}


QueryTuple QueryTuple::deserialize(const QuerySchema &schema, int8_t *tupleBits) {
    size_t fieldCount = schema.getFieldCount();
    QueryTuple result(fieldCount);
    int8_t *cursor = tupleBits;

    for(size_t i = 0; i < fieldCount; ++i) {
        result.fields_.emplace_back(Field::deserialize(schema.getField(i), cursor));
        cursor += result.fields_[i].getSize();
    }

    if(TypeUtilities::isEncrypted(schema.getField(0).getType())) {
        // deserialize an emp::Bit
        emp::Bit *bitPtr = (emp::Bit *) cursor;
        result.dummy_tag_ = Field::createSecureBool(*bitPtr);
    }
    else {
        // deserialize a bool
        result.dummy_tag_ = Field::createBool(*((bool *)cursor));
    }

    return result;

}

// only handles encrypted case
// always has dummy
/* should be unnecessary
QueryTuple QueryTuple::deserialize(const QuerySchema &schema, Bit *tupleBits) {
    size_t fieldCount = schema.getFieldCount();
    QueryTuple result(fieldCount, true);
    Bit *cursor = tupleBits;


    for(size_t i = 0; i < fieldCount; ++i) {
        QueryField aField = QueryField::deserialize(schema.getField(i), cursor);
        result.putField(aField);
        cursor += schema.getField(i).size();
    }



    result.setDummyTag(*(cursor + 7)); // padded to 8 bits to be byte-aligned, LSB is last one

    return result;
}
 */

bool QueryTuple::isEncrypted() const { return dummy_tag_.getType() == FieldType::SECURE_BOOL; }

/*Field *QueryTuple::getFieldPtr(const int &ordinal) {

    return (fields_.data()) + ordinal;
}*/


std::ostream &vaultdb::operator<<(std::ostream &strm,  const QueryTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}
