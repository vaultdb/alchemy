// creates a cross product of its input relations, sets the dummy tag in output to reflect selected tuples

#ifndef _BASIC_JOIN_H
#define _BASIC_JOIN_H


#include "join.h"


namespace vaultdb {

    template<typename  B>
    class BasicJoin : public Join<B> {

    public:
        BasicJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
        BasicJoin(QueryTable<B> *lhs, QueryTable<B> *rhs,  Expression<B> *predicate, const SortDefinition & sort = SortDefinition());
        ~BasicJoin() = default;

    protected:
        QueryTable<B> *runSelf() override;
        inline string getOperatorType() const override {     return "BasicJoin"; }

    };
}

#endif //_BASIC_JOIN_H
