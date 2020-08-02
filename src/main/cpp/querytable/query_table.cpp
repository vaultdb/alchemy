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



QueryTable::QueryTable(int num_tuples, int colCount, bool is_encrypted)
    : is_encrypted_(is_encrypted), tupleCount_(num_tuples) {
    tuples_ =
            std::unique_ptr<QueryTuple[]>(new QueryTuple[tupleCount_]);

    for(int i = 0; i < tupleCount_; ++i) {
        tuples_[i].setFieldCount(colCount); // initialize tuples
    }


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
    int colCount = GetSchema()->getFieldCount();
    int tupleCount = getTupleCount();
    bool isEncrypted = (party == EmpParty::XOR) ? true : false;


    std::unique_ptr<QueryTable> dstTable(new QueryTable(tupleCount, colCount, isEncrypted));
    dstTable->SetSchema(GetSchema());
    QueryTuple *srcTuple; // initialized below

    for(int i = 0; i < tupleCount; ++i)  {
        srcTuple = GetTuple(i);

        QueryTuple dstTuple(colCount, false);
        dstTuple = srcTuple->reveal(party);
        dstTable->putTuple(i, dstTuple);

    }

    return dstTable;


//    EmpManager *empManager  = EmpManager::getInstance();
//    return empManager->revealTable(this, (int) party);
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


QueryTable & QueryTable::operator=(const QueryTable & src) {
    if(&src == this)
        return *this;

    this->SetSchema(src.GetSchema());
    this->is_encrypted_ = src.GetIsEncrypted();
    this->tupleCount_ = src.getTupleCount();

    tuples_ =
            std::unique_ptr<QueryTuple[]>(new QueryTuple[tupleCount_]);

    for(int i = 0; i < tupleCount_; ++i) {
        tuples_[i] = src.tuples_[i];
    }

    return *this;
}

void QueryTable::putTuple(int idx, QueryTuple tuple) {
    tuples_[idx]  = tuple;
}
