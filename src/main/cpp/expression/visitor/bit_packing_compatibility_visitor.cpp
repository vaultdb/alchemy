#include "bit_packing_compatibility_visitor.h"
#include <expression/expression_node.h>
#include <expression/math_expression_nodes.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/connector_expression_nodes.h>

using namespace vaultdb;

template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(AndNode<B> &node) {
    node.lhs_->accept(this); // just recurse, does not impact compatibility of children
    node.rhs_->accept(this);
}


template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(OrNode<B> &node) {
    node.lhs_->accept(this);
    node.rhs_->accept(this);
}

template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(NotNode<B> &node) {
    node.lhs_->accept(this);
}



template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(EqualNode<B> &node) {
    checkBinaryNode(node);
}


template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(NotEqualNode<B> &node) {
    checkBinaryNode(node);
}


template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(GreaterThanNode<B> &node) {
    checkBinaryNode(node);
}

template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(LessThanNode<B> &node) {
    checkBinaryNode(node);
}


template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(GreaterThanEqNode<B> &node) {
    checkBinaryNode(node);
}


template<typename B>
void BitPackingCompatibilityVisitor<B>::visit(LessThanEqNode<B> &node) {
    checkBinaryNode(node);
}




template class vaultdb::BitPackingCompatibilityVisitor<bool>;
template class vaultdb::BitPackingCompatibilityVisitor<emp::Bit>;
