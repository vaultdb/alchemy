//
// Created by Jennie Rogers on 8/15/20.
//

#include "filter.h"

Filter::Filter(std::shared_ptr<Predicate> &predicateClass, std::shared_ptr<Operator> &child) : Operator(child) {
    predicate = predicateClass;
}


std::shared_ptr<QueryTable> Filter::runSelf() {

    std::shared_ptr<QueryTable> input = children[0]->getOutput();


    // deep copy new output, then just modify the dummy tag
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));

    for(int i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple tuple = output->getTuple(i);
        types::Value dummyTag = !(predicate->predicateCall(tuple)); // (!) because dummyTag is false if our selection criteria is satisfied
        tuple.setDummyTag(dummyTag);

        output->setTupleDummyTag(i, dummyTag);
    }
    return output;
}



