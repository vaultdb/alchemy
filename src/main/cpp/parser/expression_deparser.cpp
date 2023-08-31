#include "expression_deparser.h"
#include <emp-tool/circuits/bit.h>
#include <expression/expression_node.h>
#include <expression/math_expression_nodes.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/connector_expression_nodes.h>

using namespace vaultdb;

template<typename B>
void ExpressionDeparser<B>::visit(InputReference<B> & node) {
    ptree input_node;

    input_node.put("input", node.output_idx_); // int
    input_node.put("name",  node.output_schema_.getName());
    input_node.put("binary_mode", node.binary_mode_); // bool
    input_node.put("read_lhs", node.read_lhs_); // bool

    last_expr_ = input_node;

}

template<typename B>
void ExpressionDeparser<B>::visit(PackedInputReference<B> & node) {
    ptree input_node;

    input_node.put("input", node.output_idx_); // int
    input_node.put("name",  node.output_schema_.getName());
    input_node.put("binary_mode", node.binary_mode_); // bool
    input_node.put("read_lhs", node.read_lhs_); // bool

    last_expr_ = input_node;

}


template<typename B>
void ExpressionDeparser<B>::visit(LiteralNode<B>&  node) {
    ptree literal_node;

    literal_node.put("literal", node.payload_.toString());
    ptree subtree;
    subtree.put("type", TypeUtilities::getJSONTypeString(node.payload_.getType()));
    subtree.put("nullable", false);
    literal_node.add_child("type", subtree);

    last_expr_ = literal_node;
}

template<typename B>
void ExpressionDeparser<B>::visit(NoOp<B>&  node) {

}

template<typename B>
void ExpressionDeparser<B>::visit(AndNode<B> & node) {
    visitBinaryExpression(node, "AND");
}

template<typename B>
void ExpressionDeparser<B>::visit(OrNode<B> & node) {
    visitBinaryExpression(node, "OR");
}

template<typename B>
void ExpressionDeparser<B>::visit(NotNode<B> & node) {
    node.lhs_->accept(this);

    ptree not_node;
    not_node.put("name", "NOT");
    not_node.put("kind", "NOT");
    not_node.put("syntax", "UNARY");
    not_node.add_child("", last_expr_);

    last_expr_ = not_node;
}


template<typename B>
void ExpressionDeparser<B>::visit(PlusNode<B> & node) {
    visitBinaryExpression(node, "PLUS");
}

template<typename B>
void ExpressionDeparser<B>::visit(MinusNode<B> & node) {
    visitBinaryExpression(node, "MINUS");
}

template<typename B>
void ExpressionDeparser<B>::visit(TimesNode<B> & node) {
    visitBinaryExpression(node, "TIMES");

}

template<typename B>
void ExpressionDeparser<B>::visit(DivideNode<B> & node) {
    visitBinaryExpression(node, "DIVIDE");
}

template<typename B>
void ExpressionDeparser<B>::visit(ModulusNode<B>&  node) {
    visitBinaryExpression(node, "MODULUS");
}

template<typename B>
void ExpressionDeparser<B>::visit(EqualNode<B> & node) {
    visitBinaryExpression(node, "EQUALS");
}

template<typename B>
void ExpressionDeparser<B>::visit(NotEqualNode<B> & node) {
    visitBinaryExpression(node, "NOT_EQUALS");
}

template<typename B>
void ExpressionDeparser<B>::visit(GreaterThanNode<B> & node) {
    visitBinaryExpression(node, "GREATER_THAN");
}

template<typename B>
void ExpressionDeparser<B>::visit(LessThanNode<B> & node) {
    visitBinaryExpression(node, "LESS_THAN");

}

template<typename B>
void ExpressionDeparser<B>::visit(GreaterThanEqNode<B> & node) {
    visitBinaryExpression(node, "GREATER_THAN_OR_EQUAL");

}

template<typename B>
void ExpressionDeparser<B>::visit(LessThanEqNode<B> & node) {
    visitBinaryExpression(node, "LESS_THAN_OR_EQUAL");

}


template<typename B>
void ExpressionDeparser<B>::visitBinaryExpression(ExpressionNode<B> &binary_node, const std::string &connector) {

    ptree binary_node_ptree;

    ptree operands;
    binary_node.lhs_->accept(this);
    operands.push_back(std::make_pair("", last_expr_));

    binary_node.rhs_->accept(this);
    operands.push_back(std::make_pair("", last_expr_));

    binary_node_ptree.put("name", connector);
    binary_node_ptree.put("kind", connector);
    binary_node_ptree.put("syntax", "BINARY");
    ptree op;
    op.add_child("op", binary_node_ptree);


    op.add_child("operands", operands);

    last_expr_ = op;

}

template<typename B>
void ExpressionDeparser<B>::visit(CastNode<B> & cast_node) {
    // no-op, this is handled automatically for the limited use case of matching long <--> int and vice versa
}


template<typename B>
void ExpressionDeparser<B>::visit(CaseNode<B> &  case_node) {
    case_node.conditional_.root_->accept(this);
    ptree conditional = last_expr_;

    case_node.lhs_->accept(this);
    ptree lhs = last_expr_;

    case_node.rhs_->accept(this);
    ptree rhs = last_expr_;

    ptree case_node_ptree;
    case_node_ptree.put("name", "CASE");
    case_node_ptree.put("kind", "CASE");
    case_node_ptree.put("syntax", "SPECIAL");

    ptree operands;
    ptree operands_list;
    operands_list.push_back(std::make_pair("", conditional));
    operands_list.push_back(std::make_pair("", lhs));
    operands_list.push_back(std::make_pair("", rhs));

    case_node_ptree.add_child("operands", operands_list);
    case_node_ptree.add_child("", operands);

    last_expr_ = case_node_ptree;





}



template class vaultdb::ExpressionDeparser<bool>;
template class vaultdb::ExpressionDeparser<emp::Bit>;
