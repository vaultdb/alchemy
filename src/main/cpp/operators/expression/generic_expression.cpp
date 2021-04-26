#include "generic_expression.h"

using namespace vaultdb;

template<typename B>
Field<B> GenericExpression<B>::call(const QueryTuple<B> &aTuple) const {
    return root_->call(aTuple);
}

template<typename B>
GenericExpression<B>::GenericExpression(std::shared_ptr<ExpressionNode<B>> root, const std::string & alias, const FieldType & output_type)
: Expression<B>(alias, output_type), root_(root) {}


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

template class vaultdb::GenericExpression<bool>;
template class vaultdb::GenericExpression<emp::Bit>;


template class vaultdb::BoolExpression<bool>;
template class vaultdb::BoolExpression<emp::Bit>;
