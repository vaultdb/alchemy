//
// Created by Jennie Rogers on 9/13/20.
//

#ifndef _JOIN_EQUALITY_PREDICATE_H
#define _JOIN_EQUALITY_PREDICATE_H

#include "binary_predicate.h"

// ordinals with respect to position in each source tuple
// lhs.ordinal == rhs.ordinal
typedef std::pair<uint32_t, uint32_t> EqualityPredicate;
typedef std::vector<EqualityPredicate> ConjunctiveEqualityPredicate;

// conjunctive equality predicate
// e.g., partsupp.suppkey = lineitem.suppkey AND partsupp.partkey = lineitem.partkey

class JoinEqualityPredicate : public BinaryPredicate {
public:
    JoinEqualityPredicate(const ConjunctiveEqualityPredicate & srcPredicates, bool isEncrypted);

    types::Value predicateCall( QueryTuple * lhs,  QueryTuple * rhs) const override;

private:
    std::vector<EqualityPredicate> predicates;
    types::Value defaultValue;

};


#endif //_JOIN_EQUALITY_PREDICATE_H
