#include <util/type_utilities.h>
#include <util/field_utilities.h>
#include "query_tuple.h"
#include "query_schema.h"
#include "field/field_factory.h"

using namespace vaultdb;

QueryTuple::QueryTuple(const  size_t & aFieldCount) {
    dummy_tag_ = std::unique_ptr<Field>(new BoolField(false));
    fields_.resize(aFieldCount);

    for(size_t i = 0; i < aFieldCount; ++i) {
        fields_[i]  = std::unique_ptr<Field>(new Field());
    }

};

QueryTuple::QueryTuple(const size_t & fieldCount, const  bool & is_encrypted)  {
    if (is_encrypted) {
        dummy_tag_ = std::unique_ptr<Field>(new SecureBoolField(false)); // not a dummy
    } else {
        dummy_tag_ = std::unique_ptr<Field>(new BoolField(false));
    }

    fields_.resize(fieldCount);

    for(size_t i = 0; i < fieldCount; ++i) {
        fields_[i]  = std::unique_ptr<Field>(new Field());
    }

}


QueryTuple::QueryTuple(const QueryTuple &src) {
    fields_.resize(src.getFieldCount());


    for(size_t i = 0; i < src.getFieldCount(); ++i) {
        fields_[i] = std::unique_ptr<Field>(FieldFactory::deepCopy(src.getField(i)));
    }

  dummy_tag_ = std::unique_ptr<Field>(FieldFactory::deepCopy(src.getDummyTag()));
}

const Field * QueryTuple::getField(const int &ordinal) const {
    return  fields_[ordinal].get();
}



void QueryTuple::putField(const int &idx, const Field &f) {

    *fields_[idx] = f;
}

void QueryTuple::setDummyTag(const Field & v) {
    *dummy_tag_ = v;
}


const Field * QueryTuple::getDummyTag() const {
    return dummy_tag_.get();
}



string QueryTuple::toString(const bool &showDummies) const {
    std::stringstream sstream;

    if(showDummies
       || (!isEncrypted() && !(dummy_tag_->getValue<bool>())) // if it is real
           || isEncrypted()) { // or its status is unknown
         sstream <<   "(" <<  *fields_[0];

        for (size_t i = 1; i < getFieldCount(); ++i)
            sstream << ", " << *fields_[i];

        sstream << ")";
    }

    if(showDummies) {
       sstream <<  " (dummy=" << *dummy_tag_ <<  + ")";
    }

    return sstream.str();

}



void QueryTuple::serialize(int8_t *dst, const QuerySchema &schema) {

    assert(!isEncrypted());

    int8_t *cursor = dst;


    for(size_t fieldIdx = 0; fieldIdx < getFieldCount(); ++fieldIdx) {
        fields_[fieldIdx]->serialize(cursor);
        cursor += schema.getField(fieldIdx).size()/8;
    }

    bool dummyTag = dummy_tag_->getValue<bool>();
    *cursor = dummyTag;

}

size_t QueryTuple::getFieldCount() const {
    return fields_.size();
}


QueryTuple& QueryTuple::operator=(const QueryTuple& src) {
    if(&src == this)
        return *this;

    fields_.resize(src.getFieldCount());

    for(size_t i = 0; i < src.getFieldCount(); ++i) {
        const Field *srcField = src.getField(i);
        Field *dstField = FieldFactory::deepCopy(srcField);
        fields_[i] = std::unique_ptr<Field>(dstField);
    }

    dummy_tag_ = std::unique_ptr<Field>(FieldFactory::deepCopy(src.getDummyTag()));


    return *this;

}

QueryTuple QueryTuple::reveal(const int &empParty) const {
    QueryTuple dstTuple(fields_.size(), false);

    for(size_t i = 0; i < fields_.size(); ++i) {
        Field *revealed = fields_[i]->reveal(empParty);
        dstTuple.putField(i, *revealed);
        delete revealed;

    }



    emp::Bit dummyTag = dummy_tag_->getValue<emp::Bit>();
    dstTuple.dummy_tag_  = std::unique_ptr<Field>(new BoolField(dummyTag.reveal(empParty)));

    return dstTuple;

}



