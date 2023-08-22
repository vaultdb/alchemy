#include "merge_join.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "query_table/table_factory.h"
#include "util/field_utilities.h"

using namespace vaultdb;

template<typename B>
QueryTable<B> *MergeJoin<B>::runSelf() {
    auto lhs = this->getChild(0)->getOutput();
    auto rhs = this->getChild(1)->getOutput();
    assert(lhs->getTupleCount() == rhs->getTupleCount());

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();
    this->output_ = TableFactory<B>::getTable(lhs->getTupleCount(), this->output_schema_, lhs->storageModel(), this->sort_definition_);
    B selected, dst_dummy_tag;

    for(int i = 0; i < this->output_cardinality_; ++i) {
        Join<B>::write_left(this->output_, i,  lhs, i);
        Join<B>::write_right(this->output_, i,  rhs, i);
        selected = Join<B>::predicate_->call(lhs, i, rhs, i).template getValue<B>();

        if(or_dummy_tags_)
            dst_dummy_tag = (!selected) | lhs->getDummyTag(i) | rhs->getDummyTag(i);
        else
            dst_dummy_tag = (!selected) | (lhs->getDummyTag(i) & rhs->getDummyTag(i));


        this->output_->setDummyTag(i, dst_dummy_tag);
    }
    return this->output_;
}


template<typename B>
void MergeJoin<B>::setup() {

    updateCollation();
    auto lhs = this->getChild(0);
    auto rhs = this->getChild(1);
    assert(lhs->getOutputCardinality() == rhs->getOutputCardinality());

    this->output_cardinality_ = lhs->getOutputCardinality();


}

template class vaultdb::MergeJoin<bool>;
template class vaultdb::MergeJoin<emp::Bit>;