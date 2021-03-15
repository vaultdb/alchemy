#ifndef _BINARY_PREDICATE_H
#define _BINARY_PREDICATE_H

#include <query_table/query_tuple.h>


using namespace vaultdb;
using namespace emp;

// predicates will inherit from this class
// it is a shell for creating state for the predicate, e.g., encrypted constants for equality checks
template<typename T>
class BinaryPredicate {

public:
    BinaryPredicate() {}
    virtual ~BinaryPredicate() {}

    // override when we instantiate a predicate -- only need to implement one of these per predicate class
    virtual T predicateCall(const QueryTuple * lhs, const QueryTuple * rhs) const = 0;

};

//typedef  Field * (BinaryPredicate::*binaryPredicateCall)(QueryTuple * lhs,  QueryTuple * rhs);


#endif //_BINARY_PREDICATE_H
