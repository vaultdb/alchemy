#ifndef _CONNECTOR_EXPRESSION_NODES_H
#define _CONNECTOR_EXPRESSION_NODES_H

#include <util/field_utilities.h>
#include "expression_node.h"


namespace vaultdb {

    template<typename B>
    class  NotNode : public ExpressionNode<B> {
    public:
        NotNode( std::shared_ptr<ExpressionNode<B> > input);
        ~NotNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> child = ExpressionNode<B>::lhs_->call(src, row);

            return FieldUtilities::getBoolField(!child);
        }

        void accept(ExpressionVisitor<B> *visitor) override;

        ExpressionKind kind() const override;
    };




    template<typename B>
    class AndNode : public ExpressionNode<B>  {
    public:

        AndNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs);
        ~AndNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

            return FieldUtilities::getBoolField(lhs && rhs);
        }


        void accept(ExpressionVisitor<B> *visitor) override;

        ExpressionKind kind() const override;
    };



    template<typename B>
    class OrNode : public  ExpressionNode<B>  {
    public:
        OrNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs);
        ~OrNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override;

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

            return FieldUtilities::getBoolField(lhs || rhs);
        }


        void accept(ExpressionVisitor<B> *visitor) override;

        ExpressionKind kind() const override;
    };



}

#endif //_CONNECTOR_EXPRESSION_NODES_H
