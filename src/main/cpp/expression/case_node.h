#ifndef _CASE_NODE_H
#define _CASE_NODE_H

#include <expression/expression_node.h>
#include "generic_expression.h"

namespace  vaultdb {

    template<typename B>
    class CaseNode :  public ExpressionNode<B>  {
    public:
        // only GenericExpression supported for now
        CaseNode(const GenericExpression<B> & conditional, ExpressionNode<B> *  lhs, ExpressionNode<B> *  rhs) :
                ExpressionNode<B>(lhs, rhs), conditional_(conditional){


        }

        ~CaseNode() = default;

        Field<B> call(const QueryTuple<B> & target) const override {
            B result = conditional_.call(target).template getValue<B>();
            Field<B> lhs_eval = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs_eval = ExpressionNode<B>::rhs_->call(target);

            return Field<B>::If(result, lhs_eval, rhs_eval);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            B result = conditional_.call(src, row).template getValue<B>();

            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>::If(result, lhs, rhs);


        }

        inline Field<B> call(const QueryTable <B> *lhs, const int &lhs_row, const QueryTable <B> *rhs, const int &rhs_row) const override{
            B result = conditional_.root_->call(lhs, lhs_row, rhs, rhs_row).template getValue<B>();

            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>::If(result, l, r);

        }

       inline void accept(ExpressionVisitor<B> *visitor) override {   visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::CASE; }

        ExpressionNode<B> *clone() const override {

            return new CaseNode<B>(conditional_, ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
        }

        GenericExpression<B> conditional_;
    };


}

template class vaultdb::CaseNode<bool>;
template class vaultdb::CaseNode<emp::Bit>;


#endif //_CASE_NODE_H
