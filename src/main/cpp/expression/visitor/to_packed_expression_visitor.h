#ifndef _TO_PACKED_EXPRESSION_VISITOR_
#define _TO_PACKED_EXPRESSION_VISITOR_

#include "expression_visitor.h"
#include <expression/connector_expression_nodes.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/math_expression_nodes.h>
#include <expression/case_node.h>


namespace  vaultdb {

    template<typename B>
    class ToPackedExpressionVisitor : public ExpressionVisitor<B> {

    private:
        ExpressionNode<B>  *root_; // needs to be deleted by calling class
        QueryFieldDesc last_schema_;
        bool convert_literal_ = false;
        bool packed_expression_ = false;

    public:
        ToPackedExpressionVisitor(ExpressionNode<B>  *root);


       ExpressionNode<B> * getRoot() {
            return root_;
        }

        void visit(InputReference<B> & node) override;
        void visit(PackedInputReference<B> & node) override;

        void visit(LiteralNode<B> & node) override;

        void visit(NoOp<B> & node) override;

        void visit(AndNode<B> & node) override;

        void visit(OrNode<B> & node) override;

        void visit(NotNode<B> & node) override;

        void visit(PlusNode<B> & node) override;

        void visit(MinusNode<B> & node) override;

        void visit(TimesNode<B> & node) override;

        void visit(DivideNode<B> & node) override;

        void visit(ModulusNode<B> & node) override;

        void visit(EqualNode<B> & node) override;

        void visit(NotEqualNode<B> & node) override;

        void visit(GreaterThanNode<B> & node) override;

        void visit(LessThanNode<B> & node) override;

        void visit(GreaterThanEqNode<B> & node) override;

        void visit(LessThanEqNode<B> & node) override;

        void visit(CastNode<B> & node) override;

        void visit(CaseNode<B> & node) override;

    private:
        // expressions are of the form (<field> <op> <literal>) or (<field> <op> <field>)
        // anything more complicated will change domain/range, so don't bother
        pair<ExpressionNode<B> *, ExpressionNode<B> * > visitBinaryExpression(ExpressionNode<B> *expr) {

            pair<ExpressionNode<B> *, ExpressionNode<B> * > result;
            if(expr->lhs_->kind() == ExpressionKind::LITERAL  && inputRef(expr->rhs_)) {
                // traverse rhs first to cache schema
                expr->rhs_->accept(this);
                result.second = root_;

                if(last_schema_.bitPacked()) convert_literal_ = true;
                expr->lhs_->accept(this);
                convert_literal_ = false;
                result.first = root_;
                return result;
            }
            else if(expr->rhs_->kind() == ExpressionKind::LITERAL
               && inputRef(expr->lhs_)) {
                expr->lhs_->accept(this);
                result.first = root_;

                if(last_schema_.bitPacked()) convert_literal_ = true;
                expr->rhs_->accept(this);
                convert_literal_ = false;
                result.second = root_;
                return result;
            }
            else if(inputRef(expr->lhs_) && inputRef(expr->rhs_)
               && QueryFieldDesc::storageCompatible(expr->lhs_->output_schema_, expr->rhs_->output_schema_)
               && expr->lhs_->output_schema_.bitPacked()) {
                packed_expression_ = true;
                expr->lhs_->accept(this);
                result.first = root_;
                expr->rhs_->accept(this);
                result.second = root_;
                packed_expression_ = false;
                return result;
            }
            else { // no bit packing
                expr->lhs_->accept(this);
                result.first = root_;
                expr->rhs_->accept(this);
                result.second = root_;
                return result;
            }
        }

        inline bool inputRef(const ExpressionNode<B> *expr) {
           return expr->kind() == ExpressionKind::INPUT_REF || expr->kind() == ExpressionKind::PACKED_INPUT_REF;
        }
    };


}



#endif
