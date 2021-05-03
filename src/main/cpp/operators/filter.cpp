#include "filter.h"
#include "plain_tuple.h"
#include "secure_tuple.h"

using namespace vaultdb;


template<typename B>
Filter<B>::Filter(Operator<B> *child, BoolExpression<B> & predicate) :
     Operator<B>(child, child->getSortOrder()), predicate_(predicate) {
     }

template<typename B>
Filter<B>::Filter(shared_ptr<QueryTable<B> > child, BoolExpression<B> & predicate) :
     Operator<B>(child, child->getSortOrder()), predicate_(predicate) { }

template<typename B>
std::shared_ptr<QueryTable<B> > Filter<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();

    // deep copy new output, then just modify the dummy tag
    Operator<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));

    for(size_t i = 0; i < Operator<B>::output_->getTupleCount(); ++i) {
        QueryTuple tuple = Operator<B>::output_->getTuple(i);
        B dummyTag = tuple.getDummyTag();
        B predicateOut = predicate_.callBoolExpression(tuple);

        dummyTag =  ((!predicateOut) | dummyTag); // (!) because dummyTag is false if our selection criteria is satisfied

        QueryTuple<B> to_write = Operator<B>::output_->getTuple(i); // container pointer to source data
        to_write.setDummyTag(dummyTag);

    }

    Operator<B>::output_->setSortOrder(input->getSortOrder());
    return Operator<B>::output_;

}

template class vaultdb::Filter<bool>;
template class vaultdb::Filter<emp::Bit>;
