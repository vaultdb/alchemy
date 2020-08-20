#include <iostream>
#include "query_schema.h"

QuerySchema::QuerySchema(int num_fields) : fieldCount_(num_fields) {
    fields_ =
            std::unique_ptr<QueryFieldDesc[]>(new QueryFieldDesc[fieldCount_]);


}

void QuerySchema::putField(int index, QueryFieldDesc &fd) {
    fields_[index].initialize(fd); // copy field desc out into newly-allocated member variable
}

const QueryFieldDesc QuerySchema::getField(int i) const {
  return fields_[i];
}
int QuerySchema::getFieldCount() const {
    return fieldCount_;
}


QuerySchema::QuerySchema(const QuerySchema &s) : fieldCount_(s.getFieldCount()){
    fields_ =
            std::unique_ptr<QueryFieldDesc[]>(new QueryFieldDesc[fieldCount_]);

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

    fields_ =
            std::unique_ptr<QueryFieldDesc[]>(new QueryFieldDesc[other.getFieldCount()]);

    for (int i = 0; i < other.getFieldCount(); i++) {
        fields_[i] = other.getField(i);
    }

}


