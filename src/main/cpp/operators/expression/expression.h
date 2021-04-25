#ifndef _EXPRESSION_H
#define _EXPRESSION_H


#include <query_table/query_tuple.h>
#include <query_table/query_schema.h>

namespace vaultdb {

    template<typename B>
    class Expression {

    protected:
        std::string alias_;
        FieldType type_;


    public:
        Expression() : alias_("anonymous"), type_(FieldType::INVALID) {}
        Expression(const std::string & alias, const FieldType & type) : alias_(alias), type_(type) {}
        Expression(const Expression & src) : alias_(src.alias_), type_(src.type_) { }

        virtual ~Expression() {}



        virtual Field<B> expressionCall(const QueryTuple<B> & aTuple) const = 0;



        // what is the return type of the expression?
        FieldType getType() const { return type_; }

        // does it have an alias?
        std::string getAlias() const { return alias_; }


    //    Expression& operator=(const Expression & src) {
    //        if(&src == this)
    //            return *this;
    //
    //        this->alias_ = src.alias_;
    //        this->type_ = src.type_;
    //
    //        return *this;
    //    }


    };
}

template class vaultdb::Expression<bool>;
template class vaultdb::Expression<emp::Bit>;


#endif //_EXPRESSION_H

