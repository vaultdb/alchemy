#ifndef _SORT_MERGE_JOIN_
#define _SORT_MERGE_JOIN_

#include <expression/generic_expression.h>
#include "operator.h"
#include "join.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

// TODOs:
// reduce counts to log(fkey_length) bits

namespace  vaultdb {
    template<typename B>
    class SortMergeJoin : 
		public Join<B> {
	public:

        SortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

        SortMergeJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

        SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

        SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
                      const SortDefinition &sort = SortDefinition());

        Operator<B> *clone() const override {
            return new SortMergeJoin<B>(this->lhs_child_->clone(), this->rhs_child_->clone(), this->foreign_key_input_, this->predicate_->clone(), this->sort_definition_);
        }

        QueryTable<B> *unionAndSortTables();

		int foreignKeyChild() const { return foreign_key_input_; }
        QuerySchema deriveAugmentedSchema() const;

    protected:
        QueryTable<B> *runSelf() override;
        inline std::string getOperatorTypeString() const override {
            return "SortMergeJoin";
        }

        OperatorType getOperatorType() const override { return OperatorType::SORT_MERGE_JOIN; }

    private:
        int alpha_idx_=-1, table_id_idx_ = -1, weight_idx_ = -1, is_new_idx_ = -1; // alpha_2_idx_ = -1,
        StorageModel storage_model_ = StorageModel::ROW_STORE;
        vector<pair<uint32_t, uint32_t> > join_idxs_; // lhs, rhs
        bool foreign_key_input_ = false; // default: lhs = fkey (input 0, F), if rhs = fk, foreign_key_input_ = true
        int foreign_key_cardinality_ = 0; // public bound on output size
        bool lhs_smaller_ = true;
        Field<B> zero_, one_;
        FieldType int_field_type_, bool_field_type_;
        bool is_secure_;
        map<int, int> rhs_field_mapping_; // temp --> original
        map<int, int> lhs_field_mapping_; // temp --> original
        QuerySchema lhs_projected_schema_, rhs_projected_schema_; // cache the schema of the smaller input relation
        bool bit_packed_ = false;
        long max_intermediate_cardinality_ = 0;
		QueryTable<B> *lhs_prime_;
		QueryTable<B> *rhs_prime_;
		Field<B> table_id_field_;
        pair<QueryTable<B> *, QueryTable<B> *> augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs);
        QueryTable<B> *obliviousDistribute(QueryTable<B> *input, size_t target_size);
        QueryTable<B> *obliviousExpand(QueryTable<B> *input, bool is_lhs);
        // only for use when bit_packed_ = true and is_secure_ = true
        QueryTable<B> *obliviousExpandPacked(QueryTable<B> *input, bool is_lhs);
        QuerySchema getAugmentedSchema();

        //QueryTable<B> *alignTable(QueryTable<B> *input);
        QueryTable<B> *revertProjection(QueryTable<B> *s, const map<int, int> &expr_map, const bool &is_lhs) const;

        QueryTable<B> *projectSortKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs);

        void initializeAlphas(QueryTable<B> *dst); // update in place
        void initializeAlphasPacked(QueryTable<B> *dst); // update in place


        inline B joinMatch(QueryTable<B> *t, int lhs_row, int rhs_row) {
            // previous alignment step will make join keys in first n columns
            // this will only invoke bit packed predicate when packing is enabled
            B match = true;
            for(int i = 0; i < join_idxs_.size(); ++i) {
                match = match & (t->getPackedField(lhs_row, i) == t->getPackedField(rhs_row, i));
            }
            return match;
        }

        // to be run after getAugmentedSchema so lhs_prime_ and rhs_prime_ are initialized
        inline bool sortCompatible() {
            auto lhs_sort = lhs_prime_->getSortOrder();
            auto rhs_sort = rhs_prime_->getSortOrder();

            if(lhs_sort.size() < join_idxs_.size() || rhs_sort.size() < join_idxs_.size())
                return false;

            bool sort_compatible = true;
            for(int i = 0; i < join_idxs_.size(); ++i) {
                if(lhs_sort[i].first != i || rhs_sort[i].first != i) {
                    sort_compatible = false;
                    break;
                }
            }
            return sort_compatible;
        }

        void setup();
    };



}

#endif
