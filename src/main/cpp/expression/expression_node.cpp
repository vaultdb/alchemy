#include "expression_node.h"
#include <expression/visitor/print_expression_visitor.h>
#include <query_table/plain_tuple.h>  // leave tuple includes in or else it will not resolve field lookups in release mode
#include <query_table/secure_tuple.h>


using namespace vaultdb;


template<typename B>
std::string ExpressionNode<B>::toString()  {
    PrintExpressionVisitor<B> visitor;
    this->accept(&visitor);
    return visitor.getString();

}

template<typename B>
std::ostream &vaultdb::operator<<(std::ostream &os,  ExpressionNode<B> &expression) {
    PrintExpressionVisitor<B> visitor;
    expression.accept(&visitor);

    os << visitor.getString();

    return os;
}



template class vaultdb::ExpressionNode<bool>;
template class vaultdb::ExpressionNode<emp::Bit>;

template class vaultdb::InputReferenceNode<bool>;
template class vaultdb::InputReferenceNode<emp::Bit>;

template class vaultdb::LiteralNode<bool>;
template class vaultdb::LiteralNode<emp::Bit>;

template class vaultdb::CastNode<bool>;
template class vaultdb::CastNode<emp::Bit>;

