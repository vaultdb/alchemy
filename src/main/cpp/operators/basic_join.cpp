#include "basic_join.h"
#include <query_table/query_table.h>

using namespace vaultdb;

template<typename B>
BasicJoin<B>::BasicJoin(Operator<B> *lhs, Operator<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort) {
            this->output_cardinality_ = lhs->getOutputCardinality() * rhs->getOutputCardinality();
            this->updateCollation();
        }

template<typename B>
BasicJoin<B>::BasicJoin(QueryTable<B> *lhs, QueryTable<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort) {
            this->output_cardinality_ = lhs->tuple_cnt_ * rhs->tuple_cnt_;

        this->updateCollation();
}

template<typename B>
QueryTable<B> *BasicJoin<B>::runSelf() {
    QueryTable<B> *lhs = Operator<B>::getChild(0)->getOutput();
    lhs->pinned_ = true;
    QueryTable<B> *rhs = Operator<B>::getChild(1)->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();


    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC


    B selected, dst_dummy_tag, lhs_dummy_tag;
    // output size, colCount, is_encrypted
    this->output_ =  QueryTable<B>::getTable(lhs->tuple_cnt_ * rhs->tuple_cnt_, this->output_schema_,
                                              this->sort_definition_);
    int cursor = 0;
    int rhs_col_offset = this->output_->getSchema().getFieldCount() - rhs->getSchema().getFieldCount();


    for(uint32_t i = 0; i < lhs->tuple_cnt_; ++i) {
         lhs_dummy_tag  = lhs->getDummyTag(i);

         // copy the lhs |rhs| times
         this->output_->cloneRowRange(cursor, 0, lhs, i, rhs->tuple_cnt_);

        // initialize the rhs output
        this->output_->cloneTable(cursor, rhs_col_offset, rhs);

        // make |rhs| copies of the lhs row
        for(uint32_t j = 0; j < rhs->tuple_cnt_; ++j) {
            selected = Join<B>::predicate_->call(lhs, i, rhs, j).template getValue<B>();
            dst_dummy_tag = (!selected) | lhs_dummy_tag | rhs->getDummyTag(j);
            this->output_->setDummyTag(cursor, dst_dummy_tag);
            ++cursor;
        }
    }

    lhs->pinned_ = false;
    return this->output_;
}



template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;