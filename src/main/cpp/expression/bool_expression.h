#ifndef _BOOL_EXPRESSION_H
#define _BOOL_EXPRESSION_H
#include "expression.h"
#include "expression_node.h"

namespace  vaultdb {

    template<typename B> class ExpressionNode;

    template<typename B>
    // wrapper for ExpressionNode
    class BoolExpression : public Expression<B> {
    public:
        BoolExpression(std::shared_ptr<ExpressionNode<B> > root);

        Field<B> call(const QueryTuple<B> &aTuple) const override;

        Field<B> call(const QueryTable<B>  *src, const int & row) const override {
            return root_->call(src, row);
        }
        B callBoolExpression(const QueryTuple<B> &tuple) const;

        B callBoolExpression(const QueryTable<B>  *src, const int & row) const {
            Field<B> f = root_->call(src, row);
            return f.template getValue<B>();
        }


        ExpressionKind kind() const override { return root_->kind(); }


        ~BoolExpression() = default;

        string toString() const override;

        std::shared_ptr<ExpressionNode<B> > root_;
        FieldType type_;
    };

    std::ostream &operator<<(std::ostream &os,  BoolExpression<bool> &expression);
    std::ostream &operator<<(std::ostream &os,  BoolExpression<emp::Bit> &expression);


}

#endif //_BOOL_EXPRESSION_H
