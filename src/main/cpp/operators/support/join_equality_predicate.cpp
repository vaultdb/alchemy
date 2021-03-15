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


template class vaultdb::JoinEqualityPredicate<BoolField>;
template class vaultdb::JoinEqualityPredicate<SecureBoolField>;

