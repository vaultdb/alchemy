#include <util/type_utilities.h>
#include "join.h"
#include <query_table/field/field_factory.h>

using namespace vaultdb;

template<typename  B>
Join<B>::Join(Operator<B> *lhs, Operator<B> *rhs,   Expression<B> *predicate, const SortDefinition & sort) : Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = lhs->getOutputSchema();
    QuerySchema rhs_schema = rhs->getOutputSchema();
    this->output_schema_ = QuerySchema::concatenate(lhs_schema, rhs_schema);
}

template<typename  B>
Join<B>::Join(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort) :  Operator<B>(lhs, rhs, sort), predicate_(predicate) {
    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    this->output_schema_ = QuerySchema::concatenate(lhs_schema, rhs_schema);

}



template<typename B>
string Join<B>::getParameters() const {

    if(predicate_->exprClass() == ExpressionClass::GENERIC) {
        return ((GenericExpression<B> *) predicate_)->root_->toString();
    }
    return predicate_->toString();

}


template class vaultdb::Join<bool>;
template class vaultdb::Join<emp::Bit>;