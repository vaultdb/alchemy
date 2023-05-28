#ifndef _CONNECTOR_EXPRESSION_NODES_H
#define _CONNECTOR_EXPRESSION_NODES_H

#include <util/field_utilities.h>
#include "expression_node.h"


namespace vaultdb {

    template<typename B>
    class  NotNode : public ExpressionNode<B> {
    public:
        NotNode( std::shared_ptr<ExpressionNode<B> > input) : ExpressionNode<B>(input) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

        }
        ~NotNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> child = ExpressionNode<B>::lhs_->call(target);
            return Field<B>(type_, !child, 0);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> child = ExpressionNode<B>::lhs_->call(src, row);
            return Field<B>(type_, !child, 0);
        }

        void accept(ExpressionVisitor<B> *visitor) override { visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::NOT; }

        FieldType type_;

    };




    template<typename B>
    class AndNode : public ExpressionNode<B>  {
    public:

        AndNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs) : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

        }
        ~AndNode() = default;

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

            return Field<B>(type_, lhs && rhs, 0);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

            return Field<B>(type_, lhs && rhs, 0);
        }


        void accept(ExpressionVisitor<B> *visitor) override  { visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::AND; }
        FieldType type_;

    };



    template<typename B>
    class OrNode : public  ExpressionNode<B>  {
    public:
        OrNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs) : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

        }
        ~OrNode() = default;
        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

            return FieldUtilities::getBoolField(lhs || rhs);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

            return FieldUtilities::getBoolField(lhs || rhs);
        }


        void accept(ExpressionVisitor<B> *visitor) override  { visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::OR; }
        FieldType type_;

    };



}


template class vaultdb::NotNode<bool>;
template class vaultdb::NotNode<emp::Bit>;


template class vaultdb::AndNode<bool>;
template class vaultdb::AndNode<emp::Bit>;


template class vaultdb::OrNode<bool>;
template class vaultdb::OrNode<emp::Bit>;


#endif //_CONNECTOR_EXPRESSION_NODES_H
