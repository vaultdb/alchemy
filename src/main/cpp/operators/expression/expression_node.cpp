#include "expression_node.h"


using namespace vaultdb;

template<typename B>
ExpressionNode<B>::ExpressionNode(std::shared_ptr<ExpressionNode<B>> lhs) : lhs_(lhs) {}

template<typename B>
ExpressionNode<B>::ExpressionNode(std::shared_ptr<ExpressionNode<B>> lhs, std::shared_ptr<ExpressionNode<B>> rhs) : lhs_(lhs), rhs_(rhs) {}


template<typename B>
InputReferenceNode<B>::InputReferenceNode(const uint32_t &read_idx) : ExpressionNode<B>(nullptr), read_idx_(read_idx) {}

template<typename B>
Field<B> InputReferenceNode<B>::call(const QueryTuple<B> &target) const {
    return target.getField(read_idx_);
}

template<typename B>
ExpressionKind InputReferenceNode<B>::kind() const {
    return ExpressionKind::INPUT_REF;
}


template<typename B>
LiteralNode<B>::LiteralNode(const Field<B> &literal) : ExpressionNode<B>(nullptr), payload_(literal) {

}

template<typename B>
Field<B> LiteralNode<B>::call(const QueryTuple<B> &target) const {
    return payload_;
}

template<typename B>
ExpressionKind LiteralNode<B>::kind() const {
    return ExpressionKind::LITERAL;
}


template class vaultdb::ExpressionNode<bool>;
template class vaultdb::ExpressionNode<emp::Bit>;

template class vaultdb::InputReferenceNode<bool>;
template class vaultdb::InputReferenceNode<emp::Bit>;

template class vaultdb::LiteralNode<bool>;
template class vaultdb::LiteralNode<emp::Bit>;

