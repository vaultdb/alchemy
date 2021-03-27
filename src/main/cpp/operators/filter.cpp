#include "filter.h"

using namespace vaultdb;


template<typename B>
Filter<B>::Filter(Operator<B> *child, shared_ptr<Predicate<B> > & predicateClass) :
     Operator<B>(child), predicate(predicateClass) { }

template<typename B>
Filter<B>::Filter(shared_ptr<QueryTable<B> > child, shared_ptr<Predicate<B> >&  predicateClass) :
     Operator<B>(child), predicate(predicateClass) { }

template<typename B>
std::shared_ptr<QueryTable<B> > Filter<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children[0]->getOutput();


    // deep copy new output, then just modify the dummy tag
    Operator<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));

    for(size_t i = 0; i < Operator<B>::output->getTupleCount(); ++i) {
        QueryTuple tuple = Operator<B>::output->getTuple(i);
        B dummyTag = tuple.getDummyTag();
        B predicateOut = predicate->predicateCall(tuple);

        dummyTag =  ((!predicateOut) | dummyTag); // (!) because dummyTag is false if our selection criteria is satisfied

        Operator<B>::output->getTuplePtr(i)->setDummyTag(dummyTag);
    }

    Operator<B>::output->setSortOrder(input->getSortOrder());
    return Operator<B>::output;

}

template class vaultdb::Filter<bool>;
template class vaultdb::Filter<emp::Bit>;
