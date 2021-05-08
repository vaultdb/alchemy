#include "bool_expression.h"
#include <util/field_utilities.h>
#include <operators/join.h>

using namespace vaultdb;

std::ostream &vaultdb::operator<<(std::ostream &os, BoolExpression<bool> &expression) {
    os << *(expression.root_);
    return os;
}

std::ostream &vaultdb::operator<<(std::ostream &os, BoolExpression<emp::Bit> &expression) {
    os << *(expression.root_);
    return os;
}

template<typename B>
Field<B> BoolExpression<B>::call(const QueryTuple<B> &aTuple) const {
    return root_->call(aTuple);
}

template<typename B>
B BoolExpression<B>::callBoolExpression(const QueryTuple<B> &tuple) const {
    Field<B> output = root_->call(tuple);
    return FieldUtilities::getBoolPrimitive(output);
}

template<typename B>
BoolExpression<B>::BoolExpression(std::shared_ptr<ExpressionNode<B>> root) : Expression<B>(), root_(root){
    Expression<B>::alias_ = "predicate";
    Expression<B>::type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

}

template<typename B>
B BoolExpression<B>::call(const QueryTuple<B> &lhs, const QueryTuple<B> &rhs, const QuerySchema & output_schema) const {
    QueryTuple<B> tmp(output_schema); // create a tuple with self-managed storage
    Join<B>::write_left(true, tmp, lhs);
    Join<B>::write_right(true, tmp, rhs);
    return callBoolExpression(tmp);
}

template<typename B>
string BoolExpression<B>::toString() const {
    return root_->toString() + " " +  TypeUtilities::getTypeString(Expression<B>::type_);
}


template class vaultdb::BoolExpression<bool>;
template class vaultdb::BoolExpression<emp::Bit>;


