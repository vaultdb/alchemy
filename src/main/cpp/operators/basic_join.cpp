#include "basic_join.h"
#include "query_table/table_factory.h"
#include "util/field_utilities.h"

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
            this->output_cardinality_ = lhs->getTupleCount() * rhs->getTupleCount();

        this->updateCollation();
}

template<typename B>
QueryTable<B> *BasicJoin<B>::runSelf() {
    QueryTable<B> *lhs = Operator<B>::getChild(0)->getOutput();
    lhs->pinned_ = true;
    QueryTable<B> *rhs = Operator<B>::getChild(1)->getOutput();
    B predicate_eval;

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();


    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC


    B selected, dst_dummy_tag, lhs_dummy_tag;
    // output size, colCount, is_encrypted
    this->output_ = TableFactory<B>::getTable(lhs->getTupleCount() * rhs->getTupleCount(), this->output_schema_, lhs->storageModel(), this->sort_definition_);
    int cursor = 0;

    for(uint32_t i = 0; i < lhs->getTupleCount(); ++i) {
         lhs_dummy_tag  = lhs->getDummyTag(i);
        for(uint32_t j = 0; j < rhs->getTupleCount(); ++j) {
            Join<B>::write_left(this->output_, cursor,  lhs, i);
            Join<B>::write_right(this->output_, cursor,  rhs, j);
            selected = Join<B>::predicate_->call(lhs, i, rhs, j).template getValue<B>();
            dst_dummy_tag = (!selected) | lhs_dummy_tag | rhs->getDummyTag(j);

            Operator<B>::output_->setDummyTag(cursor, dst_dummy_tag);
            ++cursor;
        }
    }

    lhs->pinned_ = false;
    return this->output_;
}



template class vaultdb::BasicJoin<bool>;
template class vaultdb::BasicJoin<emp::Bit>;