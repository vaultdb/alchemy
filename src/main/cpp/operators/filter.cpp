//
// Created by Jennie Rogers on 8/15/20.
//

#include "filter.h"

Filter::Filter(std::shared_ptr<PredicateClass> &predicateClass, std::shared_ptr<Operator> &child) : Operator(child) {
    predicate = predicateClass;
}


std::shared_ptr<QueryTable> Filter::runSelf() {

    output = std::shared_ptr<QueryTable>(new QueryTable(*(children[0]->getOutput())));

    for(int i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple *tuple = output->getTuple(i);
        types::Value dummyTag = predicate->predicateCall(*tuple);
        tuple->SetDummyTag(&dummyTag);
    }
    return output;
}



