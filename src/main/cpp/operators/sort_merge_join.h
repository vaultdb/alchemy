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

        SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

        SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

    protected:
        QueryTable<B> *runSelf() override;
        inline std::string getOperatorType() const override {
            return "SortMergeJoin";
        }

    private:
        int alpha_1_idx_=-1, alpha_2_idx_ = -1, table_id_idx_ = -1, weight_idx_ = -1, is_new_idx_ = -1;
        StorageModel storage_model_ = StorageModel::ROW_STORE;
        vector<pair<uint32_t, uint32_t> > join_idxs_; // lhs, rhs
        bool foreign_key_input_ = false; // default: lhs = fkey (input 0, F), if rhs = fk, foreign_key_input_ = true
        int foreign_key_cardinality_ = 0; // public bound on output size
        bool lhs_smaller_ = true;
        Field<B> zero_, one_;
        bool is_secure_;
        map<int, int> rhs_field_mapping_; // temp --> original
        map<int, int> lhs_field_mapping_; // temp --> original
        QuerySchema lhs_projected_schema_, rhs_projected_schema_; // cache the schema of the smaller input relation

        pair<QueryTable<B> *, QueryTable<B> *> augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs);
        QueryTable<B> *obliviousDistribute(QueryTable<B> *input, size_t target_size);
        QueryTable<B> *obliviousExpand(QueryTable<B> *input, bool is_lhs);
        QueryTable<B> *alignTable(QueryTable<B> *input);
        QueryTable<B> *revertProjection(QueryTable<B> *s, const map<int, int> &expr_map, const bool &is_lhs) const;

        QueryTable<B> *projectSortKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs);
        int powerOfLessThanTwo(const int & n) const;

        void initializeAlphas(QueryTable<B> *dst); // update in place

        inline B joinMatch(QueryTable<B> *t, int lhs_row, int rhs_row) {
            // previous alignment step will make join keys in first n columns
            B match = true;
            for(int i = 0; i < join_idxs_.size(); ++i) {
                match = match & (t->getField(lhs_row, i) == t->getField(rhs_row, i));
            }
            return match;
        }
    };


}

#endif
