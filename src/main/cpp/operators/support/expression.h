#ifndef _EXPRESSION_H
#define _EXPRESSION_H


#include <query_table/query_tuple.h>
#include <query_table/query_schema.h>

using namespace vaultdb;

template<typename B>
class Expression {

    protected:
        std::string alias;
        FieldType expressionType;
        // function pointer for expression
        Field<B> (*exprFunc)(const QueryTuple<B> &) = nullptr;


    public:
        Expression() : alias("anonymous"), expressionType(FieldType::INVALID) {}


        Expression(const Expression & src) : alias(src.alias), expressionType(src.expressionType) {
            exprFunc = src.exprFunc;
        }

        Expression(Field<B> (*funcPtr)(const QueryTuple<B> &), const std::string & anAlias, const FieldType & aType)  :  alias(anAlias), expressionType(aType){
            exprFunc = funcPtr;
        }

        ~Expression() = default;


         Field<B> expressionCall(const QueryTuple<B> & aTuple) const {
            return exprFunc(aTuple);
        }



        // what is the return type of the expression?
        FieldType getType() const { return expressionType; }

        // does it have an alias?
        std::string getAlias() const { return alias; }


         Expression& operator=(const Expression & src) {
             if(&src == this)
                 return *this;

             this->alias = src.alias;
             this->expressionType = src.expressionType;
             this->exprFunc = src.exprFunc;

             return *this;
        }


};


#endif //_EXPRESSION_H
