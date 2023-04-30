#include <iostream>
#include "query_schema.h"

using namespace vaultdb;

QuerySchema::QuerySchema(const size_t &num_fields)  {
    fields_.reserve(num_fields);

    // initialize all fields to blanks
    for(size_t i = 0; i < num_fields; ++i) {
        fields_.push_back(QueryFieldDesc());
    }
}


void QuerySchema::putField(const QueryFieldDesc &fd) {
    uint32_t ordinal = fd.getOrdinal();
    fields_[ordinal]  = fd; // copy field desc out into newly-allocated member variable
}

QueryFieldDesc QuerySchema::getField(const int &i) const {
    return (i >= 0) ?
      fields_.at(i) :
     // dummy tag
     QueryFieldDesc(-1, fields_[0].getTableName(), "dummy_tag",
                    isSecure() ?
                        FieldType::SECURE_BOOL :
                        FieldType::BOOL, 0);
}

size_t QuerySchema::getFieldCount() const {
    return fields_.size();
}


QuerySchema::QuerySchema(std::shared_ptr<QuerySchema>  &s) {
    fields_.resize(s->getFieldCount());

  for (size_t i = 0; i < s->getFieldCount(); i++) {
      fields_[i] = s->getField(i);
  }
  initializeFieldOffsets();
}

// relies on initializeFieldOffsets()
size_t QuerySchema::size() const {
    return tuple_size_;
}

std::ostream &vaultdb::operator<<(std::ostream &os, const QuerySchema &schema) {
    size_t fieldCount = schema.getFieldCount();

    os << "(" << schema.getField(0);


    for(size_t i = 1; i < fieldCount; ++i) {
        os << ", " << schema.getField(i);
    }

    os << ")";
    return os;
}

QuerySchema &QuerySchema::operator=(const QuerySchema &other) {

    size_t fieldCount = other.getFieldCount();
    fields_.clear();
    fields_.reserve(fieldCount);
    bool valid_fields = true;

    for (size_t i = 0; i < other.getFieldCount(); i++) {
        QueryFieldDesc aFieldDesc = other.fields_[i];
        fields_.push_back(aFieldDesc);
        if(aFieldDesc.getType() == FieldType::INVALID)
            valid_fields = false;
    }
    if(valid_fields)
        initializeFieldOffsets();

    return *this;
}


bool QuerySchema::operator==(const QuerySchema &other) const {


    if(this->getFieldCount() != other.getFieldCount()) {
        return false;
    }

    for(uint32_t i = 0; i < this->getFieldCount(); ++i) {
        QueryFieldDesc thisFieldDesc = this->getField(i);
        QueryFieldDesc otherFieldDesc = other.getField(i);

        if(thisFieldDesc != otherFieldDesc) {
            return false;
        }
    }

    return true;
}

QuerySchema QuerySchema::toSecure(const QuerySchema &plainSchema) {
    QuerySchema dstSchema(plainSchema.getFieldCount());
    for(QueryFieldDesc srcFieldDesc : plainSchema.fields_) {
        QueryFieldDesc dstFieldDesc(srcFieldDesc, TypeUtilities::toSecure(srcFieldDesc.getType()));
        dstSchema.putField(dstFieldDesc);
    }
    dstSchema.initializeFieldOffsets();
    return dstSchema;
}

QuerySchema QuerySchema::toPlain(const QuerySchema &secureSchema) {
    QuerySchema dstSchema(secureSchema.getFieldCount());
    for(QueryFieldDesc srcFieldDesc : secureSchema.fields_) {
        QueryFieldDesc dstFieldDesc(srcFieldDesc, TypeUtilities::toPlain(srcFieldDesc.getType()));
        dstSchema.putField(dstFieldDesc);
    }
    dstSchema.initializeFieldOffsets();
    return dstSchema;

}


QueryFieldDesc QuerySchema::getField(const string &fieldName) const {
    for(QueryFieldDesc fieldDesc : fields_) {
        if(fieldDesc.getName() == fieldName)
            return fieldDesc;
    }

    throw; // not found
}

// in bits, idx == -1 means dummy_tag
void QuerySchema::initializeFieldOffsets()  {
    // populate ordinal --> offset mapping

    size_t runningOffset = 0L;
    // empty query table
    if(fields_.empty()) return;


    for(QueryFieldDesc fieldDesc : fields_) {
        offsets_[fieldDesc.getOrdinal()] =  runningOffset;
        runningOffset += fieldDesc.size();
    }
    // dummy tag at end
    offsets_[-1] = runningOffset;

    int dummySize = TypeUtilities::isEncrypted(fields_[0].getType()) ?   TypeUtilities::getTypeSize(FieldType::SECURE_BOOL) :  TypeUtilities::getTypeSize(FieldType::BOOL);

    tuple_size_ = runningOffset +  dummySize;

}

size_t QuerySchema::getFieldOffset(const int32_t idx) const {
    return offsets_.at(idx);
}




