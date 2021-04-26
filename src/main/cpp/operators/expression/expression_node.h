#ifndef _EXPRESSION_NODE_H
#define _EXPRESSION_NODE_H

#include <query_table/query_tuple.h>

// building blocks for composing arbitrary expressions
// initially building around Calcite JSON expression format, but may extend this to join and filter expressions (etc.)

namespace vaultdb {

    // all expressions implement the call method in this
    template<typename B>
    class ExpressionNode {
    public:
        ExpressionNode(std::shared_ptr<ExpressionNode<B> > lhs);
        ExpressionNode(std::shared_ptr<ExpressionNode<B> > lhs, std::shared_ptr<ExpressionNode<B> > rhs);

        virtual Field<B> call(const QueryTuple<B> & target) const = 0;
        virtual ~ExpressionNode() {}
    protected:
        std::shared_ptr<ExpressionNode<B> > lhs_;
        std::shared_ptr<ExpressionNode<B> > rhs_;

    };


    // read a field from a tuple
    template<typename B>
    class InputReferenceNode : public ExpressionNode<B> {
    public:
        InputReferenceNode(const uint32_t & read_idx);
        ~InputReferenceNode() = default;

        Field<B> call(const QueryTuple<B> & target) const override;

    private:
        uint32_t read_idx_;
    };

    template<typename B>
    class LiteralNode : public ExpressionNode<B> {
    public:
        LiteralNode(const Field<B> & literal);
        ~LiteralNode() = default;

        Field<B> call(const QueryTuple<B> & target) const override;

    private:
        Field<B> payload_;
    };


}



#endif //VAULTDB_EMP_EXPRESSION_NODE_H
