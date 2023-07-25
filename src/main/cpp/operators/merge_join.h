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

        MergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool partyJoin = false) : Join<B>(lhs, rhs, predicate, sort) {
            this->setPartyJoin(partyJoin);
            setup();
        }

        MergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool partyJoin = false) : Join<B>(lhs, rhs, predicate, sort) {
            this->setPartyJoin(partyJoin);
            setup();
        }



    protected:
        bool partyJoin = false;
        QueryTable<B> *runSelf() override;
        QueryTable<B> *runSelf(const bool partyJoin);
        inline string getOperatorType() const override {     return "MergeJoin"; }

        bool getPartyJoin() { return this->partyJoin; }
        void setPartyJoin(bool partyJoin) {this->partyJoin = partyJoin ;}

    private:
        void setup();
    };




}


#endif
