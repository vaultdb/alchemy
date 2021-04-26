#include "connector_expression_nodes.h"

using namespace vaultdb;

template<typename B>
NotNode<B>::NotNode(std::shared_ptr<ExpressionNode<B>> input): ExpressionNode<B>(input) {}

template<typename B>
Field<B> NotNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> input = ExpressionNode<B>::lhs_->call(target);
    return !input;
}


template<typename B>
AndNode<B>::AndNode(std::shared_ptr<ExpressionNode<B>> &lhs, std::shared_ptr<ExpressionNode<B>> &rhs) : ExpressionNode<B>(lhs, rhs){ }

template<typename B>
Field<B> AndNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
    Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

    B result = lhs && rhs;
    return FieldUtilities::getBoolField(result);
}

template<typename B>
OrNode<B>::OrNode(std::shared_ptr<ExpressionNode<B>> &lhs, std::shared_ptr<ExpressionNode<B>> &rhs) : ExpressionNode<B>(lhs, rhs) { }

template<typename B>
Field<B> OrNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
    Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

    B result = lhs || rhs;
    return FieldUtilities::getBoolField(result);

}

template class vaultdb::NotNode<bool>;
template class vaultdb::NotNode<emp::Bit>;


template class vaultdb::AndNode<bool>;
template class vaultdb::AndNode<emp::Bit>;


template class vaultdb::OrNode<bool>;
template class vaultdb::OrNode<emp::Bit>;
