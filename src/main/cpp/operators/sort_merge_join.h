#ifndef __SORT_MERGE_JOIN__
#define __SORT_MERGE_JOIN__

#include <expression/generic_expression.h>
#include "operator.h"
#include "join.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

namespace vaultdb {
	template<typename B>
	class SortMergeJoin : public Join<B> {
	public:
		SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

		void updateCollation() override {
            // just taking the first instance of equi-join key
            // TODO: refine sort order in optimizer
            this->getChild()->updateCollation();
            this->getChild(1)->updateCollation();

            SortDefinition sort_def;
            for(auto &pos : join_idxs_) {
                sort_def.push_back(ColumnSort (pos.first, SortDirection::ASCENDING));
                sort_def.push_back(ColumnSort (pos.second, SortDirection::ASCENDING));
            }
            this->sort_definition_ = sort_def;

        }

	protected:
        QueryTable<B> *runSelf() override;

        OperatorType getType() const override { return OperatorType::SORT_MERGE_JOIN; }

	private:
        int alpha_idx_=-1, table_id_idx_ = -1, weight_idx_ = -1, is_new_idx_ = -1; // alpha_2_idx_ = -1,
        vector<pair<uint32_t, uint32_t> > join_idxs_;

		Field<B> zero_, one_;
        FieldType int_field_type_, bool_field_type_;
        bool is_secure_;
        map<int, int> rhs_field_mapping_; // normalized --> original
        map<int, int> lhs_field_mapping_; // normalized --> original
        QuerySchema lhs_projected_schema_, rhs_projected_schema_; // cache the schema of the smaller input relation
        bool bit_packed_ = false;
        long max_intermediate_cardinality_ = 0;
		QueryTable<B> *lhs_prime_;
		QueryTable<B> *rhs_prime_;
		Field<B> table_id_field_;

        pair<QueryTable<B> *, QueryTable<B> *> augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs);
        QueryTable<B> *obliviousDistribute(QueryTable<B> *input, size_t target_size);
        QueryTable<B> *obliviousExpand(QueryTable<B> *input, bool is_lhs);

		QuerySchema getAugmentedSchema();

        //QueryTable<B> *alignTable(QueryTable<B> *input);
        QueryTable<B> *revertProjection(QueryTable<B> *src, const map<int, int> &expr_map, const bool &is_lhs) const;

        QueryTable<B> *projectJoinKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs);

        void initializeAlphas(QueryTable<B> *dst);

        QueryTable<B> *unionAndSortTables();

        inline B joinMatch(QueryTable<B> *t, int lhs_row, int rhs_row) {
            // previous alignment step will make join keys in first n columns
            // this will only invoke bit packed predicate when packing is enabled
            B match = true;
            for(int i = 0; i < join_idxs_.size(); ++i) {
                match = match & (t->getField(lhs_row, i) == t->getField(rhs_row, i));
            }
            return match;
        }

        void setup();

	};
}

#endif
