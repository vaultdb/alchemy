#include "shrinkwrap.h"
#include <operators/sort.h>

using namespace vaultdb;

template<typename B>
Shrinkwrap<B>::Shrinkwrap(Operator<B> *child, const size_t &output_cardinality) : Operator<B>(child, child->getSortOrder()) {
    this->output_cardinality_ = output_cardinality;
    assert(this->output_cardinality_ > 0); // check initialization
}

template<typename B>
Shrinkwrap<B>::Shrinkwrap( QueryTable<B>*input, const size_t &output_cardinality)  : Operator<B>(input) {

    this->output_cardinality_ = output_cardinality;
    assert(this->output_cardinality_ > 0); // check initialization

}


template<typename B>
QueryTable<B>*Shrinkwrap<B>::runSelf() {

    QueryTable<B> *input = Operator<B>::getChild(0)->getOutput();
    SortDefinition src_sort = input->getSortOrder();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    if(input->getTupleCount() <= this->output_cardinality_) {
        Operator<B>::output_ =  input->clone();
        return Operator<B>::output_;
    }

    // preserve the initial sort order otherwise
    SortDefinition  dst_sort;
    dst_sort.push_back(ColumnSort(-1, SortDirection::ASCENDING));  // not-dummies go first
    for(ColumnSort c : src_sort) {
        dst_sort.push_back(c);
    }

    Sort<B> sort(input->clone(), dst_sort);
    QueryTable<B> *output_ = sort.run()->clone();
    this->setSortOrder(dst_sort);
    output_->resize(this->output_cardinality_);
    return output_;
}





template class vaultdb::Shrinkwrap<bool>;
template class vaultdb::Shrinkwrap<emp::Bit>;