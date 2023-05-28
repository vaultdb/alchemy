#ifndef _GENERIC_EXPRESSION_H
#define _GENERIC_EXPRESSION_H

#include <expression/expression.h>
#include <expression/expression_node.h>

namespace  vaultdb {

    // wrapper for ExpressionNode
    template<typename B>
    class GenericExpression : public Expression<B> {
    public:
        GenericExpression(ExpressionNode < B> *root, const QuerySchema &input_schema);



        GenericExpression(ExpressionNode<B>  *root, const std::string & alias, const FieldType & output_type);
        Field<B> call(const QueryTuple<B> & target) const override {
            return root_->call(target);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return root_->call(src, row);
        }

        ExpressionKind kind() const override { return root_->kind(); }

        ~GenericExpression()  {
            if(root_ != nullptr) delete root_;
        }

        inline string toString() const override {
            return root_->toString() + " " +  TypeUtilities::getTypeString(Expression<B>::type_);
        }

        static FieldType inferFieldType(ExpressionNode < B> *root,  const QuerySchema &input_schema);

        ExpressionNode<B> *root_;
    };



}


#endif //_GENERIC_EXPRESSION_H
