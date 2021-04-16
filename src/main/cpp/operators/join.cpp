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
QuerySchema
Join<B>::concatenateSchemas(const QuerySchema &lhs_schema, const QuerySchema &rhs_schema, const bool &append_bool) {
    uint32_t output_cols = lhs_schema.getFieldCount() + rhs_schema.getFieldCount() + append_bool;
    QuerySchema result(output_cols);
    uint32_t cursor = lhs_schema.getFieldCount();

    for(uint32_t i = 0; i < lhs_schema.getFieldCount(); ++i) {
        QueryFieldDesc src_field = lhs_schema.getField(i);
        QueryFieldDesc dst_field(src_field, i);

        size_t srcStringLength = src_field.getStringLength();
        dst_field.setStringLength(srcStringLength);
        result.putField(dst_field);
   }


    for(uint32_t i = 0; i < rhs_schema.getFieldCount(); ++i) {
        QueryFieldDesc src_field = rhs_schema.getField(i);
        QueryFieldDesc dst_field(src_field, cursor);

        size_t srcStringLength = src_field.getStringLength();
        dst_field.setStringLength(srcStringLength);
        result.putField(dst_field);
        ++cursor;
    }

    // bool denotes whether table comes from primary key side (0) or foreign key side (1)
    if(append_bool) {
        FieldType field_type = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
        QueryFieldDesc bool_field(output_cols-1, "table_id", "", field_type, 0);
        result.putField(bool_field);
    }

    return result;
}

// compare two tuples and return the dummy tag for this entry
template<typename B>
B Join<B>::get_dummy_tag(const QueryTuple<B> &lhs, const QueryTuple<B> &rhs, const B &predicateEval) {
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
        size_t write_size = src_tuple.getSchema()->size()/8 - sizeof(bool); // don't overwrite dummy tag
        size_t dst_byte_cnt = dst_tuple.getSchema()->size()/8;
        size_t write_offset = dst_byte_cnt - write_size - 1;

        memcpy(dst_tuple.getData() + write_offset, src_tuple.getData(), write_size);
        dst_tuple.setDummyTag(false);
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

    emp::Bit dummy_tag = emp::If(write, emp::Bit(false), dst_tuple.getDummyTag());
    dst_tuple.setDummyTag(dummy_tag);

}


template class vaultdb::Join<bool>;
template class vaultdb::Join<emp::Bit>;