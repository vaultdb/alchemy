#include <util/type_utilities.h>
#include "join_equality_predicate.h"
#include <query_table/field/field_factory.h>

JoinEqualityPredicate::JoinEqualityPredicate(const ConjunctiveEqualityPredicate &srcPredicates, bool isEncrypted)  : predicates(srcPredicates) {
    if(isEncrypted)
        resultType = FieldType::SECURE_BOOL;
}


Field *JoinEqualityPredicate::predicateCall( QueryTuple *lhs,  QueryTuple *rhs) const {

    Field *result = FieldFactory::getOne(resultType);
   for(EqualityPredicate eq : predicates) {
       Field *equal = &(*(lhs->getField(eq.first)) == *(rhs->getField(eq.second)));
       Field *predResult = &(*result & *equal);

       delete equal;
       delete result;
       result = predResult;
   }

   return result;

}

