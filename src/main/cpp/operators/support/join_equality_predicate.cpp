#include <util/type_utilities.h>
#include "join_equality_predicate.h"
#include <query_table/field/field_factory.h>
#include <util/field_utilities.h>

using namespace vaultdb;

/*JoinEqualityPredicate::JoinEqualityPredicate(const ConjunctiveEqualityPredicate &srcPredicates, bool isEncrypted)  : predicates(srcPredicates) {
    if(isEncrypted)
        resultType = FieldType::SECURE_BOOL;
}


// TODO: make this templated
// IntField, BoolField
Field *JoinEqualityPredicate::predicateCall( QueryTuple *lhs,  QueryTuple *rhs) const {

    Field *result = FieldFactory::getOne(resultType);
   for(EqualityPredicate eq : predicates) {
       const Field *lhsField = lhs->getField(eq.first);
       const Field *rhsField = rhs->getField(eq.second);
       Field *equal = &(*(lhs->getField(eq.first)) == *(rhs->getField(eq.second)));
       Field *predResult = &(*result & *equal);

       delete equal;
       delete result;
       result = predResult;
   }

   return result;

}

 */

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
        SecureBoolField match(FieldUtilities::secureEqual(lhsField, rhsField));
        result = result & match;
    }
    return result;

}
