#include "filter.h"

using namespace vaultdb;


template<typename T>
Filter<T>::Filter(Operator *child, shared_ptr<Predicate<T> > & predicateClass) :
     Operator(child), predicate(predicateClass) { }

template<typename T>
Filter<T>::Filter(shared_ptr<QueryTable> child, shared_ptr<Predicate<T> >&  predicateClass) :
     Operator(child), predicate(predicateClass) { }

template<typename T>
std::shared_ptr<QueryTable> Filter<T>::runSelf() {
    std::shared_ptr<QueryTable> input = children[0]->getOutput();


    // deep copy new output, then just modify the dummy tag
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));

    for(size_t i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple tuple = output->getTuple(i);
        T dummyTag = static_cast<const T >(*tuple.getDummyTag());
        T predicateOut = predicate->predicateCall(tuple);

        dummyTag =  ((!predicateOut) | dummyTag); // (!) because dummyTag is false if our selection criteria is satisfied

        output->getTuplePtr(i)->setDummyTag(dummyTag);
    }

    output->setSortOrder(input->getSortOrder());
    return output;

}

template class vaultdb::Filter<BoolField>;
template class vaultdb::Filter<SecureBoolField>;
