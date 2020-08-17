//
// Created by Jennie Rogers on 8/15/20.
//

#ifndef _FILTER_H
#define _FILTER_H

#include "operator.h"


// predicate function needs aware of encrypted or plaintext state of its inputs
class Filter : public Operator {

    types::Value(*predicate)(const QueryTuple &); // predicate function pointer

public:
    Filter(types::Value(*predicateFunction)(const QueryTuple & tuple), std::shared_ptr<Operator> &child);
    std::shared_ptr<QueryTable> runSelf() override;
};


#endif // _FILTER_H
