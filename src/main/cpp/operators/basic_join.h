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
        Operator<B> *clone() const override {
            return new BasicJoin<B>(this->lhs_child_->clone(), this->rhs_child_->clone(), this->predicate_->clone(), this->sort_definition_);
        }

        ~BasicJoin() = default;

    protected:
        QueryTable<B> *runSelf() override;
        inline string getOperatorTypeString() const override {     return "BasicJoin"; }
        inline OperatorType getOperatorType() const override { return OperatorType::NESTED_LOOP_JOIN; }

    };
}

#endif //_BASIC_JOIN_H
