#include "union.h"
#include "query_table/query_table.h"
#include "project.h"

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

    size_t tuple_cnt = lhs->tuple_cnt_ + rhs->tuple_cnt_;

    // sometimes these tables have the same logical schema, but different underlying encoding.
    // only addressing bit packing for now.
    // if one side has smaller encoding than the other, take the max one and project the other one over.
    QuerySchema dst_schema = lhs->getSchema();
    auto rhs_schema = rhs->getSchema();
    bool fast_lane = true;

    for(int i = 0; i < dst_schema.getFieldCount(); ++i) {
        int l = dst_schema.getField(i).size();
        int r = rhs_schema.getField(i).size();
        if(r != l) fast_lane = false;
        if(r > l) { // rhs > lhs, lhs needs to project
            dst_schema.putField(rhs_schema.getField(i));
        }
    }

    if(fast_lane) {
        // intentionally empty sort definition
        this->output_ = QueryTable<B>::getTable(tuple_cnt, dst_schema);

        this->output_->cloneTable(0, lhs);
        this->output_->cloneTable(lhs->tuple_cnt_, rhs);
        lhs->pinned_ = false;
        return this->output_;
    }
    // else copy in one field at a time
    this->output_ =  QueryTable<B>::getTable(tuple_cnt, dst_schema);
    int cursor = 0;
    // TODO: decide on cloneColumn vs field-at-a-time once per column
    for(int i = 0; i < lhs->tuple_cnt_; ++i) {
        for(int j = 0; j < dst_schema.getFieldCount(); ++j) {
            auto f = lhs->getField(i, j);
            this->output_->setField(cursor, j, f);
        }
        auto f = lhs->getField(i, -1);
        this->output_->setField(cursor, -1, f);
        ++cursor;
    }


    for(int i = 0; i < rhs->tuple_cnt_; ++i) {
        for(int j = 0; j < dst_schema.getFieldCount(); ++j) {
            auto f = rhs->getField(i, j);
            this->output_->setField(cursor, j, f);
        }
        auto f = lhs->getField(i, -1);
        this->output_->setField(cursor, -1, f);
        ++cursor;
    }


    lhs->pinned_ = false;
    return this->output_;

}




template class vaultdb::Union<bool>;
template class vaultdb::Union<emp::Bit>;
