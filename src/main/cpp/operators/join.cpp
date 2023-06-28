#include <util/type_utilities.h>
#include "join.h"
#include <query_table/field/field_factory.h>

using namespace vaultdb;

template<typename  B>
Join<B>::Join(Operator<B> *lhs, Operator<B> *rhs,   Expression<B> *predicate, const SortDefinition & sort) : Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = lhs->getOutputSchema();
    QuerySchema rhs_schema = rhs->getOutputSchema();
    Operator<B>::output_schema_ = QuerySchema::concatenate(lhs_schema, rhs_schema);
}

template<typename  B>
Join<B>::Join(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort) :  Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    Operator<B>::output_schema_ = QuerySchema::concatenate(lhs_schema, rhs_schema);

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