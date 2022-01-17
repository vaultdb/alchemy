#include <emp-tool/circuits/bit.h>
#include "print_expression_visitor.h"
#include <expression/expression_node.h>
#include <expression/math_expression_nodes.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/connector_expression_nodes.h>

using namespace vaultdb;

template<typename B>
void PrintExpressionVisitor<B>::visit(InputReferenceNode<B> node) {
    last_value_ = "$" + std::to_string(node.read_idx_);
}

template<typename B>
void PrintExpressionVisitor<B>::visit(LiteralNode<B> node) {
    last_value_ = node.payload_.toString();
}

template<typename B>
void PrintExpressionVisitor<B>::visit(AndNode<B> node) {
    visitBinaryExpression(node, "AND");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(OrNode<B> node) {
    visitBinaryExpression(node, "OR");

}

template<typename B>
void PrintExpressionVisitor<B>::visit(NotNode<B> node) {
    node.lhs_->accept(this);
    last_value_ = "NOT " + last_value_;
}


template<typename B>
void PrintExpressionVisitor<B>::visit(PlusNode<B> node) {
    visitBinaryExpression(node, "+");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(MinusNode<B> node) {
    visitBinaryExpression(node, "-");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(TimesNode<B> node) {
    visitBinaryExpression(node, "*");

}

template<typename B>
void PrintExpressionVisitor<B>::visit(DivideNode<B> node) {
    visitBinaryExpression(node, "/");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(ModulusNode<B> node) {
    visitBinaryExpression(node, "%");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(EqualNode<B> node) {
    visitBinaryExpression(node, "==");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(NotEqualNode<B> node) {
    visitBinaryExpression(node, "!=");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(GreaterThanNode<B> node) {
    visitBinaryExpression(node, ">");
}

template<typename B>
void PrintExpressionVisitor<B>::visit(LessThanNode<B> node) {
    visitBinaryExpression(node, "<");

}

template<typename B>
void PrintExpressionVisitor<B>::visit(GreaterThanEqNode<B> node) {
    visitBinaryExpression(node, ">=");

}

template<typename B>
void PrintExpressionVisitor<B>::visit(LessThanEqNode<B> node) {
    visitBinaryExpression(node, "<=");

}

template<typename B>
std::string PrintExpressionVisitor<B>::getString() const {
    return last_value_;
}


template<typename B>
void PrintExpressionVisitor<B>::visitBinaryExpression(ExpressionNode<B> &binary_node, const std::string &connector) {
    binary_node.lhs_->accept(this);
    std::string lhs_str = last_value_;

    binary_node.rhs_->accept(this);
    std::string rhs_str = last_value_;

    last_value_ = lhs_str + " " + connector + " " + rhs_str;

}

template<typename B>
void PrintExpressionVisitor<B>::visit(CastNode<B> node) {
    node.accept(this);

    last_value_ = "CAST(" + last_value_ + ", " + TypeUtilities::getTypeString(node.dst_type_) + ")";
}

template class vaultdb::PrintExpressionVisitor<bool>;
template class vaultdb::PrintExpressionVisitor<emp::Bit>;
