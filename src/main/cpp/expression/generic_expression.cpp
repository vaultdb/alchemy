#include "generic_expression.h"
#include <expression/visitor/type_inference_visitor.h>
#include "expression_factory.h"

using namespace vaultdb;


template<typename B>
GenericExpression<B>::GenericExpression(ExpressionNode<B> *root, const QuerySchema &input_schema)
: Expression<B>("anonymous", GenericExpression<B>::inferFieldType(root, input_schema)) {
    root_ = root->clone();
}

template<typename B>
GenericExpression<B>::GenericExpression(ExpressionNode<B> *root, const string &alias, const FieldType &output_type)
        : Expression<B>(alias, output_type) {
    root_ = root->clone();

}



template<typename B>
FieldType GenericExpression<B>::inferFieldType(ExpressionNode<B> * node, const QuerySchema & input_schema) {
    TypeInferenceVisitor<B> visitor(node, input_schema);
    node->accept(&visitor);
    return visitor.getExpressionType();
}


template class vaultdb::GenericExpression<bool>;
template class vaultdb::GenericExpression<emp::Bit>;


