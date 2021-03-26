#include <util/type_utilities.h>
#include "query_tuple.h"
#include "query_schema.h"
#include "field/field_factory.h"
#include <util/utilities.h>

using namespace vaultdb;

std::ostream &vaultdb::operator<<(std::ostream &strm,  const PlainTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}

std::ostream &vaultdb::operator<<(std::ostream &strm,  const SecureTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}


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

  dummy_tag_ =  src.getDummyTag();
}

template<typename B>
const Field<B> *QueryTuple<B>::getField(const int &ordinal) const {
    return  &fields_[ordinal];
}


template<typename B>
void QueryTuple<B>::setField(const int &idx, const Field<B> &f) {
    fields_[idx] = f;
}


template<typename B>
void QueryTuple<B>::setDummyTag(const bool & v) {
    dummy_tag_ = B(v);
}



template<typename B>
const B QueryTuple<B>::getDummyTag() const {
    return dummy_tag_;
}



template<typename B>
string QueryTuple<B>::toString(const bool &showDummies) const {
   return specializedToString(*this, showDummies);
}



template<typename B>
void QueryTuple<B>::serialize(int8_t *dst, const QuerySchema &schema) {

    assert(!isEncrypted());

    int8_t *cursor = dst;


    for(size_t fieldIdx = 0; fieldIdx < getFieldCount(); ++fieldIdx) {
        fields_[fieldIdx].serialize(cursor);
        cursor += schema.getField(fieldIdx).size()/8;
    }

    Field<B>(dummy_tag_).serialize(cursor);

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
        Field<bool> revealed = fields_[i].reveal(empParty);
        dstTuple.setField(i, revealed);
    }


    bool dummyTag =  Field<B>(dummy_tag_).reveal(empParty).template getValue<bool>();
    dstTuple.setDummyTag(dummyTag);

    return dstTuple;

}


template<typename B>
void QueryTuple<B>::compareAndSwap(const B &cmp, QueryTuple<B> *lhs, QueryTuple<B> *rhs) {

    assert(lhs->getFieldCount() == rhs->getFieldCount());



    for(size_t i = 0; i < lhs->getFieldCount(); ++i) {

        Field<B>::compareAndSwap(cmp, lhs->fields_[i], rhs->fields_[i]);

    }

    Field<B> ldt =  Field<B>(lhs->dummy_tag_);
    Field<B> rdt =  Field<B>(rhs->dummy_tag_);

    Field<B>::compareAndSwap(cmp, ldt, rdt);

}

// only supported for plaintext
template <typename B>
bool  QueryTuple<B>::operator==(const QueryTuple<B> &other) const {
    return tuplesEqual(*this, other);
}

template <typename B>
PlainTuple QueryTuple<B>::deserialize(const QuerySchema &schema, int8_t *tupleBits) {
    size_t fieldCount = schema.getFieldCount();
    PlainTuple result(fieldCount);
    int8_t *cursor = tupleBits;

    for(size_t i = 0; i < fieldCount; ++i) {
        QueryFieldDesc fieldDesc = schema.getField(i);
        PlainField f = FieldFactory<bool>::deserialize(fieldDesc.getType(), fieldDesc.getStringLength(), cursor);
        result.setField(i, f);
        cursor += fieldDesc.size()/8;
    }

    bool dt = *((bool *) cursor);
    result.setDummyTag(dt);

    return result;

}

template <typename B>
SecureTuple QueryTuple<B>::deserialize(const QuerySchema &schema, emp::Bit *tupleBits) {
    size_t fieldCount = schema.getFieldCount();
    SecureTuple result(fieldCount);
    Bit *cursor = tupleBits;

    for(size_t i = 0; i < fieldCount; ++i) {
        QueryFieldDesc fieldDesc = schema.getField(i);
        SecureField f = FieldFactory<emp::Bit>::deserialize(fieldDesc.getType(), fieldDesc.getStringLength(), (int8_t *) cursor);
        result.setField(i, f);
        cursor += fieldDesc.size();
    }


    SecureField dt = SecureField(*cursor);
    result.setDummyTag(dt);

    return result;

}


