#ifndef _EXPRESSION_DEPARSER_
#define _EXPRESSION_DEPARSER_

#include "expression/visitor/expression_visitor.h"
#include <string>
#include <expression/case_node.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::basic_ptree;

namespace pt = boost::property_tree;


namespace vaultdb {
    template<typename B>
    class ExpressionDeparser : public ExpressionVisitor<B> {
    public:
        explicit ExpressionDeparser(ExpressionNode<B> *root): root_(root) {
            root_->accept(this);
        }
        void visit(InputReference<B> & node) override;

        void visit(PackedInputReference<B> & node) override;

        void visit(LiteralNode<B> & node) override;

        void visit(NoOp<B> & node) override;

        void visit(AndNode<B> & node) override;

        void visit(OrNode<B> & node) override;

        void visit(NotNode<B> & node) override;

        void visit(PlusNode<B> & node) override;

        void visit(MinusNode<B> & node) override;

        void visit(TimesNode<B> & node) override;

        void visit(DivideNode<B> & node) override;

        void visit(ModulusNode<B> & node) override;

        void visit(EqualNode<B>&  node) override;

        void visit(NotEqualNode<B> & node) override;

        void visit(GreaterThanNode<B> & node) override;

        void visit(LessThanNode<B> & node) override;

        void visit(GreaterThanEqNode<B> & node) override;

        void visit(LessThanEqNode<B> & node) override;

        void visit(CastNode<B> & node) override;

        void visit(CaseNode<B> & node) override;

        ptree getJsonPlan() const { return last_expr_; }

        string toString() {
            stringstream ss;
            pt::write_json(ss, last_expr_);
            json_plan_ = ss.str();

            // iterate over JSON plan and delete extra quotes around some fields

            return json_plan_;
        }

    private:
        ExpressionNode<B> *root_;
        string json_plan_;
        pt::ptree last_expr_;


        void visitBinaryExpression(ExpressionNode<B> &binary_node, const std::string &connector);


    };


}
#endif //_DEPARSER_EXPRESSION_VISITOR_H
