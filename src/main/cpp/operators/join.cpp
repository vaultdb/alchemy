//
// Created by Jennie Rogers on 9/13/20.
//

#include "join.h"


Join::Join(std::shared_ptr<BinaryPredicate> &predicateClass, std::shared_ptr<Operator> &lhs, std::shared_ptr<Operator> &rhs) : Operator(lhs, rhs) {
        predicate = predicateClass;
}


QuerySchema Join::concatenateSchemas(const QuerySchema &lhsSchema, const QuerySchema &rhsSchema) {
    uint32_t outputColCount = lhsSchema.getFieldCount() + rhsSchema.getFieldCount();
    QuerySchema result(outputColCount);
    uint32_t cursor = lhsSchema.getFieldCount();

    for(uint32_t i = 0; i < lhsSchema.getFieldCount(); ++i) {
        result.putField(i, lhsSchema.getField(i));
    }


    for(uint32_t i = 0; i < rhsSchema.getFieldCount(); ++i) {
        QueryFieldDesc dstField(rhsSchema.getField(i), cursor);
        result.putField(cursor, dstField);
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
        result.putField(i, lhs->getField(i));
    }


    for(uint32_t i = 0; i < rhs->getFieldCount(); ++i) {
        QueryField dstField(cursor, rhs->getField(i).getValue());
        result.putField(cursor, dstField);
        ++cursor;
    }

    return result;
}

// compare two tuples and return their entry in the output table
QueryTuple Join::compareTuples(QueryTuple *lhs, QueryTuple *rhs) {
    QueryTuple dstTuple = concatenateTuples(lhs, rhs);
    types::Value predicateEval = predicate->predicateCall(lhs, rhs);
    types::Value dummyTag = lhs->getDummyTag() | rhs->getDummyTag() | (!predicateEval);
    dstTuple.setDummyTag(dummyTag);
    return dstTuple;
}


