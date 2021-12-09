// creates a cross product of its input relations, sets the dummy tag in output to reflect selected tuples

#ifndef _BASIC_JOIN_H
#define _BASIC_JOIN_H


#include "join.h"


namespace vaultdb {

    template<typename  B>
    class BasicJoin : public Join<B> {

    public:
        BasicJoin(Operator<B> *lhs, Operator<B> *rhs, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        BasicJoin(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        ~BasicJoin() = default;

    protected:
        std::shared_ptr<QueryTable<B> > runSelf() override;
        string getOperatorType() const override;

    };
}

#endif //_BASIC_JOIN_H
