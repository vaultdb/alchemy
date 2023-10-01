#ifndef _TYPE_INFERENCE_VISITOR_H
#define _TYPE_INFERENCE_VISITOR_H

#include "expression_visitor.h"
#include <query_table/query_schema.h>
#include <map>
#include <expression/case_node.h>


namespace vaultdb {
    template<typename B>
    class TypeValidationVisitor : public ExpressionVisitor<B> {

    public:
        TypeValidationVisitor(ExpressionNode<B> *root, const QuerySchema & input_schema);

        void visit(InputReference<B>  & node) override;
        void visit(PackedInputReference<B>  & node) override;
        void visit(LiteralNode<B>  & node) override;
        void visit(NoOp<B> & node) override { last_expression_type_ =  node.output_.getType(); }

        void visit(AndNode<B>  & node) override;
        void visit(OrNode<B>  & node) override;
        void visit(NotNode<B>  & node) override;

        void visit(PlusNode<B>  & node) override;
        void visit(MinusNode<B>  & node) override;
        void visit(TimesNode<B>  & node) override;
        void visit(DivideNode<B>  & node) override;
        void visit(ModulusNode<B>  & node) override;

        void visit(EqualNode<B>  & node) override;
        void visit(NotEqualNode<B>  & node) override;
        void visit(GreaterThanNode<B>  & node) override;
        void visit(LessThanNode<B>  & node) override;
        void visit(GreaterThanEqNode<B>  & node) override;
        void visit(LessThanEqNode<B>  & node) override;

        void visit(CastNode<B>  & node) override;
        void visit(CaseNode<B>  & node) override;

        FieldType getExpressionType() const;
    private:
        QuerySchema input_schema_;
        FieldType last_expression_type_;
        FieldType bool_type_;

        void verifyBinaryNode(const ExpressionNode<B> & node);

        std::map<FieldType, int> type_rank_;

    };


}


#endif //VAULTDB_EMP_TYPE_INFERENCE_VISITOR_H
