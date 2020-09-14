//
// Created by Jennie Rogers on 9/13/20.
//
// creates a cross product of its input relations, sets the dummy tag in output to reflect selected tuples

#ifndef _BASIC_JOIN_H
#define _BASIC_JOIN_H


#include "join.h"


class BasicJoin  : public Join {

    public:
        BasicJoin(std::shared_ptr<BinaryPredicate> &predicateClass, std::shared_ptr<Operator> &lhs, std::shared_ptr<Operator> &rhs);
        std::shared_ptr<QueryTable> runSelf() override;


};


#endif //_BASIC_JOIN_H
