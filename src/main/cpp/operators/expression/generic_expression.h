#ifndef _GENERIC_EXPRESSION_H
#define _GENERIC_EXPRESSION_H

#include <operators/expression/expression.h>
#include <operators/expression/expression_node.h>
#include <util/field_utilities.h>

namespace  vaultdb {

    // wrappers for ExpressionNode
    template<typename B>
    class GenericExpression : public Expression<B> {
    public:
        GenericExpression(std::shared_ptr<ExpressionNode<B> > root, const std::string & alias, const FieldType & output_type);
        Field<B> call(const QueryTuple<B> & aTuple) const override;

        ~GenericExpression() = default;

    private:
        std::shared_ptr<ExpressionNode<B> > root_;
    };

    template<typename B>
    class BoolExpression : public Expression<B> {
    public:
        BoolExpression(std::shared_ptr<ExpressionNode<B> > root);

        Field<B> call(const QueryTuple<B> & aTuple) const override;
        B callBoolExpression(const QueryTuple<B> & tuple) const;


        ~BoolExpression() = default;

    private:
        std::shared_ptr<ExpressionNode<B> > root_;
    };


}


#endif //_GENERIC_EXPRESSION_H