template<typename B>
bool QueryTuple<B>::isEncrypted() const { return std::is_same_v<emp::Bit, B>; }



// srcTuple == nullptr if we are collecting secret shares of the other party's private data

template<typename B>
SecureTuple  QueryTuple<B>::secretShare(const PlainTuple *srcTuple, const QuerySchema &schema, const int & myParty, const int & dstParty) {
    int fieldCount = schema.getFieldCount();
    SecureTuple dstTuple(fieldCount);


    for(int i = 0; i < fieldCount; ++i) {


        const PlainField *srcField = (myParty == dstParty) ? srcTuple->getField(i) : nullptr;
        SecureField dstField = Field<B>::secretShare(srcField, schema.getField(i).getType(), schema.getField(i).getStringLength(), myParty,
                                             dstParty);
        dstTuple.setField(i, dstField);
    }

    PlainField *dummyTagField = nullptr;
    if(myParty == dstParty)
        *dummyTagField = PlainField(srcTuple->getDummyTag());

    SecureField encryptedDummyTag = Field<B>::secretShare(dummyTagField, FieldType::BOOL, 0, myParty, dstParty);
    dstTuple.setDummyTag(encryptedDummyTag);
    return dstTuple;
}

template<typename B>
std::string QueryTuple<B>::specializedToString(const PlainTuple &tuple, const bool &showDummies) const {

    std::stringstream sstream;

    bool printValue = showDummies || !tuple.getDummyTag();

    if(printValue) {
        sstream <<   "(" <<  *getField(0);

        for (size_t i = 1; i < getFieldCount(); ++i)
            sstream << ", " << *getField(i);

        sstream << ")";
    }

    if(showDummies) {
        sstream <<  " (dummy=" << dummy_tag_  << ")";
    }

    return sstream.str();

}

template<typename B>
std::string QueryTuple<B>::specializedToString(const SecureTuple &tuple, const bool &showDummies) const {
    std::stringstream sstream;


        sstream <<   "(" <<  getField(0)->toString();

        for (size_t i = 1; i < getFieldCount(); ++i)
            sstream << ", " << *getField(i);

        sstream << ")";

    if(showDummies) {
        sstream <<  " (dummy=" << "SECRET BIT" << ")";
    }

    return sstream.str();
}

template<typename B>
void QueryTuple<B>::setDummyTag(const Field<B> &d) {
    assert(d.getType() == FieldType::BOOL || d.getType() == FieldType::SECURE_BOOL);
    dummy_tag_ = d.template getValue<B>();
}

template<typename B>
bool QueryTuple<B>::tuplesEqual(const PlainTuple & lhs, const PlainTuple & rhs) {
    if(lhs.getFieldCount() != rhs.getFieldCount()) { return false; }


    //std::cout << "QueryTuple::operator==: Comparing dummy tags " << dummy_tag_.toString() << ", " << (*other.getDummyTag()).toString() << std::endl;

    if(lhs.getDummyTag() != rhs.getDummyTag()) {
        return false;
    }


    for(size_t i = 0; i < lhs.getFieldCount(); ++i) {
        if (lhs[i] != rhs[i]) {
            std::cout << "Comparing field " << i << ": |" << lhs[i] << "| len=" << lhs[i].getSize()  <<  std::endl
                      << " to              |" << rhs[i] << "| len=" << rhs[i].getSize()<<  std::endl;

            std::cout << "Failed to match!" << std::endl;
            return false;
        }
    }

    return true;
}

template<typename B>
bool QueryTuple<B>::tuplesEqual(const SecureTuple & lhs, const SecureTuple & rhs) {
    throw new std::invalid_argument("Cannot do equality check for tuples with encrypted data!"); // can't really check here
}

template class vaultdb::QueryTuple<bool>;
template class vaultdb::QueryTuple<emp::Bit>;


