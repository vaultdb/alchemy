#ifndef _CONNECTOR_EXPRESSION_NODES_H
#define _CONNECTOR_EXPRESSION_NODES_H

#include "expression_node.h"


namespace vaultdb {

    template<typename B>
    class  NotNode : public ExpressionNode<B> {
    public:
        NotNode(ExpressionNode<B> *input) : ExpressionNode<B>(input) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

        }
        ~NotNode() = default;

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::NOT) {
                return false;
            }
            const NotNode<B> &other_node = static_cast<const NotNode<B> &>(other);
            return ExpressionNode<B>::lhs_ == other_node.lhs_;
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> child = ExpressionNode<B>::lhs_->call(target);
            return Field<B>(type_, !child, 0);
        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> child = ExpressionNode<B>::lhs_->call(src, row);
            return Field<B>(type_, !child, 0);
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> child = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, !child, 0);
        }

        void accept(ExpressionVisitor<B> *visitor) override { visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::NOT; }

        ExpressionNode<B> *clone() const override {
            return new NotNode<B>(ExpressionNode<B>::lhs_);
        }

        FieldType type_;

    };




    template<typename B>
    class AndNode : public ExpressionNode<B>  {
    public:

        AndNode(ExpressionNode<B> *lhs, ExpressionNode<B> * rhs) : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

        }
        ~AndNode() = default;

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::AND) {
                return false;
            }
            const AndNode<B> &other_node = static_cast<const AndNode<B> &>(other);
            bool lhs = (*this->lhs_ == *other_node.lhs_);
            bool rhs = (*this->rhs_ == *other_node.rhs_);
            cout << "LHS: " << lhs << " RHS: " << rhs << endl;
            return lhs && rhs;
        }

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

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l && r, 0);
        }


        void accept(ExpressionVisitor<B> *visitor) override  { visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::AND; }

        ExpressionNode<B> *clone() const override {

            return new AndNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
        }

        FieldType type_;

    };



    template<typename B>
    class OrNode : public  ExpressionNode<B>  {
    public:
        OrNode(ExpressionNode<B> * lhs, ExpressionNode<B> *rhs) : ExpressionNode<B>(lhs, rhs) {
            type_ = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;

        }
        ~OrNode() = default;

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::OR) {
                return false;
            }
            const OrNode<B> &other_node = static_cast<const OrNode<B> &>(other);
            return (*this->lhs_ == *other_node.lhs_) && (*this->rhs_ == *other_node.rhs_);
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs || rhs, 0);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs || rhs, 0);
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l || r, 0);
        }


        void accept(ExpressionVisitor<B> *visitor) override  { visitor->visit(*this); }

        ExpressionKind kind() const override { return ExpressionKind::OR; }

        ExpressionNode<B> *clone() const override {
            return new OrNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
        }

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
