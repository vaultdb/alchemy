//
// Created by Jennie Rogers on 9/13/20.
//

#include "join_equality_predicate.h"

JoinEqualityPredicate::JoinEqualityPredicate(const ConjunctiveEqualityPredicate &srcPredicates, bool isEncrypted)  : predicates(srcPredicates) {
    defaultValue = isEncrypted ?  types::Value(emp::Bit(true)) : types::Value(true);

}


types::Value JoinEqualityPredicate::predicateCall( QueryTuple *lhs,  QueryTuple *rhs) const {

    types::Value result = defaultValue;
   for(EqualityPredicate eq : predicates) {
       types::Value lhsValue = lhs->getField(eq.first).getValue();
       types::Value rhsValue = rhs->getField(eq.second).getValue();

       result = result & (lhsValue == rhsValue);
   }

   return result;

}

