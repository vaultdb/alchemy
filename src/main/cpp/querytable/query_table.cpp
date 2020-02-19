//
// Created by madhav on 1/7/20.
//

#include "query_table.h"

QueryTuple *QueryTable::GetTuple(int idx) const { return &tuples_2[idx]; }

void QueryTable::SetSchema(const QuerySchema *s) {
  QueryTable::schema_ = std::make_unique<QuerySchema>(*s);
}

void QueryTable::SetSchema(std::unique_ptr<QuerySchema> s) {
  QueryTable::schema_ = std::move(s);
}

const QuerySchema *QueryTable::GetSchema() const { return schema_.get(); }

std::unique_ptr<QuerySchema> QueryTable::ReleaseSchema() { return std::move(schema_);}

unsigned int QueryTable::GetNumTuples() const { return num_tuples_; }
QueryTable::QueryTable(int num_tuples)
    : is_encrypted_(false), num_tuples_(num_tuples) {
  tuples_2 = static_cast<QueryTuple *>(malloc(
      sizeof(QueryTuple) * num_tuples_));
}

QueryTable::QueryTable(bool is_encrypted, int num_tuples)
    : is_encrypted_(is_encrypted), num_tuples_(num_tuples) {
  tuples_2 = static_cast<QueryTuple *>(malloc(
      sizeof(QueryTuple) * num_tuples_));
}

const bool QueryTable::GetIsEncrypted() const { return is_encrypted_; }
void QueryTable::AllocateQueryTuples() {
  if (num_tuples_ == 0) {
    throw;
  }
}
