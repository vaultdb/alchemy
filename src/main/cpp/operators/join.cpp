#include <util/type_utilities.h>
#include "join.h"
#include <query_table/field/field_factory.h>

using namespace vaultdb;

template<typename  B>
Join<B>::Join(Operator<B> *lhs, Operator<B> *rhs,   Expression<B> *predicate, const SortDefinition & sort) : Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = lhs->getOutputSchema();
    QuerySchema rhs_schema = rhs->getOutputSchema();
    Operator<B>::output_schema_ = concatenateSchemas(lhs_schema, rhs_schema);
}

template<typename  B>
Join<B>::Join(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort) :  Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
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


// TODO: refactor this to not require discrete tuple
template<typename B>
void Join<B>::write_left(PlainTuple & dst, const PlainTable *src, const int & idx) {

    size_t tuple_size = src->tuple_size_ - 1; // -1 for dummy tag
    PlainTuple  p = src->getPlainTuple(idx);
    memcpy(dst.getData(), p.getData(), tuple_size);

}

template<typename B>
void Join<B>::write_left(SecureTuple & dst, const SecureTable *src, const int & idx) {
    size_t write_size = src->tuple_size_ - TypeUtilities::getEmpBitSize(); // - for dummy tag
    SecureTuple s = src->getSecureTuple(idx);
    memcpy(dst.getData(), s.getData(), write_size );
}




template<typename B>
void Join<B>::write_right(SecureTuple &dst_tuple, const SecureTable *src, const int & idx) {
    int write_size = (src->getSchema().size() - 1) * TypeUtilities::getEmpBitSize(); // sans the dummy tag
    int write_offset = dst_tuple.schema_->size() - src->getSchema().size(); // bits
    Bit *dst =  dst_tuple.getData() + write_offset;

    SecureTuple s = src->getSecureTuple(idx);
    memcpy(dst, s.getData(), write_size);
}

template<typename B>
void Join<B>::write_right(PlainTuple &dst_tuple, const PlainTable *src, const int & idx) {

    size_t write_size = src->tuple_size_ - 1; // don't overwrite dummy tag
    int8_t *write_ptr = dst_tuple.getData() + (dst_tuple.getSchema()->size()/8 - src->tuple_size_);
    PlainTuple  p = src->getPlainTuple(idx);

    memcpy(write_ptr, p.getData(), write_size);

}


template<typename B>
string Join<B>::getParameters() const {

    if(predicate_->exprClass() == ExpressionClass::GENERIC) {
        return ((GenericExpression<B> *) predicate_)->root_->toString();
    }
    return predicate_->toString();

}


template<typename B>
void Join<B>::write_left(QueryTable<B> *dst, const int &dst_idx, const QueryTable<B> *src, const int &src_idx) {
    dst->cloneRow(dst_idx, 0, src, src_idx);
}


template<typename B>
void Join<B>::write_left(const B &write, QueryTable<B> *dst, const int &dst_idx, const QueryTable<B> *src,
                         const int &src_idx) {

    dst->cloneRow(write, dst_idx, 0, src, src_idx);
}


template<typename B>
void Join<B>::write_right(QueryTable<B> *dst, const int &dst_idx, const QueryTable<B> *src, const int &src_idx) {
    dst->cloneRow(dst_idx, dst->getSchema().getFieldCount() - src->getSchema().getFieldCount(),
                  src, src_idx);
}


template<typename B>
void Join<B>::write_right(const B &write, QueryTable<B> *dst, const int &dst_idx, const QueryTable<B> *src,
                         const int &src_idx) {

    dst->cloneRow(write, dst_idx, dst->getSchema().getFieldCount() - src->getSchema().getFieldCount(),
                  src, src_idx);
}




template class vaultdb::Join<bool>;
template class vaultdb::Join<emp::Bit>;