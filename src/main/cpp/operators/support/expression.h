//
// Created by Jennie Rogers on 9/3/20.
//

#ifndef _EXPRESSION_H
#define _EXPRESSION_H


#include <querytable/query_tuple.h>
#include <querytable/query_schema.h>

using namespace vaultdb;

class Expression {

    protected:
        std::string alias;
        types::TypeId expressionType;
        // since we are in MPC, this will always be private, so hardcoding this
        const bool isPrivateField = true;


    public:
        Expression() : alias("anonymous"), expressionType(types::TypeId::INVALID) {};

        Expression(const std::string & anAlias, const types::TypeId & aType)  : alias(anAlias), expressionType(aType) {};

        Expression(const Expression & src) : alias(src.alias), expressionType(src.expressionType) {};
        ~Expression() {}

        // override when we instantiate a predicate
        virtual types::Value expressionCall(const QueryTuple & aTuple) const { throw; } // we should never get here



        // what is the return type of the expression?
        types::TypeId getType() const { return expressionType; }

        // does it have an alias?
        std::string getAlias() const { return alias; }

        bool isPrivate() const { return isPrivateField; }


         Expression& operator=(const Expression & src) {
             if(&src == this)
                 return *this;

             this->alias = src.alias;
             this->expressionType = src.expressionType;

        }


};


#endif //_EXPRESSION_H
