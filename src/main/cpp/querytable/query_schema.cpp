//
// Created by madhav on 1/13/20.
//

#include "query_schema.h"

QuerySchema::QuerySchema(int num_fields) {}
void QuerySchema::PutField(int index, QueryFieldDesc &fd) {
  QuerySchema::fields_.emplace(index,
                               std::make_unique<QueryFieldDesc>(fd, index));
}
QueryFieldDesc *QuerySchema::GetField(int i) const {
  return fields_.at(i).get();
}
int QuerySchema::GetNumFields() const { return fields_.size(); }
QuerySchema::QuerySchema(const QuerySchema &s) {
  for (int i = 0; i < s.GetNumFields(); i++) {
    PutField(i, *s.GetField(i));
  }
}

size_t QuerySchema::size() const {
    size_t bitSize = 0L;
    for (int i = 0; i < GetNumFields(); i++) {
        bitSize += fields_.at(i)->size();

    }

        return bitSize;
}
