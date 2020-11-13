#ifndef _EXPRESSION_H
#define _EXPRESSION_H


#include <query_table/query_tuple.h>
#include <query_table/query_schema.h>

using namespace vaultdb;

class Expression {

    protected:
        std::string alias;
        types::TypeId expressionType;
        types::Value (*exprFunc)(const QueryTuple &) = nullptr;


    public:
        Expression() : alias("anonymous"), expressionType(types::TypeId::INVALID) {}


        Expression(const Expression & src) : alias(src.alias), expressionType(src.expressionType) {
            exprFunc = src.exprFunc;
        }

        Expression(types::Value (*funcPtr)(const QueryTuple &), const std::string & anAlias, const types::TypeId & aType)  :  alias(anAlias), expressionType(aType){
            exprFunc = funcPtr;
        }

        ~Expression() {}


         types::Value expressionCall(const QueryTuple & aTuple) const {
            return exprFunc(aTuple);
        }



        // what is the return type of the expression?
        types::TypeId getType() const { return expressionType; }

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
