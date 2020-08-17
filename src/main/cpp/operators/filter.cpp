//
// Created by Jennie Rogers on 8/15/20.
//

#include "filter.h"

Filter::Filter(types::Value (*predicateFunction)(const QueryTuple &), std::shared_ptr<Operator> & child)  : Operator(child) {
        predicate = predicateFunction;

}

std::shared_ptr<QueryTable> Filter::runSelf() {

    output = std::shared_ptr<QueryTable>(new QueryTable(*(children[0]->getOutput())));

    for(int i = 0; i < output->getTupleCount(); ++i) {
        QueryTuple *tuple = output->getTuple(i);
        types::Value dummyTag = predicate(*tuple);
        tuple->SetDummyTag(&dummyTag);
    }
    return output;
}


