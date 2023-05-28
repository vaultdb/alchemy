#include "generic_expression.h"
#include <expression/visitor/type_inference_visitor.h>

using namespace vaultdb;



template<typename B>
FieldType GenericExpression<B>::inferFieldType(std::shared_ptr<ExpressionNode<B> > node, const QuerySchema & input_schema) {
    TypeInferenceVisitor<B> visitor(node, input_schema);
    node->accept(&visitor);
    return visitor.getExpressionType();
}


template class vaultdb::GenericExpression<bool>;
template class vaultdb::GenericExpression<emp::Bit>;


