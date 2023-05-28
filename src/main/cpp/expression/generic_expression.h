#ifndef _GENERIC_EXPRESSION_H
#define _GENERIC_EXPRESSION_H

#include <expression/expression.h>
#include <expression/expression_node.h>

namespace  vaultdb {

    // wrapper for ExpressionNode
    template<typename B>
    class GenericExpression : public Expression<B> {
    public:
        GenericExpression(std::shared_ptr<ExpressionNode < B> > root, const QuerySchema &input_schema)         : Expression<B>("anonymous", GenericExpression<B>::inferFieldType(root, input_schema)), root_(root) {}



        GenericExpression(std::shared_ptr<ExpressionNode<B> > root, const std::string & alias, const FieldType & output_type)  : Expression<B>(alias, output_type), root_(root) {

        }
        Field<B> call(const QueryTuple<B> & target) const override {
            return root_->call(target);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return root_->call(src, row);
        }

        ExpressionKind kind() const override { return root_->kind(); }

        ~GenericExpression() = default;

        inline string toString() const override {
            return root_->toString() + " " +  TypeUtilities::getTypeString(Expression<B>::type_);
        }

        static FieldType inferFieldType(std::shared_ptr<ExpressionNode < B> > root,  const QuerySchema &input_schema);

        std::shared_ptr<ExpressionNode<B> > root_;
    };


}


#endif //_GENERIC_EXPRESSION_H
