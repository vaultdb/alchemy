#ifndef _EXPRESSION_COST_MODEL_
#define _EXPRESSION_COST_MODEL_
// a visitor that walks query tree and adds up costs
// analogous to the RexNodeCostVisitor in Java
// porting it to here for easier evaluation of ad-hoc expressions

#include "expression/visitor/expression_visitor.h"
#include "query_table/query_schema.h"

namespace vaultdb {
    // relies on TypeValidationVisitor in PlanParser to have already run
    // does not do type validation itself
    template<typename B>
    class ExpressionCostModel : public ExpressionVisitor<B> {
    public:
        ExpressionCostModel(ExpressionNode<B> *root, const QuerySchema & input_schema) : cumulative_cost_(0L), input_schema_(input_schema) {
            root->accept(this);
        }

        void visit(InputReference<B>  & node) override;

        void visit(PackedInputReference<B> & node) override;

        void visit(LiteralNode<B>  & node) override;

        void visit(AndNode<B>  & node) override {
            cumulative_cost_ = sumChildCosts(*((ExpressionNode<B> *) &node))  + 1; // +1 for AND gate
        }

        void visit(OrNode<B>  & node) override {
            cumulative_cost_ = sumChildCosts(*((ExpressionNode<B> *) &node)) + 1; // +1 for OR gate
        }

        void visit(NotNode<B>  & node) override;

        void visit(PlusNode<B>  & node) override {
            add_subtract(*((ExpressionNode<B> *) &node));
        }

        void visit(MinusNode<B>  & node) override {
            add_subtract(*((ExpressionNode<B> *) &node));
        }

        void visit(TimesNode<B>  & node) override;
        void visit(DivideNode<B>  & node) override;

        void visit(ModulusNode<B>  & node) override {
            throw; // not yet implemented
        }

        void visit(EqualNode<B>  & node) override {
          eq(*((ExpressionNode<B> *) &node));
        }

        void visit(NotEqualNode<B>  & node) override {
            // same cost as for ==, with free NOT gate
            eq(*((ExpressionNode<B> *) &node));
        }

        void visit(LessThanNode<B>  & node) override {
         comparison(*((ExpressionNode<B> *) &node));
        }

        void visit(GreaterThanEqNode<B>  & node) override {
            comparison(*((ExpressionNode<B> *) &node));
        }
        // same as >=
        void visit(GreaterThanNode<B>  & node) override {
            comparison(*((ExpressionNode<B> *) &node));
        }
        // same as >=
        void visit(LessThanEqNode<B>  & node) override {
            comparison(*((ExpressionNode<B> *) &node));

        }


        void visit(CastNode<B>  & node) override {
            throw; // Not yet implemented
        }

        void visit(CaseNode<B>  & node) override;

        size_t cost() const { return cumulative_cost_; }
    private:
        QuerySchema input_schema_;
        QueryFieldDesc last_field_desc_;
        size_t cumulative_cost_ = 0L;

        size_t sumChildCosts(ExpressionNode<B> & node);

        void add_subtract(ExpressionNode<B> & node);

        size_t eq(ExpressionNode<B> & node);

        void comparison(ExpressionNode<B> & node);

    };



}

#endif
