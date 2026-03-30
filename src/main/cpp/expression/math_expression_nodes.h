#ifndef _MATH_EXPRESSION_NODE_H
#define _MATH_EXPRESSION_NODE_H

#include "expression/expression_node.h"

namespace vaultdb {

template<typename B>
class PlusNode : public ExpressionNode<B>  {
public:
    PlusNode(ExpressionNode<B> *lhs, ExpressionNode<B>  *rhs)  : ExpressionNode<B>(lhs, rhs) { }

    ~PlusNode() = default;

    bool operator==(const ExpressionNode<B> &other) const override {
        if (other.kind() != ExpressionKind::PLUS) {
            return false;
        }
        const PlusNode<B> &other_node = static_cast<const PlusNode<B> &>(other);
        return (*this->lhs_ == *other_node.lhs_) && (*this->rhs_ == *other_node.rhs_);
    }

    Field<B> call(const QueryTuple<B> & target) const override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
        return lhs + rhs;
    }

    inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

        return lhs + rhs;
    }

    Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
        Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
        Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
        return l + r;
    }

    void accept(ExpressionVisitor<B> *visitor) override {    visitor->visit(*this); }

    ExpressionKind kind() const override {     return ExpressionKind::PLUS; }

    ExpressionNode<B> *clone() const override {
        return new PlusNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
    }
};

template<typename B>
class MinusNode : public  ExpressionNode<B>  {
public:
    MinusNode(ExpressionNode<B> * lhs, ExpressionNode<B>  *rhs)  : ExpressionNode<B>(lhs, rhs) { }

    ~MinusNode() = default;
    bool operator==(const ExpressionNode<B> &other) const override {
        if (other.kind() != ExpressionKind::MINUS) {
            return false;
        }
        const MinusNode<B> &other_node = static_cast<const MinusNode<B> &>(other);
        return (*this->lhs_ == *other_node.lhs_) && (*this->rhs_ == *other_node.rhs_);
    }

    Field<B> call(const QueryTuple<B> & target) const override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
        return lhs - rhs;
    }

    inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

        return lhs - rhs;
    }

    Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
        Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
        Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
        return l - r;
    }

    void accept(ExpressionVisitor<B> *visitor) override {
        visitor->visit(*this);
    }

    ExpressionKind kind() const override {   return ExpressionKind::MINUS; }

    ExpressionNode<B> *clone() const override {

        return new MinusNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
    }
};



template<typename B>
class TimesNode : public ExpressionNode<B>  {
public:
    TimesNode(ExpressionNode<B> *lhs, ExpressionNode<B> *rhs)  : ExpressionNode<B>(lhs, rhs) { }
    ~TimesNode() = default;

    bool operator==(const ExpressionNode<B> &other) const override {
        if (other.kind() != ExpressionKind::TIMES) {
            return false;
        }
        const TimesNode<B> &other_node = static_cast<const TimesNode<B> &>(other);
        return (*this->lhs_ == *other_node.lhs_) && (*this->rhs_ == *other_node.rhs_);
    }

    Field<B> call(const QueryTuple<B> & target) const override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
        return lhs * rhs;
    }
    inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

        return lhs * rhs;
    }

    Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
        Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
        Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
        return l * r;
    }

    void accept(ExpressionVisitor<B> *visitor) override {
        visitor->visit(*this);
    }

    ExpressionKind kind() const override {   return ExpressionKind::TIMES; }

    ExpressionNode<B> *clone() const override {

        return new TimesNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
    }
};

template<typename B>
class DivideNode : public  ExpressionNode<B>  {
public:
    DivideNode(ExpressionNode<B> *lhs, ExpressionNode<B> *rhs)  : ExpressionNode<B>(lhs, rhs) { }
    ~DivideNode() = default;

    bool operator==(const ExpressionNode<B> &other) const override {
        if (other.kind() != ExpressionKind::DIVIDE) {
            return false;
        }
        const DivideNode<B> &other_node = static_cast<const DivideNode<B> &>(other);
        return (*this->lhs_ == *other_node.lhs_) && (*this->rhs_ == *other_node.rhs_);
    }

    Field<B> call(const QueryTuple<B> & target) const override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
        return lhs / rhs;
    }

    inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

        return lhs / rhs;
    }

    Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
        Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
        Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
        return l / r;
    }

    void accept(ExpressionVisitor<B> *visitor) override {
        visitor->visit(*this);
    }

    ExpressionKind kind() const override {
        return ExpressionKind::DIVIDE;
    }

    ExpressionNode<B> *clone() const override {
        return new DivideNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
    }
};


template<typename B>
class ModulusNode : public  ExpressionNode<B>  {
public:
    ModulusNode(ExpressionNode<B> * lhs,  ExpressionNode<B>  *rhs)  : ExpressionNode<B>(lhs, rhs) { }
    ~ModulusNode() = default;

    bool operator==(const ExpressionNode<B> &other) const override {
        if (other.kind() != ExpressionKind::MOD) {
            return false;
        }
        const ModulusNode<B> &other_node = static_cast<const ModulusNode<B> &>(other);
        return (*this->lhs_ == *other_node.lhs_) && (*this->rhs_ == *other_node.rhs_);
    }

    Field<B> call(const QueryTuple<B> & target) const override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(target);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(target);
        return lhs % rhs;
    }
    inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
        Field<B> lhs = ExpressionNode<B>::lhs_->call(src, row);
        Field<B> rhs = ExpressionNode<B>::rhs_->call(src, row);

        return lhs % rhs;
    }

    Field<B> call(const QueryTable<B> *lhs, const int &lhs_row, const QueryTable<B> *rhs, const int &rhs_row) const override {
        Field<B> l = ExpressionNode<B>::lhs_->call(lhs, lhs_row, rhs, rhs_row);
        Field<B> r = ExpressionNode<B>::rhs_->call(lhs, lhs_row, rhs, rhs_row);
        return l % r;
    }

    void accept(ExpressionVisitor<B> *visitor) override {
        visitor->visit(*this);
    }

    ExpressionKind kind() const override {
        return ExpressionKind::MOD;
    }

    ExpressionNode<B> *clone() const override {
        return new ModulusNode<B>(ExpressionNode<B>::lhs_, ExpressionNode<B>::rhs_);
    }
};

} // namespace vaultdb



template class vaultdb::PlusNode<bool>;
template class vaultdb::PlusNode<emp::Bit>;

template class vaultdb::MinusNode<bool>;
template class vaultdb::MinusNode<emp::Bit>;

template class vaultdb::TimesNode<bool>;
template class vaultdb::TimesNode<emp::Bit>;

template class vaultdb::DivideNode<bool>;
template class vaultdb::DivideNode<emp::Bit>;

template class vaultdb::ModulusNode<bool>;
template class vaultdb::ModulusNode<emp::Bit>;


#endif // _MATH_EXPRESSION_NODE_H
