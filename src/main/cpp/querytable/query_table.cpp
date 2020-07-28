#include <util/emp_manager.h>
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

unsigned int QueryTable::getTupleCount() const {
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


std::unique_ptr<QueryTable> QueryTable::reveal(EmpParty party) const  {
    // TODO: set it so that when it is XOR-encoded, it is encrypted
    // this has downstream effects that need to be figured out first

    EmpManager *empManager  = EmpManager::getInstance();
    return empManager->revealTable(this, (int) party);
}

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only works in PUBLIC or XOR mode
bool *QueryTable::serialize() const {
    // dst size is in bits
    size_t tupleWidth =  schema_.get()->size();
    size_t dstSize = tupleCount_ * tupleWidth;
    bool *dst = new bool[dstSize];
    bool *cursor = dst;

    for(int i = 0; i < tupleCount_; ++i) {
        tuples_[i].serialize(cursor, schema_.get());
        cursor += tupleWidth;
    }

    return dst;
}

std::ostream &operator<<(std::ostream &os, const QueryTable &table) {

    QuerySchema *schemaPtr = table.schema_.get();

    os <<  *(schemaPtr) << " isEncrypted? " << table.is_encrypted_ << std::endl;

    for(int i = 0; i < table.getTupleCount(); ++i)
        os << table.tuples_[i] << std::endl;



    return os;
}

std::string QueryTable::toString() const {

    std::ostringstream stream;
    stream << *this;
    return stream.str();
}
