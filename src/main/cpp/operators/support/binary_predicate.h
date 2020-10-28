#ifndef _BINARY_PREDICATE_H
#define _BINARY_PREDICATE_H

#include <query_table/query_tuple.h>


using namespace vaultdb;
using namespace emp;

// predicates will inherit from this class
// it is a shell for creating state for the predicate, e.g., encrypted constants for equality checks
class BinaryPredicate {

public:
    BinaryPredicate() {}
    ~BinaryPredicate() {}

    // override when we instantiate a predicate -- only need to implement one of these per predicate class
    virtual types::Value predicateCall( QueryTuple * lhs,  QueryTuple * rhs) const = 0;

};

typedef  types::Value (BinaryPredicate::*binaryPredicateCall)(QueryTuple * lhs,  QueryTuple * rhs);


#endif //_BINARY_PREDICATE_H
