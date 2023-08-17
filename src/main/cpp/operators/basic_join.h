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
        BasicJoin(const BasicJoin<B> & src) : Join<B>(src) {}

        Operator<B> *clone() const override {
            return new BasicJoin<B>(*this);
        }

        ~BasicJoin() = default;

    protected:
        QueryTable<B> *runSelf() override;
        inline OperatorType getType() const override { return OperatorType::NESTED_LOOP_JOIN; }

    };
}

#endif //_BASIC_JOIN_H
