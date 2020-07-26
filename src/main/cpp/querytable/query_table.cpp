#include "query_table.h"
#include "../data/proto_converter.h"

QueryTuple *QueryTable::GetTuple(int idx) const {
    return &tuples_[idx];
}

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

unsigned int QueryTable::GetNumTuples() const {
    return tupleCount_;
}

QueryTable::QueryTable(int num_tuples)
    : is_encrypted_(false), tupleCount_(num_tuples) {
    tuples_ =
            std::unique_ptr<QueryTuple[]>(new QueryTuple[tupleCount_]);

}

QueryTable::QueryTable(int num_tuples, bool is_encrypted)
    : is_encrypted_(is_encrypted), tupleCount_(num_tuples) {
    tuples_ =
            std::unique_ptr<QueryTuple[]>(new QueryTuple[tupleCount_]);

}

const bool QueryTable::GetIsEncrypted() const { return is_encrypted_; }

std::unique_ptr<QueryTable> QueryTable::GetQueryFromProtoStream(const void *buf,
                                                                int len) {
  dbquery::Table t;
  t.ParseFromArray(buf, len);
  return ProtoToQueryTable(t);
}

std::string QueryTable::GetQueryTableXorString(QueryTable *input_table) {
  return QueryTableToXorProto(input_table).SerializeAsString();
}


/*std::unique_ptr<QueryTable> QueryTable::reveal(EmpParty party) const  {
    // TODO: set it so that when it is XOR-encoded, it is encrypted
    // this has downstream effects that need to be figured out first

    std::unique_ptr<QueryTable> result(new QueryTable(this->tupleCount_, false));

    for(int i = 0; i < tupleCount_; ++i) {
        QueryTuple decrypted = tuples_[i].reveal(party);
        result->tuples_[i]
   }

    return result;
}*/

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only works in PUBLIC or XOR mode
bool *QueryTable::serialize() const {
    size_t dstSize = tupleCount_ * schema_.get()->size();
    bool *dst = new bool[dstSize];
    return nullptr;
}

std::ostream &operator<<(std::ostream &os, const QueryTable &table) {

    QuerySchema *schemaPtr = table.schema_.get();

    os <<  *(schemaPtr) << " isEncrypted? " << table.is_encrypted_ << std::endl;

    for(int i = 0; i < table.GetNumTuples(); ++i)
        os << table.tuples_[i] << std::endl;



    return os;
}

std::string QueryTable::toString() const {

    std::ostringstream stream;
    stream << *this;
    return stream.str();
}
