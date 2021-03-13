#include <util/type_utilities.h>
#include "join.h"
#include <query_table/field/field_factory.h>


Join::Join(Operator *lhs, Operator *rhs, shared_ptr<BinaryPredicate> predicateClass) : Operator(lhs, rhs) {
        predicate = predicateClass;
}

Join::Join(shared_ptr<QueryTable> lhs, shared_ptr<QueryTable> rhs, shared_ptr<BinaryPredicate> &predicateClass) :  Operator(lhs, rhs) {
    predicate = predicateClass;
}


QuerySchema Join::concatenateSchemas(const QuerySchema &lhsSchema, const QuerySchema &rhsSchema) {
    uint32_t outputColCount = lhsSchema.getFieldCount() + rhsSchema.getFieldCount();
    QuerySchema result(outputColCount);
    uint32_t cursor = lhsSchema.getFieldCount();

    for(uint32_t i = 0; i < lhsSchema.getFieldCount(); ++i) {
        FieldDesc srcField = lhsSchema.getField(i);
        FieldDesc dstField(srcField, i);
        size_t srcStringLength = srcField.getStringLength();
        dstField.setStringLength(srcStringLength);
        result.putField(dstField);
   }


    for(uint32_t i = 0; i < rhsSchema.getFieldCount(); ++i) {
        FieldDesc srcField = rhsSchema.getField(i);
        FieldDesc dstField(srcField, cursor);
        size_t srcStringLength = srcField.getStringLength();
        dstField.setStringLength(srcStringLength);
        result.putField(dstField);
        ++cursor;
    }

    return result;
}

// TODO: make this faster by heap-allocating everything once
QueryTuple Join::concatenateTuples( QueryTuple *lhs,  QueryTuple *rhs) {
    const uint32_t outputFieldCount = lhs->getFieldCount() + rhs->getFieldCount();
    QueryTuple result(outputFieldCount);
    uint32_t lhsFieldCount  = lhs->getFieldCount();
    uint32_t cursor = lhsFieldCount;

    for(uint32_t i = 0; i < lhsFieldCount; ++i) {
        result.putField(lhs->getField(i), -1);
    }


    for(uint32_t i = 0; i < rhs->getFieldCount(); ++i) {

        Field *dstField = FieldFactory::copyField(rhs->getField(i));
        result.putField(dstField, cursor);
    }

    return result;
}

// compare two tuples and return their entry in the output table
QueryTuple Join::compareTuples(QueryTuple *lhs, QueryTuple *rhs, const Field * predicateEval) {
    QueryTuple dstTuple = concatenateTuples(lhs, rhs);

   Field *dummyTag = &(*(lhs->getDummyTag()) | *(rhs->getDummyTag()) | (!*predicateEval));

    dstTuple.setDummyTag(dummyTag);
    return dstTuple;
}



