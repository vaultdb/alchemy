#include "merge_join.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "query_table/table_factory.h"
#include "util/field_utilities.h"

using namespace vaultdb;

template<typename B>
QueryTable<B> *MergeJoin<B>::runSelf() {
    auto lhs = this->getChild(0)->getOutput();
    lhs->pinned_ = true;
    auto rhs = this->getChild(1)->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();
    this->output_ = TableFactory<B>::getTable(this->output_cardinality_, this->output_schema_,
                                              this->sort_definition_);

    int lhs_card = lhs->getTupleCount();
    int rhs_card = rhs->getTupleCount();

    bool one_non_empty = (lhs_card > 0 || rhs_card > 0);  // need at least one non-empty input
    bool one_side_empty = (lhs_card == 0 || rhs_card == 0); // other input is empty
    bool two_sided_input = (lhs_card == rhs_card && lhs_card > 0);
    bool one_sided_input = (one_side_empty & one_non_empty);
    int rhs_col_offset = this->output_->getSchema().getFieldCount() - rhs->getSchema().getFieldCount();
    if (two_sided_input) {

        B selected, dst_dummy_tag;

        for (int i = 0; i < this->output_cardinality_; ++i) {
            this->output_->cloneRow(i, 0, lhs, i); // Join<B>::write_left(this->output_, i, lhs, i);
            this->output_->cloneRow(i, rhs_col_offset, rhs, i); //  Join<B>::write_right(this->output_, i, rhs, i);
            selected = Join<B>::predicate_->call(lhs, i, rhs, i).template getValue<B>();

            if (or_dummy_tags_)
                dst_dummy_tag = (!selected) | lhs->getDummyTag(i) | rhs->getDummyTag(i);
            else
                dst_dummy_tag = (!selected) | (lhs->getDummyTag(i) & rhs->getDummyTag(i));


            this->output_->setDummyTag(i, dst_dummy_tag);
        }
    }
    else if(one_sided_input) {
        // all others have a single party providing input, so pad the remaining fields with zeros

        QueryTable<B> *non_empty;
        if(lhs_card > 0) {
            non_empty = lhs;
        }
        else {
            non_empty = rhs;
        }

        for(int i = 0; i < this->getOutputCardinality(); ++i) {
            Join<B>::write_left(this->output_, i, non_empty, i);
            this->output_->setDummyTag(i, non_empty->getDummyTag(i));
        }

    }
    else {
        // two non-empty inputs, different cardinalities
        throw std::runtime_error("MergeJoin: unsupported input configuration");
    }

    lhs->pinned_ = false;
    return this->output_;
}


template<typename B>
void MergeJoin<B>::setup() {

    updateCollation();
    auto lhs = this->getChild(0);
    auto rhs = this->getChild(1);


    int lhs_card = lhs->getOutputCardinality();
    int rhs_card = rhs->getOutputCardinality();

    bool one_non_empty = (lhs_card > 0 || rhs_card > 0);  // need at least one non-empty input
    bool one_side_empty = (lhs_card == 0 || rhs_card == 0); // other input is empty
    bool two_sided_input = (lhs_card == rhs_card && lhs_card > 0);
    bool one_sided_input = (one_side_empty & one_non_empty);

    assert(one_sided_input || two_sided_input);

    this->output_cardinality_ = std::max(lhs->getOutputCardinality(), rhs->getOutputCardinality());


}

template class vaultdb::MergeJoin<bool>;
template class vaultdb::MergeJoin<emp::Bit>;