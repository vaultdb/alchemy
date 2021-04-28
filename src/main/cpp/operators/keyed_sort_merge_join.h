#ifndef _KEYED_SORT_MERGE_JOIN_H
#define _KEYED_SORT_MERGE_JOIN_H

#include "join.h"

namespace vaultdb {
    template<typename B>
    class KeyedSortMergeJoin : public Join<B> {
    public:
        // simplified version of https://arxiv.org/pdf/2003.09481.pdf
        // only for foreign key / primary key join
        KeyedSortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key, const BoolExpression<B> & predicate);
        KeyedSortMergeJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const BoolExpression<B> & predicate);
        ~KeyedSortMergeJoin() = default;
        std::shared_ptr<QueryTable<B> > runSelf() override;


    private:
        void writeLeftTuples(std::shared_ptr<QueryTable<B> > lhs, std::shared_ptr<QueryTable<B> > dst_table);
        void writeRightTuples(std::shared_ptr<QueryTable<B> > lhs, std::shared_ptr<QueryTable<B> > dst_table,
                std::vector<std::pair<uint32_t, uint32_t> > equality_condition);

        shared_ptr<QueryTable<B> > augmentTable(shared_ptr<SecureTable> & src_table, const QuerySchema & dst_schema, const bool & foreign_key);
        shared_ptr<QueryTable<B> > augmentTable(shared_ptr<PlainTable> & src_table, const QuerySchema & dst_schema, const bool & foreign_key);

    };


}


#endif //_SORT_MERGE_JOIN_H
