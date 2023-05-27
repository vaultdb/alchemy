#include <util/type_utilities.h>
#include "join.h"
#include <query_table/field/field_factory.h>

using namespace vaultdb;

template<typename  B>
Join<B>::Join(Operator<B> *lhs, Operator<B> *rhs,  const BoolExpression<B> & predicate, const SortDefinition & sort) : Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = lhs->getOutputSchema();
    QuerySchema rhs_schema = rhs->getOutputSchema();
    Operator<B>::output_schema_ = concatenateSchemas(lhs_schema, rhs_schema);

}

template<typename  B>
Join<B>::Join(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs,  const BoolExpression<B> & predicate, const SortDefinition & sort) :  Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = *lhs->getSchema();
    QuerySchema rhs_schema = *rhs->getSchema();
    Operator<B>::output_schema_ = concatenateSchemas(lhs_schema, rhs_schema);

}

template<typename  B>
QuerySchema
Join<B>::concatenateSchemas(const QuerySchema &lhs_schema, const QuerySchema &rhs_schema, const bool &append_bool) {
    uint32_t output_cols = lhs_schema.getFieldCount() + rhs_schema.getFieldCount() + append_bool;
    QuerySchema result;
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
    result.initializeFieldOffsets();
    return result;
}


// copy src_tuple to lhs of dst for its half of the join
template<typename B>
void Join<B>::write_left(const bool &write, PlainTuple &dst_tuple, const PlainTuple &src_tuple) {
    if(write) {
        size_t tuple_size = src_tuple.getSchema()->size()/8 - 1; // -1 for dummy tag
        memcpy(dst_tuple.getData(), src_tuple.getData(), tuple_size);
    }
}


template<typename B>
void Join<B>::write_left(const emp::Bit &write, SecureTuple &dst_tuple, const SecureTuple &src_tuple) {
    size_t write_bit_cnt = src_tuple.getSchema()->size() - 1; // -1 for dummy tag

    emp::Bit *write_ptr = dst_tuple.getData();
    emp::Bit *read_ptr = src_tuple.getData();
    for(size_t i = 0; i < write_bit_cnt; ++i) {
        *write_ptr = emp::If(write, *read_ptr, *write_ptr);
        ++read_ptr;
        ++write_ptr;
    }

}

template<typename B>
void Join<B>::write_left(PlainTuple &dst_tuple, const PlainTuple &src_tuple) {
    size_t tuple_size = src_tuple.getSchema()->size()/8 - 1; // -1 for dummy tag
    memcpy(dst_tuple.getData(), src_tuple.getData(), tuple_size);
}


template<typename B>
void Join<B>::write_left(SecureTuple &dst_tuple, const SecureTuple &src_tuple) {

    memcpy(dst_tuple.getData(), src_tuple.getData(), (src_tuple.getSchema()->size() - 1) * TypeUtilities::getEmpBitSize());
}



template<typename B>
void Join<B>::write_right(const bool &write, PlainTuple &dst_tuple, const PlainTuple &src_tuple) {
    if(write) {
        size_t write_size = src_tuple.getSchema()->size()/8 - 1; // don't overwrite dummy tag
        size_t dst_byte_cnt = dst_tuple.getSchema()->size()/8;
        size_t write_offset = dst_byte_cnt - write_size - 1;

        memcpy(dst_tuple.getData() + write_offset, src_tuple.getData(), write_size);
    }
}


template<typename B>
void Join<B>::write_right(const emp::Bit &write, SecureTuple &dst_tuple, const SecureTuple &src_tuple) {
    size_t write_bit_cnt = src_tuple.getSchema()->size() - 1; // don't overwrite dummy tag
    size_t dst_bit_cnt = dst_tuple.getSchema()->size() - 1;
    size_t write_offset = dst_bit_cnt - write_bit_cnt;

    emp::Bit *write_ptr = dst_tuple.getData() + write_offset;
    emp::Bit *read_ptr = src_tuple.getData();
    for(size_t i = 0; i < write_bit_cnt; ++i) {
        *write_ptr = emp::If(write, *read_ptr, *write_ptr);
        ++read_ptr;
        ++write_ptr;
    }

}


template<typename B>
void Join<B>::write_right(SecureTuple &dst_tuple, const SecureTuple &src_tuple) {
    Bit *dst = dst_tuple.getData() +  (dst_tuple.getSchema()->size()  - src_tuple.getSchema()->size() );
    Bit *src = src_tuple.getData();
    memcpy(dst, src, (src_tuple.getSchema()->size() - 1) * TypeUtilities::getEmpBitSize());


}

template<typename B>
void Join<B>::write_right(PlainTuple &dst_tuple, const PlainTuple &src_tuple) {
    size_t write_size = src_tuple.getSchema()->size()/8 - sizeof(bool); // don't overwrite dummy tag
    size_t dst_byte_cnt = dst_tuple.getSchema()->size()/8;
    size_t write_offset = dst_byte_cnt - write_size - 1;

    memcpy(dst_tuple.getData() + write_offset, src_tuple.getData(), write_size);

}


template<typename B>
string Join<B>::getParameters() const {
   return predicate_.root_->toString();

}



template class vaultdb::Join<bool>;
template class vaultdb::Join<emp::Bit>;