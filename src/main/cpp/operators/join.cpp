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
/*
template <typename B>
QueryTuple<B> Join<B>::concatenateTuples( QueryTuple<B> *lhs,  QueryTuple<B> *rhs) {
    QueryTuple<B> result(0);
    uint32_t lhsFieldCount  = lhs->getFieldCount();
    uint32_t cursor = 0;

    for(; cursor < lhsFieldCount; ++cursor) {
        result.setField(cursor, lhs->getField(cursor));
    }


    for(uint32_t i = 0; i < rhs->getFieldCount(); ++i) {
        result.setField(cursor, rhs->getField(i));
        ++cursor;
    }

    return result;
}
*/

// compare two tuples and return the dummy tag for this entry
template<typename B>
B Join<B>::compareTuples(const QueryTuple<B> &lhs, const QueryTuple<B> &rhs, const Field<B> &predicateEval) {
   // QueryTuple dstTuple = concatenateTuples(lhs, rhs);
    B lhsDummyTag = lhs.getDummyTag();
    B rhsDummyTag = rhs.getDummyTag();

    return (!predicateEval) | lhsDummyTag | rhsDummyTag;
}

// copy src_tuple to lhs of dst for its half of the join
template<typename B>
void Join<B>::write_left(const bool &write, PlainTuple &dst_tuple, const PlainTuple &src_tuple) {
    if(write) {
        size_t tuple_size = src_tuple.getSchema()->size()/8;
        memcpy(dst_tuple.getData(), src_tuple.getData(), tuple_size);
    }
}


template<typename B>
void Join<B>::write_left(const emp::Bit &write, SecureTuple &dst_tuple, const SecureTuple &src_tuple) {
    size_t write_bit_cnt = src_tuple.getSchema()->size();
    size_t write_size = write_bit_cnt * sizeof(emp::block);

    emp::Integer src(write_bit_cnt, 0), dst(write_bit_cnt, 0);

    memcpy(src.bits.data(), src_tuple.getData(), write_size);
    memcpy(dst.bits.data(), dst_tuple.getData(), write_size);

    dst = dst.select(write, src);

    memcpy(dst_tuple.getData(), dst.bits.data(), write_size);

}

template<typename B>
void Join<B>::write_right(const bool &write, PlainTuple &dst_tuple, const PlainTuple &src_tuple) {
    if(write) {
        size_t write_size = src_tuple.getSchema()->size()/8;
        size_t dst_bit_cnt = dst_tuple.getSchema()->size();
        size_t write_offset = dst_bit_cnt - write_size;


        memcpy(dst_tuple.getData() + write_offset, src_tuple.getData(), write_size);
    }
}


template<typename B>
void Join<B>::write_right(const emp::Bit &write, SecureTuple &dst_tuple, const SecureTuple &src_tuple) {
    size_t write_bit_cnt = src_tuple.getSchema()->size();
    size_t dst_bit_cnt = dst_tuple.getSchema()->size();
    size_t write_offset = dst_bit_cnt - write_bit_cnt;
    size_t write_size = write_bit_cnt * sizeof(emp::block);

    emp::Integer src(write_bit_cnt, 0), dst(write_bit_cnt, 0);

    memcpy(src.bits.data(), src_tuple.getData(), write_size);
    memcpy(dst.bits.data(), dst_tuple.getData() + write_offset, write_size);


    dst = dst.select(write, src);

    memcpy(dst_tuple.getData() + write_offset, dst.bits.data(), write_size);

}


template class vaultdb::Join<bool>;
template class vaultdb::Join<emp::Bit>;