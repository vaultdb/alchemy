#ifndef _BOOL_EXPRESSION_H
#define _BOOL_EXPRESSION_H
#include "expression.h"
#include "expression_node.h"

#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>


namespace  vaultdb {

    template<typename B> class ExpressionNode;

    template<typename B>
    // wrapper for ExpressionNode
    class BoolExpression : public Expression<B> {
    public:
        BoolExpression(ExpressionNode<B> * root) : Expression<B>(), root_(root){
            Expression<B>::alias_ = "predicate";
            Expression<B>::type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

        }


        Field<B> call(const QueryTuple<B> &aTuple) const override {
            return root_->call(aTuple);
        }

        Field<B> call(const QueryTable<B>  *src, const int & row) const override {
            return root_->call(src, row);
        }
        B callBoolExpression(const QueryTuple<B> &tuple) const {
            Field<B> output = root_->call(tuple);
            return output.template getValue<B>();
        }

        B callBoolExpression(const QueryTable<B>  *src, const int & row) const {
            Field<B> f = root_->call(src, row);
            return f.template getValue<B>();
        }


        ExpressionKind kind() const override { return root_->kind(); }


        ~BoolExpression() {
           if(root_ != nullptr) delete root_;
        }

        string toString() const override {
            return root_->toString() + " " +  TypeUtilities::getTypeString(Expression<B>::type_);
        }

        ExpressionNode<B>  *root_;
        FieldType type_;
    };

    template<typename B>
    std::ostream &operator<<(std::ostream &os,  BoolExpression<B> &expression) {
        os << *(expression.root_);
        return os;
    }


}

#endif //_BOOL_EXPRESSION_H
