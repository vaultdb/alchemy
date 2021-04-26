#include "math_expression_nodes.h"

using namespace vaultdb;

template<typename B>
PlusNode<B>::PlusNode(std::shared_ptr <ExpressionNode<B> > &lhs, std::shared_ptr<ExpressionNode<B> > &rhs) : ExpressionNode<B>(lhs, rhs) { }

template<typename B>
Field<B> PlusNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
    Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
    return lhs + rhs;
}

template<typename B>
ExpressionKind PlusNode<B>::kind() const {
    return ExpressionKind::PLUS;
}


template<typename B>
MinusNode<B>::MinusNode(std::shared_ptr<ExpressionNode<B>> &lhs, std::shared_ptr<ExpressionNode<B>> &rhs) : ExpressionNode<B>(lhs, rhs) { }

template<typename B>
Field<B> MinusNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
    Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
    return lhs - rhs;
}

template<typename B>
ExpressionKind MinusNode<B>::kind() const {
    return ExpressionKind::MINUS;
}


template<typename B>
TimesNode<B>::TimesNode(std::shared_ptr<ExpressionNode<B>> &lhs, std::shared_ptr<ExpressionNode<B>> &rhs) : ExpressionNode<B>(lhs, rhs) { }

template<typename B>
Field<B> TimesNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
    Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
    return lhs * rhs;
}

template<typename B>
ExpressionKind TimesNode<B>::kind() const {
    return ExpressionKind::TIMES;
}


template<typename B>
DivideNode<B>::DivideNode(std::shared_ptr<ExpressionNode<B>> &lhs, std::shared_ptr<ExpressionNode<B>> &rhs) : ExpressionNode<B>(lhs, rhs) { }

template<typename B>
Field<B> DivideNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
    Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
    return lhs / rhs;
}

template<typename B>
ExpressionKind DivideNode<B>::kind() const {
    return ExpressionKind::DIVIDE;
}


template<typename B>
ModulusNode<B>::ModulusNode(std::shared_ptr<ExpressionNode<B>> &lhs, std::shared_ptr<ExpressionNode<B>> &rhs) : ExpressionNode<B>(lhs, rhs) { }

template<typename B>
Field<B> ModulusNode<B>::call(const QueryTuple<B> &target) const {
    Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
    Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
    return lhs / rhs;
}

template<typename B>
ExpressionKind ModulusNode<B>::kind() const {
    return ExpressionKind::MOD;
}


template class vaultdb::PlusNode<bool>;
template class vaultdb::PlusNode<emp::Bit>;

template class vaultdb::MinusNode<bool>;
template class vaultdb::MinusNode<emp::Bit>;

template class vaultdb::TimesNode<bool>;
template class vaultdb::TimesNode<emp::Bit>;

template class vaultdb::DivideNode<bool>;
template class vaultdb::DivideNode<emp::Bit>;

template class vaultdb::ModulusNode<bool>;
template class vaultdb::ModulusNode<emp::Bit>;
