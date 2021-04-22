#ifndef _FUNCTION_EXPRESSION_H
#define _FUNCTION_EXPRESSION_H


#include "expression.h"
#include <query_table/query_tuple.h>
#include <query_table/query_schema.h>

using namespace vaultdb;

template<typename B>
class FunctionExpression : public Expression<B> {

    protected:
        // function pointer for expression
        Field<B> (*exprFunc)(const QueryTuple<B> &) = nullptr;


    public:
        FunctionExpression() : Expression<B>(FieldType::INVALID, "anonymous") {}


        FunctionExpression(const FunctionExpression & src) : Expression<B>(src.getType(), src.getAlias()) {
            exprFunc = src.exprFunc;
        }

        FunctionExpression(Field<B> (*funcPtr)(const QueryTuple<B> &), const FieldType & type, const std::string & alias)  : Expression<B>(type, alias) {
            exprFunc = funcPtr;
        }

        ~FunctionExpression() = default;


         Field<B> expressionCall(const QueryTuple<B> & aTuple) const override {
            return exprFunc(aTuple);
        }




         FunctionExpression& operator=(const FunctionExpression & src)  {
             if(&src == this)
                 return *this;

             this->alias_ = src.alias_;
             this->expression_type_ = src.expression_type_;
             this->exprFunc = src.exprFunc;

             return *this;
        }


};


#endif //_EXPRESSION_H
