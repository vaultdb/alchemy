#include "expression_node.h"
#include "expression_factory.h"
#include <expression/visitor/print_expression_visitor.h>
#include <query_table/plain_tuple.h>  // leave tuple includes in or else it will not resolve field lookups in release mode
#include <query_table/secure_tuple.h>


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

