#include <expression/expression_node.h>
#include <expression/expression_factory.h>
#include <expression/visitor/print_expression_visitor.h>
#include <query_table/plain_tuple.h>  // leave tuple includes in or else it will not resolve field lookups in release mode
#include <query_table/secure_tuple.h>
#include "util/field_utilities.h"
#include <string>

using namespace vaultdb;



template<typename B>
ExpressionNode<B>::ExpressionNode(ExpressionNode<B> *lhs) {
    if(lhs != nullptr)
        lhs_ = lhs->clone();
}


template<typename B>
ExpressionNode<B>::ExpressionNode(ExpressionNode<B> *lhs, ExpressionNode<B> *rhs) {
    assert(lhs != nullptr && rhs!= nullptr);


    lhs_ = lhs->clone();
    rhs_ = rhs->clone();

}

template<typename B>
std::string ExpressionNode<B>::toString()  {
    PrintExpressionVisitor<B> visitor;
    this->accept(&visitor);
    return visitor.getString();

}

std::ostream &vaultdb::operator<<(std::ostream &os,  ExpressionNode<bool> &expression) {
    PrintExpressionVisitor<bool> visitor;
    expression.accept(&visitor);

    os << visitor.getString();

    return os;
}

std::ostream &vaultdb::operator<<(std::ostream &os,  ExpressionNode<Bit> &expression) {
    PrintExpressionVisitor<Bit> visitor;
    expression.accept(&visitor);

    os << visitor.getString();

    return os;
}


template<typename B>
bool LiteralNode<B>::operator==(const ExpressionNode<B> &other) const {
    if(other.kind() != ExpressionKind::LITERAL) {
        return false;
    }
    const LiteralNode<B> &other_ref = static_cast<const LiteralNode<B> &>(other);
    return FieldUtilities::extract_bool(other_ref.payload_ == payload_);
}


template<typename B>
bool NoOp<B>::operator==(const ExpressionNode<B> &other) const {
    if(other.kind() != ExpressionKind::NO_OP) {
        return false;
    }
    const NoOp<B> &other_ref = static_cast<const NoOp<B> &>(other);
    return FieldUtilities::extract_bool(other_ref.output_ == output_);
}

template class vaultdb::ExpressionNode<bool>;
template class vaultdb::ExpressionNode<emp::Bit>;

template class vaultdb::InputReference<bool>;
template class vaultdb::InputReference<emp::Bit>;

template class vaultdb::LiteralNode<bool>;
template class vaultdb::LiteralNode<emp::Bit>;

template class vaultdb::NoOp<bool>;
template class vaultdb::NoOp<emp::Bit>;

template class vaultdb::CastNode<bool>;
template class vaultdb::CastNode<emp::Bit>;

