#include "join_equality_predicate.h"
#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>


using namespace vaultdb;

template<typename B>
JoinEqualityPredicate<B>::JoinEqualityPredicate(const ConjunctiveEqualityPredicate &srcPredicates)  : predicate(srcPredicates) {

}

template<typename B>
B JoinEqualityPredicate<B>::predicateCall(const  QueryTuple<B>  & lhs, const QueryTuple<B>  & rhs) const {

    B result = B(true);
   for(EqualityPredicate p : predicate) {
       B eq = lhs.getField(p.first) == rhs.getField(p.second);
       result = result & eq;

   }

   return result;

}


template class vaultdb::JoinEqualityPredicate<bool>;
template class vaultdb::JoinEqualityPredicate<emp::Bit>;

