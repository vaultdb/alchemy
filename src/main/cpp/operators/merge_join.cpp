#include "merge_join.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "query_table/table_factory.h"

using namespace vaultdb;

template<typename B>
QueryTable<B> *MergeJoin<B>::runSelf() {
    auto lhs = this->getChild(0)->getOutput();
    auto rhs = this->getChild(1)->getOutput();

    assert(lhs->getTupleCount() == rhs->getTupleCount());

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    this->output_ = TableFactory<B>::getTable(lhs->getTupleCount(), this->output_schema_, lhs->storageModel(), this->sort_definition_);

    for(int i = 0; i < this->output_cardinality_; ++i) {
        Join<B>::write_left(this->output_, i,  lhs, i);
        Join<B>::write_right(this->output_, i,  rhs, i);
    }

    return this->output_;
}


template<typename B>
void MergeJoin<B>::setup() {

    auto lhs = this->getChild(0);
    auto rhs = this->getChild(1);

    assert(lhs->isEncrypted() == rhs->isEncrypted()); // only support all plaintext or all MPC
    assert(lhs->getOutputCardinality() == rhs->getOutputCardinality());

    this->output_cardinality_ = lhs->getOutputCardinality();

    // require that this is sorted on the join key
    // TODO: make this robust to permutations on sort orders
    auto p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    auto join_idxs  = join_visitor.getEqualities();

    auto lhs_sort = lhs->getSortOrder();
    auto rhs_sort = rhs->getSortOrder();

    assert(lhs_sort.size() >= join_idxs.size());
    assert(rhs_sort.size() >= join_idxs.size());

    int i = 0;
    for(auto key_pair : join_idxs) {
        // visitor always outputs lhs, rhs
        assert(lhs_sort[i].first == key_pair.first);
        assert(rhs_sort[i].first == (key_pair.second - lhs->getSchema().getFieldCount()));
        assert(lhs_sort[i].second == rhs_sort[i].second);
        ++i;
    }

}