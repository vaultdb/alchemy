#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <query_table/query_tuple.h>
#include <query_table/query_schema.h>

namespace  vaultdb {
template<typename B>
    class Expression {
    public:
        Expression(const FieldType & type, const std::string & alias)
            : expression_type_(type), alias_(alias) {}

       // implemented by child methods
        virtual Field<B> expressionCall(const QueryTuple<B> & aTuple) const = 0;
        //virtual Expression& operator=(const Expression & src) = 0;



        // what is the return type of the expression?
        FieldType getType() const { return expression_type_; }

        // does it have an alias?
        std::string getAlias() const { return alias_; }

    protected:
        FieldType expression_type_;
        std::string alias_;


    };

}

#endif //VAULTDB_EMP_EXPRESSION_H
