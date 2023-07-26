#include "merge_join.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "query_table/table_factory.h"
#include "util/field_utilities.h"

using namespace vaultdb;

template<typename B>
QueryTable<B> *MergeJoin<B>::runSelf() {
    // This Merge Join works like right join, we keep all rows in right table.
    auto lhs = this->getChild(0)->getOutput();
    auto rhs = this->getChild(1)->getOutput();

    assert(lhs->getTupleCount() == rhs->getTupleCount());

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();
    this->output_ = TableFactory<B>::getTable(lhs->getTupleCount(), this->output_schema_, lhs->storageModel(), this->sort_definition_);
    B selected, dst_dummy_tag, lhs_dummy_tag, rhs_dummy_tag;

    int write_idx = 0;
    for(int i = 0; i < this->output_cardinality_; ++i) {

        lhs_dummy_tag = lhs->getDummyTag(i);
        rhs_dummy_tag = rhs->getDummyTag(i);

        if(this->getLeafNodeFlag()) {
            if(FieldUtilities::extract_bool(lhs_dummy_tag & rhs_dummy_tag))
                continue;

            Join<B>::write_left(this->output_, write_idx,  lhs, i);
            Join<B>::write_right(this->output_, write_idx,  rhs, i);
            selected = Join<B>::predicate_->call(lhs, i, rhs, i).template getValue<B>();

            dst_dummy_tag = (!selected) | lhs_dummy_tag & rhs_dummy_tag;
            this->output_->setDummyTag(write_idx, dst_dummy_tag);
            write_idx++;
        }
        else {
            Join<B>::write_left(this->output_, write_idx,  lhs, i);
            Join<B>::write_right(this->output_, write_idx,  rhs, i);
            selected = Join<B>::predicate_->call(lhs, i, rhs, i).template getValue<B>();

            dst_dummy_tag = (!selected) | lhs_dummy_tag | rhs_dummy_tag;
            this->output_->setDummyTag(write_idx, dst_dummy_tag);
            write_idx++;
        }
    }
    this->output_cardinality_ = write_idx;
    this->output_->tuple_cnt_ = write_idx;
    return this->output_;
}


template<typename B>
void MergeJoin<B>::setup() {

    auto lhs = this->getChild(0);
    auto rhs = this->getChild(1);

    // just use lhs sort order for now, can use one or sides' since they are equivalent for equi-join
    this->sort_definition_ = lhs->getSortOrder();

    assert(lhs->getOutputCardinality() == rhs->getOutputCardinality());

    this->output_cardinality_ = lhs->getOutputCardinality();

    // require that this is sorted on the join key
    // TODO: make this robust to permutations in sort column order
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
        assert(rhs_sort[i].first == (key_pair.second - lhs->getOutputSchema().getFieldCount()));
        assert(lhs_sort[i].second == rhs_sort[i].second);
        ++i;
    }

}

template class vaultdb::MergeJoin<bool>;
template class vaultdb::MergeJoin<emp::Bit>;