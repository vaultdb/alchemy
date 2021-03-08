#include "filter.h"

Filter::Filter(Operator *child, shared_ptr<Predicate> predicateClass) : Operator(child) {
    predicate = predicateClass;
}

Filter::Filter(shared_ptr<QueryTable> child, shared_ptr<Predicate> predicateClass) : Operator(child) {
    predicate = predicateClass;
}

std::shared_ptr<QueryTable> Filter::runSelf() {

    std::shared_ptr<QueryTable> input = children[0]->getOutput();


    // deep copy new output, then just modify the dummy tag
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));

    for(size_t i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple tuple = output->getTuple(i);
        Field *predicateOut = predicate->predicateCall(tuple);
        Field *select = &(!(*predicateOut));

        Field *dummyTag =   &(*select | *tuple.getDummyTag()); // (!) because dummyTag is false if our selection criteria is satisfied
        delete select;
        delete predicateOut;

        output->getTuplePtr(i)->setDummyTag(dummyTag);
    }

    output->setSortOrder(input->getSortOrder());
    return output;
}



