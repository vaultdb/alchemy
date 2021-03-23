#include <util/type_utilities.h>
#include "query_tuple.h"
#include "query_schema.h"
#include "field/field_factory.h"

using namespace vaultdb;

template<typename B>
QueryTuple<B>::QueryTuple(const  size_t & aFieldCount) {
    dummy_tag_ = B(false);
    fields_.resize(aFieldCount);

    for(size_t i = 0; i < aFieldCount; ++i) {
        fields_[i]  = Field<B>();
    }

};


template<typename B>
QueryTuple<B>::QueryTuple(const QueryTuple<B> &src) {
    fields_.resize(src.getFieldCount());


    for(size_t i = 0; i < src.getFieldCount(); ++i) {
        fields_[i] = *src.getField(i);
    }

  dummy_tag_ =  *src.getDummyTag();
}

template<typename B>
const Field<B> *QueryTuple<B>::getField(const int &ordinal) const {
    return  &fields_[ordinal];
}


template<typename B>
void QueryTuple<B>::putField(const int &idx, const Field<B> &f) {
    fields_[idx] = f;
}

template<typename B>
void QueryTuple<B>::setDummyTag(const B & v) {
    dummy_tag_ = v;
}

template<typename B>
void QueryTuple<B>::setDummyTag(const bool & v) {
    dummy_tag_ = B(v);
}


template<typename B>
const B *QueryTuple<B>::getDummyTag() const {
    return &dummy_tag_;
}



template<typename B>
string QueryTuple<B>::toString(const bool &showDummies) const {
    std::stringstream sstream;

    if(showDummies
       ||    (!isEncrypted() && !(dummy_tag_.getBool())) // if it is real
       || isEncrypted()) { // or its status is unknown
        sstream <<   "(" <<  getField(0)->toString();

        for (size_t i = 1; i < getFieldCount(); ++i)
            sstream << ", " << getField(i)->toString();

        sstream << ")";
    }

    if(showDummies) {
        sstream <<  " (dummy=" << dummy_tag_.toString() + ")";
    }

    return sstream.str();

}



template<typename B>
void QueryTuple<B>::serialize(int8_t *dst, const QuerySchema &schema) {

    assert(!isEncrypted());

    int8_t *cursor = dst;


    for(size_t fieldIdx = 0; fieldIdx < getFieldCount(); ++fieldIdx) {
        fields_[fieldIdx].serialize(cursor);
        cursor += schema.getField(fieldIdx).size()/8;
    }

    dummy_tag_.serialize(cursor);

}

template<typename B>
size_t QueryTuple<B>::getFieldCount() const {
    return fields_.size();
}


template<typename B>
QueryTuple<B> & QueryTuple<B>::operator=(const QueryTuple& src) {
    if(&src == this)
        return *this;

    if(fields_.size() != src.getFieldCount()) {
        fields_.resize(src.getFieldCount());
    }

    for(size_t i = 0; i < src.getFieldCount(); ++i) {
        fields_[i] = src.fields_[i];
    }

    dummy_tag_ = src.dummy_tag_;


    return *this;

}

template<typename B>
PlainTuple QueryTuple<B>::reveal(const int &empParty) const {
    PlainTuple dstTuple(fields_.size());

    for(size_t i = 0; i < fields_.size(); ++i) {
        Field<BoolField> revealed = fields_[i].reveal(empParty);
        dstTuple.putField(i, revealed);
    }


    dstTuple.setDummyTag(BoolField(dummy_tag_.reveal(empParty)));

    return dstTuple;

}


template<typename B>
void QueryTuple<B>::compareAndSwap(const B &cmp, QueryTuple<B> *lhs, QueryTuple<B> *rhs) {

    assert(lhs->getFieldCount() == rhs->getFieldCount());



    for(size_t i = 0; i < lhs->getFieldCount(); ++i) {

        Field<B>::compareAndSwap(cmp, lhs->fields_[i], rhs->fields_[i]);

    }

    Field<B>::compareAndSwap(cmp, lhs->dummy_tag_, rhs->dummy_tag_);

}

// only supported for plaintext
template <typename B>
bool  QueryTuple<B>::operator==(const QueryTuple<B> &other) const {
    if(getFieldCount() != other.getFieldCount()) { return false; }

    if(isEncrypted() != other.isEncrypted()) { return false; }

    if(isEncrypted()) { throw new std::invalid_argument("Cannot do equality check for tuples with encrypted data!");} // can't really check here, assume encrypted data is ok

    //std::cout << "QueryTuple::operator==: Comparing dummy tags " << dummy_tag_.toString() << ", " << (*other.getDummyTag()).toString() << std::endl;
    // now know that <B> is BoolField
    if((dummy_tag_ != (*other.getDummyTag())).getBool()) {
        return false;
    }


    for(size_t i = 0; i < getFieldCount(); ++i) {
        //std::cout << "Comparing field: |" << fields_[i] << "| len=" << fields_[i].getSize()  <<  std::endl
        //         << " to              |" << other.fields_[i] << "| len=" << fields_[i].getSize()<<  std::endl;
        if ((fields_[i] != other.fields_[i]).getBool()) {
           // std::cout << "Failed to match!" << std::endl;
            return false;
        }
    }

    return true;
}

template <typename B>
QueryTuple<B> QueryTuple<B>::deserialize(const QuerySchema &schema, int8_t *tupleBits) {
    size_t fieldCount = schema.getFieldCount();
    QueryTuple result(fieldCount);
    int8_t *cursor = tupleBits;

    for(size_t i = 0; i < fieldCount; ++i) {
        result.fields_[i] = Field<B>::deserialize(schema.getField(i), cursor);
        cursor += schema.getField(i).size()/8;

    }

    B dt = B(cursor);
    result.setDummyTag(dt);

    return result;

}


template<typename B>
bool QueryTuple<B>::isEncrypted() const { return dummy_tag_.getType() == FieldType::SECURE_BOOL; }



// srcTuple == nullptr if we are collecting secret shares of the other party's private data

template<typename B>
SecureTuple  QueryTuple<B>::secretShare(const PlainTuple *srcTuple, const QuerySchema &schema, const int & myParty, const int & dstParty) {
    int fieldCount = schema.getFieldCount();
    SecureTuple dstTuple(fieldCount);


    for(int i = 0; i < fieldCount; ++i) {


        const Field<BoolField> *srcField = (myParty == dstParty) ? srcTuple->getField(i) : nullptr;
        Field<SecureBoolField> dstField = Field<B>::secretShare(srcField, schema.getField(i).getType(), schema.getField(i).getStringLength(), myParty,
                                             dstParty);
        dstTuple.putField(i, dstField);
    }

     const BoolField *dummyTag = (myParty == dstParty) ? srcTuple->getDummyTag(): nullptr;

    SecureBoolField encryptedDummyTag = static_cast<SecureBoolField>(Field<B>::secretShare(dummyTag, FieldType::BOOL,
                                                                                            0, myParty, dstParty));
    dstTuple.setDummyTag(encryptedDummyTag);
    return dstTuple;
}



std::ostream &vaultdb::operator<<(std::ostream &strm,  const PlainTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}

std::ostream &vaultdb::operator<<(std::ostream &strm,  const SecureTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}


template class vaultdb::QueryTuple<BoolField>;
template class vaultdb::QueryTuple<SecureBoolField>;
