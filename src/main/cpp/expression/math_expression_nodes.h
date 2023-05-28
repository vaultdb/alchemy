#ifndef _MATH_EXPRESSION_NODE_H
#define _MATH_EXPRESSION_NODE_H

#include "expression_node.h"

namespace vaultdb {

    template<typename B>
    class PlusNode : public ExpressionNode<B>  {
    public:
        PlusNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs)  : ExpressionNode<B>(lhs, rhs) { }

        ~PlusNode() = default;
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


        void accept(ExpressionVisitor<B> *visitor) override {    visitor->visit(*this);
        }

        ExpressionKind kind() const override {     return ExpressionKind::PLUS;
        }
    };

    template<typename B>
    class MinusNode : public  ExpressionNode<B>  {
    public:
        MinusNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs)  : ExpressionNode<B>(lhs, rhs) { }

        ~MinusNode() = default;
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

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionKind kind() const override {   return ExpressionKind::MINUS; }
    };



    template<typename B>
    class TimesNode : public ExpressionNode<B>  {
    public:
        TimesNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs)  : ExpressionNode<B>(lhs, rhs) { }
        ~TimesNode() = default;
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

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionKind kind() const override {   return ExpressionKind::TIMES; }
    };

    template<typename B>
    class DivideNode : public  ExpressionNode<B>  {
    public:
        DivideNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs)  : ExpressionNode<B>(lhs, rhs) { }
        ~DivideNode() = default;
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


        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionKind kind() const override {
            return ExpressionKind::DIVIDE;
        }
    };


    template<typename B>
    class ModulusNode : public  ExpressionNode<B>  {
    public:
        ModulusNode(std::shared_ptr<ExpressionNode<B> > & lhs, std::shared_ptr<ExpressionNode<B> > & rhs)  : ExpressionNode<B>(lhs, rhs) { }
        ~ModulusNode() = default;
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

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionKind kind() const override {
            return ExpressionKind::MOD;
        }
    };


}



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
