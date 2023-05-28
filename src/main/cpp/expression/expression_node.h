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
        ExpressionNode(std::shared_ptr<ExpressionNode<B> > lhs);
        ExpressionNode(std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs);

        virtual Field<B> call(const QueryTuple<B> & target) const = 0;
        virtual Field<B> call(const QueryTable<B>  *src, const int & row) const = 0;

        virtual ExpressionKind kind() const = 0;
        virtual void accept(ExpressionVisitor<B> * visitor) = 0;
        virtual ~ExpressionNode() {}
        std::string toString();

        std::shared_ptr<ExpressionNode<B> > lhs_;
        std::shared_ptr<ExpressionNode<B> > rhs_;

    };

    std::ostream &operator<<(std::ostream &os,  ExpressionNode<bool> &expression);
    std::ostream &operator<<(std::ostream &os,  ExpressionNode<emp::Bit> &expression);


    // read a field from a tuple
    template<typename B>
    class InputReferenceNode : public ExpressionNode<B> {
    public:
        InputReferenceNode(const uint32_t & read_idx);
        ~InputReferenceNode() = default;

        Field<B> call(const QueryTuple<B> & target) const override;
        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return src->getField(row, read_idx_);
        }

        ExpressionKind kind() const override;

        void accept(ExpressionVisitor<B> *visitor) override;

        uint32_t read_idx_;
    };

    template<typename B>
    class LiteralNode : public ExpressionNode<B> {
    public:
        LiteralNode(const Field<B> & literal);
        ~LiteralNode() = default;

        Field<B> call(const QueryTuple<B> & target) const override;

        inline Field<B> call(const QueryTable<B>  *src, const int & row) const  override {
            return payload_;
        }

        ExpressionKind kind() const override;
        void accept(ExpressionVisitor<B> *visitor) override;
        std::shared_ptr<LiteralNode<emp::Bit> > toSecure() const;


        Field<B> payload_;



    };


    template<typename B>
    class CastNode : public ExpressionNode<B> {
    public:
        CastNode(std::shared_ptr<ExpressionNode<B> > &input, const FieldType &dst_type);

        ~CastNode() = default;

        inline Field<B> call(const QueryTuple<B> &target) const override {
            Field<B> src = ExpressionNode<B>::lhs_->call(target);
            return castField(src);
        }

        inline Field<B> call(const QueryTable<B> *src, const int &row) const override {
            Field<B> field = ExpressionNode<B>::lhs_->call(src, row);
            return castField(field);
        }


        ExpressionKind kind() const override;

        void accept(ExpressionVisitor<B> *visitor) override;

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