void QueryTuple::compareAndSwap(QueryTuple *lhs, QueryTuple *rhs, const emp::Bit & cmp) {

    assert(lhs->getFieldCount() == rhs->getFieldCount());

    for(size_t i = 0; i < lhs->getFieldCount(); ++i) {

        Field::compareAndSwap(cmp, *(lhs->fields_[i]), *(rhs->fields_[i]));

    }

    Field::compareAndSwap(cmp, *lhs->dummy_tag_, *rhs->dummy_tag_);

}

bool  QueryTuple::operator==(const QueryTuple &other) const {
    if(getFieldCount() != other.getFieldCount()) { return false; }

    if(isEncrypted() != other.isEncrypted()) { return false; }

    if(*dummy_tag_ != *(other.dummy_tag_))  return false;


    for(size_t i = 0; i < getFieldCount(); ++i) {
//        std::cout << "Comparing field: |" << *(fields_[i]) << "| len=" << fields_[i]->getSize()  <<  std::endl
//                 << " to              |" << *(other.fields_[i]) << "| len=" << fields_[i]->getSize()<<  std::endl;
        bool eq = FieldUtilities::equals(fields_[i].get(), other.fields_[i].get());
        if(!eq) {
            return false;
        }
    }

    return true;
}


QueryTuple QueryTuple::deserialize(const QuerySchema &schema, int8_t *tupleBits) {
    size_t fieldCount = schema.getFieldCount();
    QueryTuple result(fieldCount);
    int8_t *cursor = tupleBits;

    for(size_t i = 0; i < fieldCount; ++i) {
        result.fields_.emplace_back(std::make_unique<Field>(Field::deserialize(schema.getField(i), cursor)));
        cursor += result.fields_[i]->getSize();
    }

    if(TypeUtilities::isEncrypted(schema.getField(0).getType())) {
        // deserialize an emp::Bit
        emp::Bit *bitPtr = (emp::Bit *) cursor;
        result.dummy_tag_ = std::unique_ptr<Field> (new SecureBoolField(*bitPtr));
    }
    else {
        // deserialize a bool
        result.dummy_tag_ = std::unique_ptr<Field>(new BoolField(cursor));
    }

    return result;

}


bool QueryTuple::isEncrypted() const { return dummy_tag_->getType() == FieldType::SECURE_BOOL; }



// srcTuple == nullptr if we are collecting secret shares of the other party's private data
QueryTuple QueryTuple::secretShare(const QueryTuple *srcTuple, const QuerySchema &schema, const int & myParty, const int & dstParty) {
    int fieldCount = schema.getFieldCount();
    QueryTuple dstTuple(fieldCount, true);


    for(int i = 0; i < fieldCount; ++i) {

        const Field *srcField = (myParty == dstParty) ? srcTuple->getField(i) : nullptr;
        Field *dstField = Field::secretShare(srcField, schema.getField(i).getType(), schema.getField(i).getStringLength(), myParty,
                                             dstParty);
        dstTuple.putField(i, *dstField);
        delete dstField;
    }

    BoolField *dummyTag = nullptr;
    if(srcTuple != nullptr)
        dummyTag = (BoolField *) srcTuple->getDummyTag();


    Field *encryptedDummyTag = Field::secretShare(dummyTag, FieldType::BOOL, 0, myParty, dstParty);
    dstTuple.setDummyTag(*encryptedDummyTag);

    delete encryptedDummyTag;

    return dstTuple;
}

void QueryTuple::setDummyTag(const bool &b) {
    dummy_tag_ = std::unique_ptr<Field>(new BoolField(b));
}

void QueryTuple::setDummyTag(const Bit &b) {
    dummy_tag_ = std::unique_ptr<Field>(new SecureBoolField(b));
}

std::ostream &vaultdb::operator<<(std::ostream &strm,  const QueryTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}
