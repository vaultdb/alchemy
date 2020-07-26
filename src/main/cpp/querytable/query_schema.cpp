#include <iostream>
#include "query_schema.h"

QuerySchema::QuerySchema(int num_fields) : fieldCount_(num_fields) {
    fields_ =
            std::unique_ptr<QueryFieldDesc[]>(new QueryFieldDesc[fieldCount_]);


}

void QuerySchema::PutField(int idx, QueryFieldDesc &fd) {
    fields_[idx].initialize(fd); // copy field desc out into newly-allocated member variable
}

QueryFieldDesc *QuerySchema::GetField(int i) const {
  return &fields_[i];
}
int QuerySchema::GetNumFields() const {
    return fieldCount_;
}


QuerySchema::QuerySchema(const QuerySchema &s) : fieldCount_(s.GetNumFields()){
    fields_ =
            std::unique_ptr<QueryFieldDesc[]>(new QueryFieldDesc[fieldCount_]);
  for (int i = 0; i < s.GetNumFields(); i++) {
      fields_[i].initialize(*s.GetField(i));
  }
}

size_t QuerySchema::size() const {
    size_t bitSize = 0L;
    for (int i = 0; i < GetNumFields(); i++) {
        bitSize += fields_[i].size();

    }

        return bitSize;
}

std::ostream &operator<<(std::ostream &os, const QuerySchema &schema) {
    int fieldCount = schema.GetNumFields();

    os << "(" << schema.fields_[0];


    for(int i = 1; i < fieldCount; ++i) {
        os << ", " << schema.fields_[i];
    }

    os << ")";
    return os;
}
