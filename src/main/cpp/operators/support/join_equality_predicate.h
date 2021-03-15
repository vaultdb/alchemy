#ifndef _JOIN_EQUALITY_PREDICATE_H
#define _JOIN_EQUALITY_PREDICATE_H

#include "binary_predicate.h"

// ordinals with respect to position in each source tuple
// lhs.ordinal == rhs.ordinal
typedef std::pair<size_t, size_t> EqualityPredicate;
typedef std::vector<EqualityPredicate> ConjunctiveEqualityPredicate;

// conjunctive equality predicate
// e.g., partsupp.suppkey = lineitem.suppkey AND partsupp.partkey = lineitem.partkey

template <typename T>
class JoinEqualityPredicate : public BinaryPredicate<T> {
public:
    JoinEqualityPredicate(const ConjunctiveEqualityPredicate & srcPredicates, bool isEncrypted) {}

    virtual T predicateCall( const QueryTuple * lhs, const QueryTuple * rhs) const override { throw; }; // we should never get here, use specializations below instead


};



template <>
class JoinEqualityPredicate<BoolField> : public BinaryPredicate<BoolField> {
public:
    JoinEqualityPredicate(const ConjunctiveEqualityPredicate & srcPredicates);
    BoolField predicateCall(const QueryTuple * lhs, const QueryTuple *rhs) const override;

private:
    ConjunctiveEqualityPredicate predicate;
};

template <>
class JoinEqualityPredicate<SecureBoolField> : public BinaryPredicate<SecureBoolField> {
public:
    JoinEqualityPredicate(const ConjunctiveEqualityPredicate & srcPredicates);
    SecureBoolField predicateCall(const QueryTuple * lhs, const QueryTuple *rhs) const override;

private:
    ConjunctiveEqualityPredicate predicate;
};



#endif //_JOIN_EQUALITY_PREDICATE_H
