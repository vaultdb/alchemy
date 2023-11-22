#include "to_packed_expression_visitor.h"

using namespace vaultdb;

template<typename B>
ToPackedExpressionVisitor<B>::ToPackedExpressionVisitor(ExpressionNode<B> *root) {
    root->accept(this);
}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(InputReference<B> & node) {
    last_schema_ = node.output_schema_;
    root_ = (packed_expression_) ?  new PackedInputReference<B>(node) : node.clone();

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(PackedInputReference<B> & node) {
    root_ = new PackedInputReference<B>(node);
    last_schema_ = node.output_schema_;
}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(LiteralNode<B> & node) {
    // if field min, subtract this from literal
    if(convert_literal_ && std::is_same_v<B, Bit> && (last_schema_.getType() == FieldType::SECURE_INT || last_schema_.getType() == FieldType::SECURE_LONG)) {
            emp::Integer val = node.payload_.getInt();
            val = val - Integer(val.size(), last_schema_.getFieldMin(), PUBLIC);
            val.resize(last_schema_.size() + 1); // for sign bit
            Field<B> dst(node.payload_.getType(),val);
            root_ = new LiteralNode<B>(dst);
            return;
        }
    
    root_ = node.clone();
 }
template<typename B>
void ToPackedExpressionVisitor<B>::visit(NoOp<B> & node) { root_ = node.clone(); }


template<typename B>
void ToPackedExpressionVisitor<B>::visit(AndNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B> * lhs = root_;

    node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new AndNode<B>(lhs, rhs);


}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(OrNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *lhs = root_;

    node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new OrNode<B>(lhs, rhs);

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(NotNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *in = root_;

    root_ = new NotNode<B>(in);


}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(PlusNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *lhs = root_;

    node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new PlusNode<B>(lhs, rhs);

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(MinusNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *lhs = root_;

    node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new MinusNode<B>(lhs, rhs);


}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(TimesNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *lhs = root_;

    node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new TimesNode<B>(lhs, rhs);

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(DivideNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *lhs = root_;

    node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new DivideNode<B>(lhs, rhs);


}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(ModulusNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *lhs = root_;

    node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new ModulusNode<B>(lhs, rhs);


}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(EqualNode<B> & node) {
    pair<ExpressionNode<B> *, ExpressionNode<B> *> children = visitBinaryExpression(&node);
    root_ = new EqualNode<B>(children.first, children.second);
    delete children.first;
    delete children.second;

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(NotEqualNode<B> & node) {
    pair<ExpressionNode<B> *, ExpressionNode<B> *> children = visitBinaryExpression(&node);
    root_ = new NotEqualNode<B>(children.first, children.second);
    delete children.first;
    delete children.second;

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(GreaterThanNode<B> & node) {
    pair<ExpressionNode<B> *, ExpressionNode<B> *> children = visitBinaryExpression(&node);
    root_ = new GreaterThanNode<B>(children.first, children.second);
    delete children.first;
    delete children.second;

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(LessThanNode<B> & node) {
    pair<ExpressionNode<B> *, ExpressionNode<B> *> children = visitBinaryExpression(&node);
    root_ = new LessThanNode<B>(children.first, children.second);
    delete children.first;
    delete children.second;

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(GreaterThanEqNode<B> & node) {
    pair<ExpressionNode<B> *, ExpressionNode<B> *> children = visitBinaryExpression(&node);
    root_ = new GreaterThanEqNode<B>(children.first, children.second);
    delete children.first;
    delete children.second;

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(LessThanEqNode<B> & node) {
    pair<ExpressionNode<B> *, ExpressionNode<B> *> children = visitBinaryExpression(&node);
    root_ = new LessThanEqNode<B>(children.first, children.second);
    delete children.first;
    delete children.second;

}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(CastNode<B> & node) {
    node.lhs_->accept(this);
    ExpressionNode<B>  *input = root_;
    root_ =  new CastNode<B>(input, node.dst_type_);
}

template<typename B>
void ToPackedExpressionVisitor<B>::visit(CaseNode<B> & case_node) {
    case_node.conditional_.root_->accept(this);

    ExpressionNode<B>  *conditional_node = root_;

    GenericExpression<B> conditional(conditional_node, case_node.conditional_.getAlias(), case_node.conditional_.getType());

    case_node.lhs_->accept(this);
    ExpressionNode<B>  *lhs = root_;

    case_node.rhs_->accept(this);
    ExpressionNode<B>  *rhs = root_;

    root_ = new CaseNode<B>(conditional, lhs, rhs);

}


template class vaultdb::ToPackedExpressionVisitor<bool>;
template class vaultdb::ToPackedExpressionVisitor<emp::Bit>;
