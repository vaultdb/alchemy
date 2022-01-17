#include "shrinkwrap.h"
#include <operators/sort.h>

using namespace vaultdb;

template<typename B>
Shrinkwrap<B>::Shrinkwrap(Operator<B> *child, const size_t &output_cardinality) : Operator<B>(child, child->getSortOrder()), cardinality_bound_(output_cardinality){
    assert(cardinality_bound_ > 0); // check initialization
}

template<typename B>
Shrinkwrap<B>::Shrinkwrap(shared_ptr<QueryTable<B>> &input, const size_t &output_cardinality)  : Operator<B>(input), cardinality_bound_(output_cardinality) {
    assert(cardinality_bound_ > 0); // check initialization
}


template<typename B>
shared_ptr<QueryTable<B>> Shrinkwrap<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();
    SortDefinition src_sort = input->getSortOrder();


    // preserve the initial sort order otherwise
    SortDefinition  dst_sort;
    dst_sort.push_back(ColumnSort(-1, SortDirection::ASCENDING));  // not-a-dummies go first
    for(int i = 0; i < src_sort.size(); ++i) {
        ColumnSort c = src_sort[i];
        dst_sort.push_back(c);
    }

    Sort<B> sort(input, dst_sort);
    shared_ptr<QueryTable<B>> output_ = sort.run();

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