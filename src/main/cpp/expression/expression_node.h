#ifndef _EXPRESSION_NODE_H
#define _EXPRESSION_NODE_H

#include <query_table/query_tuple.h>
#include <query_table/query_table.h>

#include <expression/expression_kind.h>
#include "visitor/expression_visitor.h"
#include <expression/bool_expression.h>
#include <query_table/field/field_factory.h>

// building blocks for composing expressions
// initially building around Calcite JSON expression format, but may extend this to join and filter expressions (etc.)

namespace vaultdb {

    template<typename B> class QueryTable;

    // all expressions implement the call method in this
    template<typename B>
    class ExpressionNode {
    public:
        ExpressionNode(ExpressionNode<B> *lhs);

        ExpressionNode(ExpressionNode<B> *lhs, ExpressionNode<B> *rhs);


        virtual Field<B> call(const QueryTuple<B> & target) const = 0;
        virtual Field<B> call(const QueryTable<B>  *src, const int & row) const = 0;
        virtual ExpressionNode<B> *clone() const = 0;

        virtual ExpressionKind kind() const = 0;
        virtual void accept(ExpressionVisitor<B> * visitor) = 0;
        ~ExpressionNode() {
            if(lhs_ != nullptr) delete lhs_;
            if(rhs_ != nullptr) delete rhs_;
        }
        std::string toString();

        ExpressionNode<B> *lhs_ = nullptr;
        ExpressionNode<B> *rhs_ = nullptr;

    };

    std::ostream &operator<<(std::ostream &os,  ExpressionNode<bool> &expression);
    std::ostream &operator<<(std::ostream &os,  ExpressionNode<Bit> &expression);


    // read a field from a tuple
    template<typename B>
    class InputReferenceNode : public ExpressionNode<B> {
    public:
        InputReferenceNode(const uint32_t & read_idx)  : ExpressionNode<B>(nullptr), read_idx_(read_idx) {}
        ~InputReferenceNode() = default;

        inline Field<B> call(const QueryTuple<B> & target) const override {
            return target.getField(read_idx_);
        }
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return src->getField(row, read_idx_);
        }

        ExpressionKind kind() const override {     return ExpressionKind::INPUT_REF;
        }

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);

        }

        ExpressionNode<B> *clone() const override {
            return new InputReferenceNode<B>(read_idx_);
        }

        uint32_t read_idx_;
    };

    template<typename B>
    class LiteralNode : public ExpressionNode<B> {
    public:
        LiteralNode(const Field<B> & literal) : ExpressionNode<B>(nullptr), payload_(literal) {

        }
        ~LiteralNode() = default;

        Field<B> call(const QueryTuple<B> & target) const override { return  payload_; }

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return payload_;
        }

        ExpressionKind kind() const override {    return ExpressionKind::LITERAL; }

        void accept(ExpressionVisitor<B> *visitor) override {  visitor->visit(*this); }

        LiteralNode<emp::Bit>  *toSecure() const {
            return new LiteralNode<emp::Bit>(payload_.secret_share());

        }

        ExpressionNode<B> *clone() const override {
            return  new LiteralNode<B>(payload_);
        }


        Field<B> payload_;



    };


    template<typename B>
    class CastNode : public ExpressionNode<B> {
    public:
        CastNode(ExpressionNode<B> *input, const FieldType &dst_type) : ExpressionNode<B>(input),  dst_type_(dst_type) { }

        ~CastNode() = default;

        inline Field<B> call(const QueryTuple<B> &target) const override {
            Field<B> src = ExpressionNode<B>::lhs_->call(target);
            return castField(src);
        }

        inline Field<B> call(const QueryTable<B> *src, const int &row) const override {
            Field<B> field = ExpressionNode<B>::lhs_->call(src, row);
            return castField(field);
        }


        ExpressionKind kind() const override {     return ExpressionKind::CAST;
         }

        void accept(ExpressionVisitor<B> *visitor) override {
            visitor->visit(*this);
        }

        ExpressionNode<B> *clone() const override {
            ExpressionNode<B> *child = ExpressionNode<B>::lhs_->clone();
            return new CastNode<B>(child, dst_type_);
        }

        FieldType dst_type_;
    private:
        inline Field<B> castField(const Field<B> &src) const {
            switch (dst_type_) {
                case FieldType::INT:
                case FieldType::SECURE_INT:
                    return FieldFactory<B>::toInt(src);
                case FieldType::LONG:
                case FieldType::SECURE_LONG:
                    return FieldFactory<B>::toLong(src);
                case FieldType::FLOAT:
                case FieldType::SECURE_FLOAT:
                    return FieldFactory<B>::toFloat(src);
                default:
                    throw;
            }

        }

    };




}





#endif //_EXPRESSION_NODE_H
