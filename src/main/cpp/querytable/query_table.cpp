#include "query_table.h"
#include "../data/proto_converter.h"

QueryTuple *QueryTable::GetTuple(int idx) const { return &tuples_2[idx]; }

void QueryTable::SetSchema(const QuerySchema *s) {
  QueryTable::schema_ = std::make_unique<QuerySchema>(*s);
}

void QueryTable::SetSchema(std::unique_ptr<QuerySchema> s) {
  QueryTable::schema_ = std::move(s);
}

const QuerySchema *QueryTable::GetSchema() const { return schema_.get(); }

std::unique_ptr<QuerySchema> QueryTable::ReleaseSchema() {
  return std::move(schema_);
}

unsigned int QueryTable::GetNumTuples() const { return num_tuples_; }
QueryTable::QueryTable(int num_tuples)
    : is_encrypted_(false), num_tuples_(num_tuples) {
  tuples_2 =
      static_cast<QueryTuple *>(malloc(sizeof(QueryTuple) * num_tuples_));
}

QueryTable::QueryTable(bool is_encrypted, int num_tuples)
    : is_encrypted_(is_encrypted), num_tuples_(num_tuples) {
  tuples_2 =
      static_cast<QueryTuple *>(malloc(sizeof(QueryTuple) * num_tuples_));
}

const bool QueryTable::GetIsEncrypted() const { return is_encrypted_; }
void QueryTable::AllocateQueryTuples() {
  if (num_tuples_ == 0) {
    throw;
  }
}
std::unique_ptr<QueryTable> QueryTable::GetQueryFromProtoStream(const void *buf,
                                                                int len) {
  dbquery::Table t;
  t.ParseFromArray(buf, len);
  return ProtoToQuerytable(t);
}

std::string QueryTable::GetQueryTableXorString(QueryTable *input_table) {
  return QueryTableToXorProto(input_table).SerializeAsString();
}


std::unique_ptr<QueryTable> QueryTable::reveal(EmpParty party) const  {
    // TODO: set it so that when it is XOR-encoded, it is encrypted
    // this has downstream effects that need to be figured out first

    std::unique_ptr<QueryTable> result(new QueryTable(false, this->num_tuples_));

    for(int i = 0; i < num_tuples_; ++i) {
        QueryTuple *decrypted  = tuples_[i]->reveal(party);
        result->tuples_.emplace_back(std::unique_ptr<QueryTuple>(decrypted));
   }

    return result;
}