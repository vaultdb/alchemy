#ifndef _COMPARATOR_EXPRESSION_NODES_H
#define _COMPARATOR_EXPRESSION_NODES_H

#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>

#include "expression_node.h"

namespace vaultdb {

    template<typename B> class ExpressionNode;
    template<typename B> class ExpressionVisitor;


    template<typename B>
    class  EqualNode : public ExpressionNode<B> {
    public:
        EqualNode( ExpressionNode<B> * lhs, ExpressionNode<B> * rhs ) : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
        }

        ~EqualNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs == rhs, 0);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs == rhs, 0);
        }


        ExpressionKind kind() const override { return ExpressionKind::EQ; }

        inline void accept(ExpressionVisitor<B> *visitor) override {   visitor->visit(*this); }

        ExpressionNode<B> *clone() const override {
            ExpressionNode<B> *lhs = ExpressionNode<B>::lhs_->clone();
            ExpressionNode<B> *rhs = ExpressionNode<B>::rhs_->clone();

            return new EqualNode<B>(lhs, rhs);
        }

        FieldType type_;


    };

    template<typename B>
    class  NotEqualNode : public ExpressionNode<B> {
    public:
        NotEqualNode( ExpressionNode<B> * lhs, ExpressionNode<B> * rhs ) : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
        }

        ~NotEqualNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

            return Field<B>(type_, lhs != rhs, 0);

        }
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs != rhs, 0);
        }


        ExpressionKind kind() const override { return ExpressionKind::NEQ; }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            ExpressionNode<B> *lhs = ExpressionNode<B>::lhs_->clone();
            ExpressionNode<B> *rhs = ExpressionNode<B>::rhs_->clone();

            return new NotEqualNode<B>(lhs, rhs);
        }

        FieldType type_;

    };


    template<typename B>
    class  LessThanNode : public ExpressionNode<B> {
    public:
        LessThanNode( ExpressionNode<B> * lhs, ExpressionNode<B> * rhs )   : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;


        }
        ~LessThanNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

            return Field<B>(type_, lhs < rhs, 0);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

            return Field<B>(type_, lhs < rhs, 0);

        }


        ExpressionKind kind() const override {
            return ExpressionKind::LT;
        }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            ExpressionNode<B> *lhs = ExpressionNode<B>::lhs_->clone();
            ExpressionNode<B> *rhs = ExpressionNode<B>::rhs_->clone();

            return new LessThanNode<B>(lhs, rhs);
        }

        FieldType type_;

    };


    template<typename B>
    class  GreaterThanNode : public ExpressionNode<B> {
    public:
        GreaterThanNode( ExpressionNode<B> * lhs, ExpressionNode<B> * rhs )   : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;


        }
        ~GreaterThanNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs > rhs, 0);

        }


        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs > rhs, 0);

        }


        ExpressionKind kind() const override {
            return ExpressionKind::GT;
        }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            ExpressionNode<B> *lhs = ExpressionNode<B>::lhs_->clone();
            ExpressionNode<B> *rhs = ExpressionNode<B>::rhs_->clone();

            return new GreaterThanNode<B>(lhs, rhs);
        }

        FieldType type_;

    };


    template<typename B>
    class  LessThanEqNode : public ExpressionNode<B> {
    public:
        LessThanEqNode( ExpressionNode<B> *lhs, ExpressionNode<B> *rhs )   : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;


        }
        ~LessThanEqNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs <= rhs, 0);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs <= rhs, 0);

        }


        ExpressionKind kind() const override { return ExpressionKind::LEQ; }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            ExpressionNode<B> *lhs = ExpressionNode<B>::lhs_->clone();
            ExpressionNode<B> *rhs = ExpressionNode<B>::rhs_->clone();

            return new LessThanEqNode<B>(lhs, rhs);
        }

        FieldType type_;

    };
    template<typename B>
    class  GreaterThanEqNode : public ExpressionNode<B> {
    public:
        GreaterThanEqNode( ExpressionNode<B> * lhs, ExpressionNode<B> * rhs )   : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;


        }
        ~GreaterThanEqNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs >= rhs, 0);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs >= rhs, 0);

        }


        ExpressionKind kind() const override { return ExpressionKind::GEQ; }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            ExpressionNode<B> *lhs = ExpressionNode<B>::lhs_->clone();
            ExpressionNode<B> *rhs = ExpressionNode<B>::rhs_->clone();

            return new GreaterThanEqNode<B>(lhs, rhs);
        }

        FieldType type_;

    };



}



#endif