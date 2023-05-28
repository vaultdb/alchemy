#include "function_expression.h"


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

