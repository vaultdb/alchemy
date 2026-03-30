#include "expression/visitor/type_validation_visitor.h"
#include <expression/expression_node.h>
#include <expression/math_expression_nodes.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/connector_expression_nodes.h>

using namespace vaultdb;



template<typename B>
TypeValidationVisitor<B>::TypeValidationVisitor(ExpressionNode<B> *root, const QuerySchema &input_schema)
        : input_schema_(input_schema), bool_type_(std::is_same_v<bool, B> ? FieldType::BOOL : FieldType::SECURE_BOOL) {
    type_rank_[FieldType::BOOL] = type_rank_[FieldType::SECURE_BOOL] = 0;
    type_rank_[FieldType::INT] = type_rank_[FieldType::SECURE_INT] = 1;
    type_rank_[FieldType::LONG] = type_rank_[FieldType::SECURE_LONG] = 2;
    type_rank_[FieldType::FLOAT] = type_rank_[FieldType::SECURE_FLOAT] = 3;


    root->accept(this);
}

template<typename B>
void TypeValidationVisitor<B>::visit(InputReference<B>  & node) {
    last_expression_type_ = node.output_schema_.getType(); // input_schema_.getField(node.read_idx_).getType();
}

template<typename B>
void TypeValidationVisitor<B>::visit(PackedInputReference<B>  & node) {
    last_expression_type_ = node.output_schema_.getType();
}

template<typename B>
void TypeValidationVisitor<B>::visit(LiteralNode<B>  & node) {
    last_expression_type_ = node.payload_.getType();
}


// B has to match up with Field<B> if/when we plug in values to the equation later
template<typename B>
void TypeValidationVisitor<B>::visit(AndNode<B>  & node) {
    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(OrNode<B>  & node) {
    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(NotNode<B>  & node) {
    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(PlusNode<B>  & node) {
    verifyBinaryNode(node);
}

template<typename B>
void TypeValidationVisitor<B>::visit(MinusNode<B>  & node) {

    verifyBinaryNode(node);
}

template<typename B>
void TypeValidationVisitor<B>::visit(TimesNode<B> & node) {
    verifyBinaryNode(node);

}

template<typename B>
void TypeValidationVisitor<B>::visit(DivideNode<B> & node) {
    verifyBinaryNode(node);
}

template<typename B>
void TypeValidationVisitor<B>::visit(ModulusNode<B> & node) {
    verifyBinaryNode(node);
}

template<typename B>
void TypeValidationVisitor<B>::visit(EqualNode<B> & node) {
    verifyBinaryNode(node);
    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(NotEqualNode<B> & node) {
    verifyBinaryNode(node);

    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(GreaterThanNode<B> & node) {
    verifyBinaryNode(node);

    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(LessThanNode<B> & node) {
    verifyBinaryNode(node);

    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(GreaterThanEqNode<B> & node) {
    verifyBinaryNode(node);

    last_expression_type_ = bool_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(LessThanEqNode<B> & node) {
    verifyBinaryNode(node);

    last_expression_type_ = bool_type_;
}

template<typename B>
FieldType TypeValidationVisitor<B>::getExpressionType() const {
    return last_expression_type_;
}


template<typename B>
void TypeValidationVisitor<B>::verifyBinaryNode(const ExpressionNode<B> &binary_node) {
    binary_node.lhs_->accept(this);
    FieldType lhs_type = last_expression_type_;

    binary_node.rhs_->accept(this);
    FieldType rhs_type = last_expression_type_;
    assert(lhs_type == rhs_type);

    last_expression_type_ = lhs_type;
}
/*
template<typename B>
FieldType TypeValidationVisitor<B>::resolveType(const FieldType &lhs, const FieldType &rhs) {
    // check for unsupported types
    assert(type_rank_.find(lhs) != type_rank_.end() &&  type_rank_.find(rhs) != type_rank_.end());

    if(type_rank_[lhs] > type_rank_[rhs])
        return lhs;
    return rhs;
}*/

template<typename B>
void TypeValidationVisitor<B>::visit(CastNode<B> & node) {
    last_expression_type_ =  node.dst_type_;
}

template<typename B>
void TypeValidationVisitor<B>::visit(CaseNode<B>& node) {
    return verifyBinaryNode(node);
}



template class vaultdb::TypeValidationVisitor<bool>;
template class vaultdb::TypeValidationVisitor<emp::Bit>;
