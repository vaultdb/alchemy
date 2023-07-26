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

        MergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool leafNodeFlag = false) : Join<B>(lhs, rhs, predicate, sort) {
            this->setLeafNodeFlag(leafNodeFlag);
            setup();
        }

        MergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate, const SortDefinition & sort = SortDefinition(), bool leafNodeFlag = false) : Join<B>(lhs, rhs, predicate, sort) {
            this->setLeafNodeFlag(leafNodeFlag);
            setup();
        }



    protected:
        bool leafNodeFlag = false;
        QueryTable<B> *runSelf() override;
        QueryTable<B> *runSelf(const bool leafNodeFlag);
        inline string getOperatorType() const override {     return "MergeJoin"; }

        bool getLeafNodeFlag() { return this->leafNodeFlag; }
        void setLeafNodeFlag(bool leafNodeFlag) {this->leafNodeFlag = leafNodeFlag ;}

    private:
        void setup();
    };




}


#endif
