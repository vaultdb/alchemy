#ifndef _EXPRESSION_H
#define _EXPRESSION_H


#include <query_table/query_tuple.h>
#include <query_table/query_schema.h>
#include "expression/expression_kind.h"
#include <string>


namespace vaultdb {

template<typename B> class QueryTable;

template<typename B>
class Expression {

protected:
    std::string alias_;
    FieldType type_;


public:
    Expression() : alias_("anonymous"), type_(FieldType::INVALID) {}
    Expression(const std::string & alias, const FieldType & type) : alias_(alias), type_(type) {}
    Expression(const Expression & src) : alias_(src.alias_), type_(src.type_) { }
    virtual Expression<B> *clone() const = 0;
    virtual ~Expression() {}
    virtual bool operator==(const Expression<B> & other) const = 0;
    bool operator!=(const Expression<B> & other) const {
        return !(*this == other);
    }


    virtual Field<B> call(const QueryTuple<B> & aTuple) const = 0;
    virtual Field<B> call(const QueryTable<B>  *src, const int & row) const = 0;
    virtual Field<B> call(const QueryTable<B>  *lhs, const int & lhs_row, const QueryTable<B> *rhs, const int & rhs_row) const = 0;
    virtual ExpressionKind kind() const = 0;
    virtual std::string toString() const = 0;
    virtual ExpressionClass exprClass() const = 0;


    // what is the return type of the expression?
    FieldType getType() const { return type_; }

    // does it have an alias?
    std::string getAlias() const { return alias_; }


    // for lazy schema evaluation, e.g., Project
    void setType(const FieldType & type) {type_ = type; }
    void setAlias(const std::string & alias) {alias_ = alias; }


//    Expression& operator=(const Expression & src);

};
} // namespace vaultdb





template class vaultdb::Expression<bool>;
template class vaultdb::Expression<emp::Bit>;


#endif //_EXPRESSION_H

