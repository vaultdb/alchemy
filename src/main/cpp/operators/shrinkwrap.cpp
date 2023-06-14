#include "shrinkwrap.h"
#include <operators/sort.h>

using namespace vaultdb;

template<typename B>
Shrinkwrap<B>::Shrinkwrap(Operator<B> *child, const size_t &output_cardinality) : Operator<B>(child, child->getSortOrder()), cardinality_bound_(output_cardinality){
    assert(cardinality_bound_ > 0); // check initialization
}

template<typename B>
Shrinkwrap<B>::Shrinkwrap( QueryTable<B>*input, const size_t &output_cardinality)  : Operator<B>(input), cardinality_bound_(output_cardinality) {
    assert(cardinality_bound_ > 0); // check initialization
}


template<typename B>
QueryTable<B>*Shrinkwrap<B>::runSelf() {

    QueryTable<B> *input = Operator<B>::getChild(0)->getOutput();
    SortDefinition src_sort = input->getSortOrder();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = emp::CircuitExecution::circ_exec->num_and();

    if(input->getTupleCount() < cardinality_bound_) {
        Operator<B>::output_ =  input->clone();
        return Operator<B>::output_;
    }

    // preserve the initial sort order otherwise
    SortDefinition  dst_sort;
    dst_sort.push_back(ColumnSort(-1, SortDirection::ASCENDING));  // not-a-dummies go first
    for(ColumnSort c : src_sort) {
        dst_sort.push_back(c);
    }

    Sort<B> sort(input, dst_sort);
    QueryTable<B> *output_ = sort.run();

    output_->resize(cardinality_bound_);
    return output_;
}

template<typename B>
string Shrinkwrap<B>::getOperatorType() const {
    return "Shrinkwrap";
}

template<typename B>
string Shrinkwrap<B>::getParameters() const {
    return "cardinality_bound=" + std::to_string(cardinality_bound_);
}



template class vaultdb::Shrinkwrap<bool>;
template class vaultdb::Shrinkwrap<emp::Bit>;