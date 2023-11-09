#include "shrinkwrap.h"
#include <operators/sort.h>

using namespace vaultdb;

template<typename B>
Shrinkwrap<B>::Shrinkwrap(Operator<B> *child, const size_t &output_cardinality) : Operator<B>(child, child->getSortOrder()) {
    this->output_cardinality_ = output_cardinality;
    assert(this->output_cardinality_ > 0); // check initialization
    updateCollation();
}

template<typename B>
Shrinkwrap<B>::Shrinkwrap( QueryTable<B>*input, const size_t &output_cardinality)  : Operator<B>(input) {

    this->output_cardinality_ = output_cardinality;
    assert(this->output_cardinality_ > 0); // check initialization
    updateCollation();

}


template<typename B>
QueryTable<B>*Shrinkwrap<B>::runSelf() {

    QueryTable<B> *input = this->getChild(0)->getOutput();


    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    if(input->tuple_cnt_ <= this->output_cardinality_) {
        Operator<B>::output_ =  input->clone();
        return Operator<B>::output_;
    }

    // preserve the initial sort order otherwise
    Sort<B> sort(input->clone(), this->sort_definition_);
    QueryTable<B> *output_ = sort.run()->clone();
    this->setSortOrder(this->sort_definition_);
    output_->resize(this->output_cardinality_);
    return output_;
}





template class vaultdb::Shrinkwrap<bool>;
template class vaultdb::Shrinkwrap<emp::Bit>;