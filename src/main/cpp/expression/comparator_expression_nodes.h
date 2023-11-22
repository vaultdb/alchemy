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

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::EQ) {
                return false;
            }
            const EqualNode<B> &other_node = static_cast<const EqualNode<B> &>(other);
            bool lhs = (*this->lhs_ == *other_node.lhs_);
            bool rhs = (*this->rhs_ == *other_node.rhs_);
            return lhs && rhs;
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs == rhs);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs == rhs);
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l == r);
        }

        ExpressionKind kind() const override { return ExpressionKind::EQ; }

        inline void accept(ExpressionVisitor<B> *visitor) override {   visitor->visit(*this); }

        ExpressionNode<B> *clone() const override {
            return new EqualNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
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

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::NEQ) {
                return false;
            }
            const NotEqualNode<B> &other_node = static_cast<const NotEqualNode<B> &>(other);
            return *this->lhs_ == *other_node.lhs_ && *this->rhs_ == *other_node.rhs_;
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

            return Field<B>(type_, lhs != rhs);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs != rhs);
        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l != r);
        }


        ExpressionKind kind() const override { return ExpressionKind::NEQ; }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            return new NotEqualNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);

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

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::LT) {
                return false;
            }
            const LessThanNode<B> &other_node = static_cast<const LessThanNode<B> &>(other);
            return *this->lhs_ == *other_node.lhs_ && *this->rhs_ == *other_node.rhs_;
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);

            return Field<B>(type_, lhs < rhs);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

            return Field<B>(type_, lhs < rhs);

        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l < r);
        }

        ExpressionKind kind() const override {
            return ExpressionKind::LT;
        }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            return new LessThanNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
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

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::GT) {
                return false;
            }
            const GreaterThanNode<B> &other_node = static_cast<const GreaterThanNode<B> &>(other);
            return *this->lhs_ == *other_node.lhs_ && *this->rhs_ == *other_node.rhs_;
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs > rhs);

        }


        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs > rhs);

        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l > r);
        }

        ExpressionKind kind() const override {
            return ExpressionKind::GT;
        }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            return new GreaterThanNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);

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

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::LEQ) {
                return false;
            }
            const LessThanEqNode<B> &other_node = static_cast<const LessThanEqNode<B> &>(other);
            return *this->lhs_ == *other_node.lhs_ && *this->rhs_ == *other_node.rhs_;
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs <= rhs);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs <= rhs);

        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l <= r);
        }

        ExpressionKind kind() const override { return ExpressionKind::LEQ; }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            return new LessThanEqNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
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

        bool operator==(const ExpressionNode<B> &other) const override {
            if (other.kind() != ExpressionKind::GEQ) {
                return false;
            }
            const GreaterThanEqNode<B> &other_node = static_cast<const GreaterThanEqNode<B> &>(other);
            return *this->lhs_ == *other_node.lhs_ && *this->rhs_ == *other_node.rhs_;
        }

        Field<B> call(const QueryTuple<B> & target) const override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
            return Field<B>(type_, lhs >= rhs);

        }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
            Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);
            return Field<B>(type_, lhs >= rhs);

        }

        Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
            Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
            Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
            return Field<B>(type_, l >= r);
        }


        ExpressionKind kind() const override { return ExpressionKind::GEQ; }

        void accept(ExpressionVisitor <B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            return new GreaterThanEqNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);

        }

        FieldType type_;

    };



}



#endif