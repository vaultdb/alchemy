//
// Created by Jennie Rogers on 8/15/20.
//

#ifndef _FILTER_H
#define _FILTER_H

#include <operators/support/predicate.h>
#include "operator.h"


// predicate function needs aware of encrypted or plaintext state of its inputs
class Filter : public Operator {

    std::shared_ptr<Predicate> predicate;


public:
    Filter(std::shared_ptr<Predicate> &predicateClass, std::shared_ptr<Operator> &child);
    std::shared_ptr<QueryTable> runSelf() override;
};


#endif // _FILTER_H
