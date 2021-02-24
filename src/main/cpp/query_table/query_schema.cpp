#include <iostream>
#include "query_schema.h"
#include <util/type_utilities.h>

using namespace vaultdb;

QuerySchema::QuerySchema(const size_t &num_fields)  {
    fields_.reserve(num_fields);

    // initialize all fields to blanks
    for(int i = 0; i < num_fields; ++i) {
        fields_.push_back(QueryFieldDesc());
    }



}


void QuerySchema::putField(const QueryFieldDesc &fd) {
    uint32_t ordinal = fd.getOrdinal();
    fields_[ordinal]  = fd; // copy field desc out into newly-allocated member variable

}

const QueryFieldDesc QuerySchema::getField(const int &i) const {
  return fields_.at(i);
}

int QuerySchema::getFieldCount() const {
    return fields_.size();
}


QuerySchema::QuerySchema(const QuerySchema &s) {
    fields_.reserve(s.getFieldCount());

  for (int i = 0; i < s.getFieldCount(); i++) {
      fields_.push_back( s.getField(i));
  }
}

size_t QuerySchema::size() const {
    size_t bitSize = 0L;
    for (int i = 0; i < getFieldCount(); i++) {
        bitSize += fields_[i].size();

    }
    int dummySize = TypeUtilities::isEncrypted(fields_[0].getType()) ?   TypeUtilities::getTypeSize(types::TypeId::ENCRYPTED_BOOLEAN) :  TypeUtilities::getTypeSize(types::TypeId::BOOLEAN);

    bitSize += dummySize; // for dummy tag
    return bitSize;
}

std::ostream &vaultdb::operator<<(std::ostream &os, const QuerySchema &schema) {
    int fieldCount = schema.getFieldCount();

    os << "(" << schema.getField(0);


    for(int i = 1; i < fieldCount; ++i) {
        os << ", " << schema.getField(i);
    }

    os << ")";
    return os;
}

QuerySchema &QuerySchema::operator=(const QuerySchema &other) {

    size_t fieldCount = other.getFieldCount();
    fields_.clear();
    fields_.reserve(fieldCount);

    for (int i = 0; i < other.getFieldCount(); i++) {
        QueryFieldDesc aFieldDesc = other.fields_[i];
        fields_.push_back(aFieldDesc);
    }

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
    return dstSchema;
}

QuerySchema QuerySchema::toPlain(const QuerySchema &secureSchema) {
    QuerySchema dstSchema(secureSchema.getFieldCount());
    for(QueryFieldDesc srcFieldDesc : secureSchema.fields_) {
        QueryFieldDesc dstFieldDesc(srcFieldDesc, TypeUtilities::toPlain(srcFieldDesc.getType()));
        dstSchema.putField(dstFieldDesc);
    }
    return dstSchema;

}

int QuerySchema::getOrdinal(const std::string & fieldName) const {
   int offset = 0;
   for(QueryFieldDesc fieldDesc : fields_) {
       if(fieldDesc.getName() == fieldName)
           return offset;
       ++offset;
   }
    // no match
   return -1;

}




