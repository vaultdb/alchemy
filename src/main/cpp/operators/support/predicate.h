//
// Created by Jennie Rogers on 8/16/20.
//

#ifndef _PREDICATE_CLASS_H
#define _PREDICATE_CLASS_H

#include <querytable/query_tuple.h>

using namespace vaultdb;
using namespace emp;

// predicates will inherit from this class
// it is a shell for creating state for the predicate, e.g., encrypted constants for equality checks
class PredicateClass {

public:
    PredicateClass() {}
    ~PredicateClass() {}
    // override when we instantiate a predicate
    virtual types::Value predicateCall(const QueryTuple & aTuple) const = 0;


};



typedef  types::Value (PredicateClass::*predicateCall)(const QueryTuple & aTuple);

#endif //_PREDICATE_CLASS_H
