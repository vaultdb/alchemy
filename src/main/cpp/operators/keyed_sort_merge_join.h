#ifndef _KEYED_SORT_MERGE_JOIN_H
#define _KEYED_SORT_MERGE_JOIN_H

#include "join.h"

namespace vaultdb {
    template<typename B>
    class KeyedSortMergeJoin : public Join<B> {
    public:
        // simplified version of https://arxiv.org/pdf/2003.09481.pdf
        // only for foreign key / primary key join
        KeyedSortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        KeyedSortMergeJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const BoolExpression<B> & predicate, const SortDefinition & sort = SortDefinition());
        ~KeyedSortMergeJoin() = default;
        std::shared_ptr<QueryTable<B> > runSelf() override;


    private:
        void writeLeftTuples(std::shared_ptr<QueryTable<B> > lhs, std::shared_ptr<QueryTable<B> > dst_table);
        void writeRightTuples(std::shared_ptr<QueryTable<B> > lhs, std::shared_ptr<QueryTable<B> > dst_table);

        void distributeValues(shared_ptr<QueryTable<bool> > d);
        void distributeValues(shared_ptr<QueryTable<emp::Bit> > d);

        // takes src tuple from rhs / primary key and adds table_idx column to it.
        //QueryTuple<B> augmentRightTuple(QueryTuple<B> & src);
        std::vector<pair<uint32_t, uint32_t> > equality_conditions_;
        QuerySchema lhs_schema_, rhs_schema_;

    };


}


#endif //_SORT_MERGE_JOIN_H
