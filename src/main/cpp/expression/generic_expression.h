#ifndef _GENERIC_EXPRESSION_H
#define _GENERIC_EXPRESSION_H

#include <expression/expression.h>
#include <expression/expression_node.h>

namespace  vaultdb {

    // wrapper for ExpressionNode
    template<typename B>
    class GenericExpression : public Expression<B> {
    public:
        GenericExpression(std::shared_ptr<ExpressionNode<B> > root);
        GenericExpression(std::shared_ptr<ExpressionNode<B> > root, const std::string & alias, const FieldType & output_type);
        Field<B> call(const QueryTuple<B> & aTuple) const override;
        ExpressionKind kind() const override { return root_->kind(); }

        ~GenericExpression() = default;

        string toString() const override;

        std::shared_ptr<ExpressionNode<B> > root_;
    };


}


#endif //_GENERIC_EXPRESSION_H
