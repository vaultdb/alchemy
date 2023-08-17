#ifndef _MERGE_JOIN_H
#define _MERGE_JOIN_H

#include "join.h"
#include "expression/expression.h"

namespace  vaultdb {

    template<typename B>
    class MergeJoin : public Join<B> {
        public:
        // predicate unused, columns lined up owing to construction in input order
        MergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition()) : Join<B>(lhs, rhs, predicate, sort) {
            setup();
          }

        MergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition()) : Join<B>(lhs, rhs, predicate, sort) {
            setup();
        }

        MergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool or_dummy_tags = false) : Join<B>(lhs, rhs, predicate, sort), or_dummy_tags_(or_dummy_tags) {
            setup();
        }

        MergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool or_dummy_tags = false) : Join<B>(lhs, rhs, predicate, sort), or_dummy_tags_(or_dummy_tags)  {
            setup();
            }

       MergeJoin(const MergeJoin & src) : Join<B>(src) {
                or_dummy_tags_ = src.or_dummy_tags_;
       }

        Operator<B> *clone() const override {
             return new MergeJoin<B>(*this);
        }

    protected:
        bool or_dummy_tags_ = false;
        QueryTable<B> *runSelf() override;
        inline OperatorType getType() const override { return OperatorType::MERGE_JOIN; }


    private:
        void setup();
    };




}


#endif
