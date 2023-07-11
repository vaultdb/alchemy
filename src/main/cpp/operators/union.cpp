#include "union.h"
#include "query_table/table_factory.h"

using namespace vaultdb;


template<typename B>
Union<B>::Union(Operator<B> *lhs, Operator<B> *rhs) : Operator<B>(lhs, rhs) {
    if(lhs == nullptr || rhs == nullptr) {
        throw std::invalid_argument("Union can't take in null child operators!");
    }
    this->sort_definition_.clear(); // union will change collation
    this->output_schema_ = lhs->getOutputSchema();
    this->output_cardinality_ = lhs->getOutputCardinality() + rhs->getOutputCardinality();
}

template<typename B>
Union<B>::Union(QueryTable<B> *lhs, QueryTable<B> *rhs) : Operator<B>(lhs, rhs) {
    if(lhs == nullptr || rhs == nullptr) {
        throw std::invalid_argument("Union can't take in null tables!");
    }
    this->output_schema_  = lhs->getSchema();
    this->sort_definition_.clear();
    this->output_cardinality_ = lhs->getTupleCount() + rhs->getTupleCount();

}




template<typename B>
QueryTable<B> * Union<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    QueryTable<B> *rhs = this->getChild(1)->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    assert(lhs->getSchema() == rhs->getSchema()); // union compatible
    assert(lhs->storageModel() == rhs->storageModel());

    QuerySchema output_schema = lhs->getSchema();
    size_t tuple_cnt = lhs->getTupleCount() + rhs->getTupleCount();

    // intentionally empty sort definition
    this->output_ = TableFactory<B>::getTable(tuple_cnt, output_schema, lhs->storageModel());

    this->output_->cloneTable(0, lhs);
    this->output_->cloneTable(lhs->getTupleCount(), rhs);

    return this->output_;
}

template<typename B>
string Union<B>::getOperatorType() const {
    return "Union";
}

template<typename B>
string Union<B>::getParameters() const {
    return std::string();
}


template class vaultdb::Union<bool>;
template class vaultdb::Union<emp::Bit>;
