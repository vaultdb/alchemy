#include "bool_expression.h"
#include "expression_factory.h"

using namespace vaultdb;

template<typename B>
BoolExpression<B>::BoolExpression(ExpressionNode<B> *root) : Expression<B>() {
    Expression<B>::alias_ = "predicate";
    Expression<B>::type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
    root_ = root->clone();
}





template<typename B>
BoolExpression<B>::BoolExpression(const BoolExpression<B> &src)  : Expression<B>() {
    Expression<B>::alias_ = src.alias_;
    Expression<B>::type_ = src.type_;

    root_ = src.root_->clone();

}


template class vaultdb::BoolExpression<bool>;
template class vaultdb::BoolExpression<emp::Bit>;


