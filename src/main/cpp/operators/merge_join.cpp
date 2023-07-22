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
    B selected, dst_dummy_tag;

    int lhs_idx = 0;
    for(int i = 0; i < this->output_cardinality_; ++i) {
        selected = Join<B>::predicate_->call(lhs, lhs_idx, rhs, i).template getValue<B>();

        // Only Iterate when lhs is not dummy value, if dummy value, just write down.
        // Iterate until selected got true, Find next matched left tuple
        if(FieldUtilities::extract_bool(selected)){
            while(FieldUtilities::extract_bool(lhs->getDummyTag(lhs_idx))){
                lhs_idx++;
            }

            // In case if there are dummies with same key in a row
            selected = Join<B>::predicate_->call(lhs, lhs_idx, rhs, i).template getValue<B>();
            if(!FieldUtilities::extract_bool(selected))
                lhs_idx--;

        } else{
            while(!FieldUtilities::extract_bool(selected))
                selected = Join<B>::predicate_->call(lhs, ++lhs_idx, rhs, i).template getValue<B>();

            while(FieldUtilities::extract_bool(lhs->getDummyTag(lhs_idx))){
                selected = Join<B>::predicate_->call(lhs, ++lhs_idx, rhs, i).template getValue<B>();
                if(!FieldUtilities::extract_bool(selected)){
                    lhs_idx--;
                    break;
                }
            }

        }
        selected = Join<B>::predicate_->call(lhs, lhs_idx, rhs, i).template getValue<B>();
        Join<B>::write_left(this->output_, i,  lhs, lhs_idx);
        Join<B>::write_right(this->output_, i,  rhs, i);
        dst_dummy_tag = (!selected) | lhs->getDummyTag(lhs_idx) | rhs->getDummyTag(i);

        this->output_->setDummyTag(i, dst_dummy_tag);
    }
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