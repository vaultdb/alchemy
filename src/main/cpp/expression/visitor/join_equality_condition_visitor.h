#ifndef _JOIN_EQUALITY_CONDITION_VISITOR_H
#define _JOIN_EQUALITY_CONDITION_VISITOR_H

#include <expression/visitor/expression_visitor.h>
#include <expression/expression_node.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/connector_expression_nodes.h>
#include <expression/math_expression_nodes.h>
#include <expression/case_node.h>

#include <stdlib.h>
#include <vector>
#include <utility>

namespace vaultdb {

// only doing this for conjunctive equality predicates
template<typename B>
class JoinEqualityConditionVisitor : public ExpressionVisitor<B> {

private:
    uint32_t last_ordinal_ = 10000; // placeholder
    std::vector<std::pair<uint32_t, uint32_t> > equalities_;

public:
    explicit JoinEqualityConditionVisitor(ExpressionNode<B> *root) {
        root->accept(this);
    }

    std::vector<std::pair<uint32_t, uint32_t> > getEqualities();

    void visit(InputReference<B> & node) override;

    void visit(PackedInputReference<B> & node) override;

    void visit(LiteralNode<B> & node) override { throw; }

    void visit(NoOp<B> & node) override { return; } // sometimes we have an empty join predicate

    void visit(AndNode<B> & node) override;

    void visit(OrNode<B> & node) override { throw; }

    void visit(NotNode<B> & node) override { throw; }

    void visit(PlusNode<B> & node) override { throw; }

    void visit(MinusNode<B> & node) override { throw; }

    void visit(TimesNode<B> & node) override { throw; }

    void visit(DivideNode<B> & node) override { throw; }

    void visit(ModulusNode<B> & node) override { throw; }

    void visit(EqualNode<B> & node) override;

    void visit(NotEqualNode<B> & node) override { throw; }

    void visit(GreaterThanNode<B> & node) override { throw; }

    void visit(LessThanNode<B> & node) override { throw; }

    void visit(GreaterThanEqNode<B> & node) override { throw; }

    void visit(LessThanEqNode<B> & node) override { throw; }

    void visit(CastNode<B> & node) override { throw; }

    void visit(CaseNode<B> & node) override { throw; }

};

} // namespace vaultdb

#endif //_JOIN_EQUALITY_CONDITION_VISITOR_H
