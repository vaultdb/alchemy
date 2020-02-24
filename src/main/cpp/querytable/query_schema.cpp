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
