#include <util/type_utilities.h>
#include "join_equality_predicate.h"
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename T>
JoinEqualityPredicate<T>::JoinEqualityPredicate(const ConjunctiveEqualityPredicate &srcPredicates)  : predicate(srcPredicates) {

}

template<typename T>
T JoinEqualityPredicate<T>::predicateCall(const  QueryTuple *lhs, const QueryTuple *rhs) const {

    T result = T(true);
   for(EqualityPredicate p : predicate) {
       const Field *lhsField = lhs->getField(p.first);
       const Field *rhsField = rhs->getField(p.second);
       Field *equal = FieldUtilities::equal(lhsField, rhsField);
       T eq = static_cast<T &>(*equal);
       result = result & eq;

       delete equal;

   }

   return result;

}

/*

JoinEqualityPredicate<BoolField>::JoinEqualityPredicate(const ConjunctiveEqualityPredicate &srcPredicates) : predicate(srcPredicates){

}


BoolField JoinEqualityPredicate<BoolField>::predicateCall(const QueryTuple * lhs, const QueryTuple *rhs) const  {
    BoolField result =  BoolField(true);

    for(EqualityPredicate eq : predicate) {
        const Field *lhsField = lhs->getField(eq.first);
        const Field *rhsField = rhs->getField(eq.second);
        BoolField match(FieldUtilities::equal(lhsField, rhsField));
        result = result & match;
    }
    return result;

}


JoinEqualityPredicate<SecureBoolField>::JoinEqualityPredicate(const ConjunctiveEqualityPredicate &srcPredicates) : predicate(srcPredicates){

}


SecureBoolField JoinEqualityPredicate<SecureBoolField>::predicateCall(const QueryTuple * lhs, const QueryTuple *rhs) const  {
    SecureBoolField result =  SecureBoolField(true);

    for(EqualityPredicate eq : predicate) {
        const Field *lhsField = lhs->getField(eq.first);
        const Field *rhsField = rhs->getField(eq.second);
        Field *equal = FieldUtilities::equal(lhsField, rhsField);
        SecureBoolField *match = static_cast<SecureBoolField *>(equal);
        result = result & *match;
    }
    return result;

}
*/

template class vaultdb::JoinEqualityPredicate<BoolField>;
template class vaultdb::JoinEqualityPredicate<SecureBoolField>;

