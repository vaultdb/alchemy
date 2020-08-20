#include <iostream>
#include "query_schema.h"

QuerySchema::QuerySchema(const int &num_fields) : fieldCount_(num_fields) {
    fields_.reserve(num_fields);


}

void QuerySchema::putField(const int &idx, const QueryFieldDesc &fd) {
    std::cout << "Putting field: " << fd << std::endl;
    QueryFieldDesc tmp = fd;

    std::cout << "Copied out tmp=" << tmp << std::endl;
    fields_[idx]  = fd; // copy field desc out into newly-allocated member variable

    std::cout << "In putfield, took in: " << fd << " stored: " << fields_[idx] << std::endl;
    assert(fd.getType() == fields_[idx].getType());
}

const QueryFieldDesc QuerySchema::getField(const int &i) const {
  return fields_.at(i);
}

int QuerySchema::getFieldCount() const {
    return fieldCount_;
}


QuerySchema::QuerySchema(const QuerySchema &s) : fieldCount_(s.getFieldCount()){
    fields_.reserve(fieldCount_);

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

    std::cout << "Assigning from " << other << std::endl;
    fields_.reserve(other.getFieldCount());

    for (int i = 0; i < other.getFieldCount(); i++) {
        QueryFieldDesc aFieldDesc = other.fields_[i];
        std::cout << "Src field: " << aFieldDesc << std::endl;
        fields_[i] = aFieldDesc;
    }

    return *this;
}


