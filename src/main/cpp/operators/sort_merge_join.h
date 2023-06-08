#ifndef _SORT_MERGE_JOIN_H
#define _SORT_MERGE_JOIN_H

#include "join.h"

namespace vaultdb {
    template<typename B>
    class SortMergeJoin : public Join<B> {
    public:
        // simplified version of https://arxiv.org/pdf/2003.09481.pdf
        // only for foreign key / primary key join
        SortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key, const BoolExpression<B> & predicate);
        SortMergeJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const BoolExpression<B> & predicate);
        ~SortMergeJoin() = default;


    protected:
        std::shared_ptr<QueryTable<B> > runSelf() override;
        string getOperatorType() const override { return "SortMergeJoin"; }


    private:
        shared_ptr<QueryTable<B> > augmentTable(shared_ptr<SecureTable> & src_table, const QuerySchema & dst_schema, const bool & foreign_key);
        shared_ptr<QueryTable<B> > augmentTable(shared_ptr<PlainTable> & src_table, const QuerySchema & dst_schema, const bool & foreign_key);

    };


}


#endif //_SORT_MERGE_JOIN_H
