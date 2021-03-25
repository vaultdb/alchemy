#include <util/type_utilities.h>
#include "join.h"
#include <query_table/field/field_factory.h>

template<typename  B>
Join<B>::Join(Operator<B> *lhs, Operator<B> *rhs, shared_ptr<BinaryPredicate<B> > predicateClass) : Operator<B>(lhs, rhs) {
        predicate = predicateClass;
}

template<typename  B>
Join<B>::Join(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs, shared_ptr<BinaryPredicate<B> > &predicateClass) :  Operator<B>(lhs, rhs) {
    predicate = predicateClass;
}

template<typename  B>
QuerySchema Join<B>::concatenateSchemas(const QuerySchema &lhsSchema, const QuerySchema &rhsSchema) {
    uint32_t outputColCount = lhsSchema.getFieldCount() + rhsSchema.getFieldCount();
    QuerySchema result(outputColCount);
    uint32_t cursor = lhsSchema.getFieldCount();

    for(uint32_t i = 0; i < lhsSchema.getFieldCount(); ++i) {
        QueryFieldDesc srcField = lhsSchema.getField(i);
        QueryFieldDesc dstField(srcField, i);

        size_t srcStringLength = srcField.getStringLength();
        dstField.setStringLength(srcStringLength);
        result.putField(dstField);
   }


    for(uint32_t i = 0; i < rhsSchema.getFieldCount(); ++i) {
        QueryFieldDesc srcField = rhsSchema.getField(i);
        QueryFieldDesc dstField(srcField, cursor);

        size_t srcStringLength = srcField.getStringLength();
        dstField.setStringLength(srcStringLength);
        result.putField(dstField);
        ++cursor;
    }

    return result;
}

// TODO: make this faster by heap-allocating everything once
template <typename B>
QueryTuple<B> Join<B>::concatenateTuples( QueryTuple<B> *lhs,  QueryTuple<B> *rhs) {
    const uint32_t outputFieldCount = lhs->getFieldCount() + rhs->getFieldCount();
    QueryTuple<B> result(outputFieldCount);
    uint32_t lhsFieldCount  = lhs->getFieldCount();
    uint32_t cursor = 0;

    for(; cursor < lhsFieldCount; ++cursor) {
        result.putField(cursor, *(lhs->getField(cursor)));
    }


    for(uint32_t i = 0; i < rhs->getFieldCount(); ++i) {
        result.putField(cursor, *(rhs->getField(i)));
        ++cursor;
    }

    return result;
}

// compare two tuples and return their entry in the output table
template<typename B>
QueryTuple<B> Join<B>::compareTuples(QueryTuple<B> *lhs, QueryTuple<B> *rhs, const B &predicateEval) {
    QueryTuple dstTuple = concatenateTuples(lhs, rhs);
    B lhsDummyTag = static_cast<const B &> (*(lhs->getDummyTag()));
    B rhsDummyTag = static_cast<const B &> (*(rhs->getDummyTag()));

    B dummyTag = (!predicateEval) | lhsDummyTag | rhsDummyTag;
    dstTuple.setDummyTag(dummyTag);
    return dstTuple;
}



template class vaultdb::Join<BoolField>;
template class vaultdb::Join<SecureBoolField>;