#ifndef _SORT_MERGE_JOIN_
#define _SORT_MERGE_JOIN_

#include <expression/generic_expression.h>
#include "operator.h"
#include "join.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

namespace  vaultdb {
    template<typename B>
    class SortMergeJoin : 
		public Join<B> {
	public:
        SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

        SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

    private:
        int alpha_1_idx_=-1, alpha_2_idx = -1, table_id_idx = -1;
        vector<pair<int, int> > join_idxs_; // lhs, rhs


        QueryTable<B> *augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs);

        QueryTable<B> *projectSortKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols);

        void initializeAlphas(QueryTable<B> *dst); // updates in place
        B joinMatch(QueryTable<B> *t, int lhs_row, int rhs_row, int join_key_cnt) {
            // previous alignment step will make join keys in first n columns
            B match = true;
            for(int i = 0; i < join_key_cnt; ++i) {
                match = match & (t->getField(lhs_row, i) == t->getField(rhs_row, i));
            }
            return match;
        }
    };


}
#endif
