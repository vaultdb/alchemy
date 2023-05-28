#ifndef _COMPARATOR_EXPRESSION_NODES_H
#define _COMPARATOR_EXPRESSION_NODES_H

#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>
#include "visitor/expression_visitor.h"

#include "expression_node.h"

namespace vaultdb {

    template<typename B> class ExpressionNode;

    template<typename B>
    class  EqualNode : public ExpressionNode<B> {
    public:
        EqualNode( std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs );
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


        ExpressionKind kind() const override;

        void accept(ExpressionVisitor <B> *visitor) override;
        FieldType type_;


    };

    template<typename B>
    class  NotEqualNode : public ExpressionNode<B> {
    public:
        NotEqualNode( std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs );
        ~NotEqualNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs != rhs, 0);
        }


        ExpressionKind kind() const override;

        void accept(ExpressionVisitor <B> *visitor) override;
        FieldType type_;

    };


    template<typename B>
    class  LessThanNode : public ExpressionNode<B> {
    public:
        LessThanNode( std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs );
        ~LessThanNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

            return Field<B>(type_, lhs < rhs, 0);

        }


        ExpressionKind kind() const override;

        void accept(ExpressionVisitor <B> *visitor) override;
        FieldType type_;

    };


    template<typename B>
    class  GreaterThanNode : public ExpressionNode<B> {
    public:
        GreaterThanNode( std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs );
        ~GreaterThanNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs > rhs, 0);

        }


        ExpressionKind kind() const override;

        void accept(ExpressionVisitor <B> *visitor) override;
        FieldType type_;

    };


    template<typename B>
    class  LessThanEqNode : public ExpressionNode<B> {
    public:
        LessThanEqNode( std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs );
        ~LessThanEqNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs <= rhs, 0);

        }


        ExpressionKind kind() const override;

        void accept(ExpressionVisitor <B> *visitor) override;
        FieldType type_;

    };
    template<typename B>
    class  GreaterThanEqNode : public ExpressionNode<B> {
    public:
        GreaterThanEqNode( std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs );
        ~GreaterThanEqNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs >= rhs, 0);

        }


        ExpressionKind kind() const override;

        void accept(ExpressionVisitor <B> *visitor) override;
        FieldType type_;

    };



}


#endif