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

        GenericExpression(const GenericExpression<B> & src) {
            root_ = src.root_->clone();
        }

        Expression<B> *clone() const override {
            return new GenericExpression<B>(*this);
        }


        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return root_->call(src, row);
        }

        Field<B> call(const QueryTable<B>  *lhs, const int & lhs_row, const QueryTable<B> *rhs, const int & rhs_row) const override {
            return  root_->call(lhs, lhs_row, rhs, rhs_row);
        }

        ExpressionKind kind() const override { return root_->kind(); }
        ExpressionClass exprClass() const override { return ExpressionClass::GENERIC; }

        ~GenericExpression()  {
            if(root_ != nullptr) {
                delete root_;
            }
        }

        inline string toString() const override {
            return root_->toString() + " " +  TypeUtilities::getTypeString(Expression<B>::type_);
        }


        static FieldType inferFieldType(ExpressionNode < B> *root,  const QuerySchema &input_schema);

        ExpressionNode<B> *root_ = nullptr;
    };



}


#endif //_GENERIC_EXPRESSION_H
