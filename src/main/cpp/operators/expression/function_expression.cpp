#include "function_expression.h"

using namespace vaultdb;

template<typename B>
FunctionExpression<B>::FunctionExpression(Field<B> (*funcPtr)(const QueryTuple<B> &), const string &anAlias,
                                          const FieldType &aType)  : Expression<B>(anAlias, aType) {
    expr_func_ = funcPtr;
}

template<typename B>
Field<B> FunctionExpression<B>::expressionCall(const QueryTuple<B> &aTuple) const {
    return expr_func_(aTuple);
}
//
//template<typename B>
//FunctionExpression<B> &FunctionExpression<B>::operator=(const FunctionExpression &src) {
//    this->alias_ = src.alias_;
//    this->type_ = src.type_;
//    this->expr_func_ = src.expr_func_;
//    return *this;
//}

template class vaultdb::FunctionExpression<bool>;
template class vaultdb::FunctionExpression<emp::Bit>;

