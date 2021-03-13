#include "filter.h"

using namespace vaultdb;


Filter<BoolField>::Filter(Operator *child, shared_ptr<Predicate<BoolField> > & predicateClass) :
     Operator(child), predicate(predicateClass) { }


Filter<BoolField>::Filter(shared_ptr<QueryTable> child, shared_ptr<Predicate<BoolField> >&  predicateClass) :
     Operator(child), predicate(predicateClass) { }

std::shared_ptr<QueryTable> Filter<BoolField>::runSelf() {

    std::shared_ptr<QueryTable> input = children[0]->getOutput();


    // deep copy new output, then just modify the dummy tag
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));

    for(size_t i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple tuple = output->getTuple(i);
        BoolField dummyTag = *((BoolField *) tuple.getDummyTag());
        BoolField predicateOut = predicate->predicateCall(tuple);

        dummyTag =  ((!predicateOut) | dummyTag); // (!) because dummyTag is false if our selection criteria is satisfied

        output->getTuplePtr(i)->setDummyTag(dummyTag);
    }

    output->setSortOrder(input->getSortOrder());
    return output;
}

std::shared_ptr<QueryTable> Filter<SecureBoolField>::runSelf() {

    std::shared_ptr<QueryTable> input = children[0]->getOutput();


    // deep copy new output, then just modify the dummy tag
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));

    for(size_t i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple tuple = output->getTuple(i);
        SecureBoolField dummyTag = *((SecureBoolField *) tuple.getDummyTag());
        SecureBoolField predicateOut = predicate->predicateCall(tuple);

        dummyTag =  ((!predicateOut) | dummyTag); // (!) because dummyTag is false if our selection criteria is satisfied

        output->getTuplePtr(i)->setDummyTag(dummyTag);
    }

    output->setSortOrder(input->getSortOrder());
    return output;

}

