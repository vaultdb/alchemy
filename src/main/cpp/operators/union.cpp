#include "union.h"
#include "query_table/query_table.h"

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
    this->output_cardinality_ = lhs->tuple_cnt_ + rhs->tuple_cnt_;

}




template<typename B>
QueryTable<B> * Union<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    lhs->pinned_ = true;
    QueryTable<B> *rhs = this->getChild(1)->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    assert(lhs->getSchema() == rhs->getSchema()); // union compatible
    assert(lhs->storageModel() == rhs->storageModel());

    QuerySchema output_schema = lhs->getSchema();
    size_t tuple_cnt = lhs->tuple_cnt_ + rhs->tuple_cnt_;

    // intentionally empty sort definition
    this->output_ =  QueryTable<B>::getTable(tuple_cnt, output_schema);

    this->output_->cloneTable(0, lhs);
    this->output_->cloneTable(lhs->tuple_cnt_, rhs);
    lhs->pinned_ = false;
    return this->output_;
}




template class vaultdb::Union<bool>;
template class vaultdb::Union<emp::Bit>;
