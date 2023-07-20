#ifndef _MERGE_JOIN_H
#define _MERGE_JOIN_H

#include "join.h"
#include "expression/expression.h"

namespace  vaultdb {

    template<typename B>
    class MergeJoin : public Join<B> {
        public:

        MergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition()) : Join<B>(lhs, rhs, predicate, sort) {
          }

        MergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition()) : Join<B>(lhs, rhs, predicate, sort) {
            this->output_cardinality_ = lhs->getTupleCount();
        }



    protected:
        QueryTable<B> *runSelf() override;
        inline string getOperatorType() const override {     return "MergeJoin"; }

    private:
        void setup();
    };




}


#endif // _FILTER_H
