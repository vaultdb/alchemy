#ifndef _CASE_NODE_H
#define _CASE_NODE_H

#include <expression/expression_node.h>


namespace  vaultdb {

    template<typename B>
    class CaseNode :  public ExpressionNode<B>  {
    public:
        CaseNode(BoolExpression<B> & conditional, std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs) :
                ExpressionNode<B>(lhs, rhs), conditional_(conditional){ }
        ~CaseNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            B result = conditional_.callBoolExpression(target);
            Field<B> lhs_eval = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs_eval = ExpressionNode<B>::rhs_->call(target);

            return Field<B>::If(result, lhs_eval, rhs_eval);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            B result = conditional_.callBoolExpression(src, row);

            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>::If(result, lhs, rhs);


        }


       inline void accept(ExpressionVisitor<B> *visitor) override {   visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::CASE; }

        BoolExpression<B> conditional_;
    };


}

template class vaultdb::CaseNode<bool>;
template class vaultdb::CaseNode<emp::Bit>;


#endif //_CASE_NODE_H
