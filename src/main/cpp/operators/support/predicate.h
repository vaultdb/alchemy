#ifndef _PREDICATE_CLASS_H
#define _PREDICATE_CLASS_H

#include <query_table/query_tuple.h>

using namespace vaultdb;
using namespace emp;

// predicates will inherit from this class
// it is a shell for creating state for the predicate, e.g., encrypted constants for equality checks
class Predicate {

public:
    Predicate() {}
    ~Predicate() {}
    // override when we instantiate a predicate
    virtual types::Value predicateCall(const QueryTuple & aTuple) const = 0;

};



typedef  types::Value (Predicate::*predicateCall)(const QueryTuple & aTuple);
#endif //_PREDICATE_CLASS_H
