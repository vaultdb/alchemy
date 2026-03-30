#ifndef _BIT_PACKING_COMPATIBILITY_VISITOR_H_
#define _BIT_PACKING_COMPATIBILITY_VISITOR_H_
#include <expression/visitor/expression_visitor.h>
#include <query_table/query_schema.h>
#include <map>
#include <expression/case_node.h>


namespace vaultdb {
template<typename B>
class BitPackingCompatibilityVisitor : public ExpressionVisitor<B> {

private:
    bool bit_packing_compatible_ = true;

    inline bool inputsCompatible(const ExpressionNode<B> *lhs, const ExpressionNode<B> *rhs) {
        if (QueryFieldDesc::storageCompatible(lhs->output_schema_, rhs->output_schema_))
            return true;
        if (lhs->kind() == ExpressionKind::LITERAL || rhs->kind() == ExpressionKind::LITERAL) {
            return true; // can adjust types for this later.  We already passed type validation earlier so we know the types are compatible
        }
        return false;
    }

    inline void checkBinaryNode(const ExpressionNode<B> & node) {
        node.lhs_->accept(this);
        node.rhs_->accept(this);
        bit_packing_compatible_ = bit_packing_compatible_ && inputsCompatible(node.lhs_, node.rhs_);
    }


public:
    explicit BitPackingCompatibilityVisitor(ExpressionNode<B> *root) { root->accept(this); }

    bool bitPackingCompatible() const { return bit_packing_compatible_; }

    void visit(InputReference<B> &node) override {} // does not change the domain or range of expression operands

    void visit(PackedInputReference<B> &node) override {} // shouldn't get here because we don't instantiate this operator until verifying we have compatibility in this visitor

    void visit(LiteralNode<B> &node) override {}

    void visit(AndNode<B> &node) override;

    void visit(OrNode<B> &node) override;

    void visit(NotNode<B> &node) override;

    void visit(PlusNode<B> &node) override { bit_packing_compatible_ = false; } // domain change is deal-breaker, no need to continue traversing

    void visit(MinusNode<B> &node) override { bit_packing_compatible_ = false; }

    void visit(TimesNode<B> &node) override { bit_packing_compatible_ = false; }

    void visit(DivideNode<B> &node) override { bit_packing_compatible_ = false; }

    void visit(ModulusNode<B> &node) override { bit_packing_compatible_ = false; }

    void visit(EqualNode<B> &node) override;

    void visit(NotEqualNode<B> &node) override;

    void visit(GreaterThanNode<B> &node) override;

    void visit(LessThanNode<B> &node) override;

    void visit(GreaterThanEqNode<B> &node) override;

    void visit(LessThanEqNode<B> &node) override;

    void visit(CastNode<B> &node) override { bit_packing_compatible_ = false; }

    void visit(CaseNode<B> &node) override { bit_packing_compatible_ = false; }

};


} // namespace vaultdb

#endif