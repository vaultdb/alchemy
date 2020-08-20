#include <iostream>
#include "query_schema.h"

QuerySchema::QuerySchema(const int &num_fields) : fieldCount_(num_fields) {
    fields_.reserve(num_fields);
    assert(fieldCount_ == 5);



}


void QuerySchema::putField(const int &idx, const QueryFieldDesc &fd) {
    fields_[idx]  = fd; // copy field desc out into newly-allocated member variable

}

const QueryFieldDesc QuerySchema::getField(const int &i) const {
  return fields_.at(i);
}

int QuerySchema::getFieldCount() const {
    return fieldCount_;
}


QuerySchema::QuerySchema(const QuerySchema &s) : fieldCount_(s.getFieldCount()){
    fields_.reserve(fieldCount_);
    assert(fieldCount_ == 5);

  for (int i = 0; i < s.getFieldCount(); i++) {
      fields_[i] = s.getField(i);
  }
}

size_t QuerySchema::size() const {
    size_t bitSize = 0L;
    for (int i = 0; i < getFieldCount(); i++) {
        bitSize += fields_[i].size();

    }
    ++bitSize; // account for dummy tag
        return bitSize;
}

std::ostream &operator<<(std::ostream &os, const QuerySchema &schema) {
    int fieldCount = schema.getFieldCount();

    os << "(" << schema.fields_[0];


    for(int i = 1; i < fieldCount; ++i) {
        os << ", " << schema.fields_[i];
    }

    os << ")";
    return os;
}

QuerySchema &QuerySchema::operator=(const QuerySchema &other) {

    fieldCount_ = other.getFieldCount();

    fields_.reserve(fieldCount_);

    for (int i = 0; i < other.getFieldCount(); i++) {
        QueryFieldDesc aFieldDesc = other.fields_[i];
        fields_.push_back(aFieldDesc);
    }

    return *this;
}

void QuerySchema::appendField(const QueryFieldDesc & aFieldDesc) {
    fields_.push_back(aFieldDesc);
}


