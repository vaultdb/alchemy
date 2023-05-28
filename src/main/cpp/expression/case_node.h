#ifndef _CASE_NODE_H
#define _CASE_NODE_H

#include <expression/expression_node.h>


namespace  vaultdb {
    template<typename B>
    class CaseNode :  public ExpressionNode<B>  {
    public:
        CaseNode(BoolExpression<B> & conditional, std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs);
        ~CaseNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            B result = conditional_.callBoolExpression(src, row);

            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>::If(result, lhs, rhs);


        }


        void accept(ExpressionVisitor<B> *visitor) override;

        ExpressionKind kind() const override;

        BoolExpression<B> conditional_;
    };

}

#endif //_CASE_NODE_H
