//
// Created by Jennie Rogers on 8/15/20.
//

#include "filter.h"

Filter::Filter(std::shared_ptr<PredicateClass> &predicateClass, std::shared_ptr<Operator> &child) : Operator(child) {
    predicate = predicateClass;
}


std::shared_ptr<QueryTable> Filter::runSelf() {

    std::shared_ptr<QueryTable> input = children[0]->getOutput();
    // deep copy new output
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));

    for(int i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple tuple = output->getTuple(i);
        types::Value dummyTag = predicate->predicateCall(tuple);
        std::cout << "Have dummy tag: " << dummyTag.reveal() << " at " << dummyTag;
        tuple.setDummyTag(dummyTag);
       std::cout << ", tuple dummy tag: " << output->getTuple(i).getDummyTag().reveal() << std::endl;
    }
    return output;
}



