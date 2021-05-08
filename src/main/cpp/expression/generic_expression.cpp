#include "generic_expression.h"

using namespace vaultdb;

template<typename B>
GenericExpression<B>::GenericExpression(std::shared_ptr<ExpressionNode<B>> root)
        : Expression<B>("anonymous", FieldType::UNKNOWN), root_(root) {}


template<typename B>
GenericExpression<B>::GenericExpression(std::shared_ptr<ExpressionNode<B>> root, const string &alias,
                                        const FieldType &output_type) : Expression<B>(alias, output_type), root_(root) {

}


template<typename B>
Field<B> GenericExpression<B>::call(const QueryTuple<B> &aTuple) const {
    return root_->call(aTuple);
}

template<typename B>
string GenericExpression<B>::toString() const {
   return root_->toString() + " " +  TypeUtilities::getTypeString(Expression<B>::type_);
}


template class vaultdb::GenericExpression<bool>;
template class vaultdb::GenericExpression<emp::Bit>;


