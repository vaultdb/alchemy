#ifndef _SORT_MERGE_JOIN_
#define _SORT_MERGE_JOIN_

#include <expression/generic_expression.h>
#include "operator.h"
#include "join.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

// TODO: create alt SMJ operator for many-to-many relationships

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
        SortMergeJoin(const SortMergeJoin & src) : Join<B>(src) {
            foreign_key_input_ = src.foreignKeyChild();
            setup();
        }

        Operator<B> *clone() const override {
            return new SortMergeJoin<B>(*this);
        }

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


        int foreignKeyChild() const { return foreign_key_input_; }
        QuerySchema deriveProjectedSchema() const;
        QuerySchema deriveAugmentedSchema() const;
        const vector<pair<uint32_t, uint32_t>> joinKeyIdxs() const { return join_idxs_; }

        inline bool sortCompatible() {
            auto lhs_sort = this->lhs_child_->getSortOrder();
            auto rhs_sort = this->rhs_child_->getSortOrder();
            auto lhs_schema = this->lhs_child_->getOutputSchema();
            if(lhs_sort.size() < join_idxs_.size() || rhs_sort.size() < join_idxs_.size())
                return false;

            for(int i = 0; i < join_idxs_.size(); ++i) {
                if(lhs_sort[i].first != join_idxs_[i].first || rhs_sort[i].first + lhs_schema.getFieldCount() != join_idxs_[i].second) {
                    return false;
                }
            }
            return true;
        }


        inline bool sortCompatible(const Operator<B> *child) {
            bool lhs_child = (child->getOperatorId() == this->getChild()->getOperatorId());
            // check it is a child
            if(!lhs_child) assert(child->getOperatorId() == this->getChild(1)->getOperatorId());

            auto child_sort = (lhs_child) ? this->getChild()->getSortOrder() : this->getChild(1)->getSortOrder();
            auto lhs_schema_cols = this->getChild()->getOutputSchema().getFieldCount();
            if(child_sort.size() < join_idxs_.size())
                return false;

            for(int i = 0; i < join_idxs_.size(); ++i) {
                if(lhs_child && child_sort[i].first != join_idxs_[i].first)  {
                    return false;
                }
                if(!lhs_child && (child_sort[i].first + lhs_schema_cols != join_idxs_[i].second)) {
                    return false;
                }
            }
            return true;
        }

        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != OperatorType::KEYED_SORT_MERGE_JOIN) {
                return false;
            }

            auto rhs = dynamic_cast<const SortMergeJoin<B> &>(other);

            if(*this->predicate_ != *rhs.predicate_ || this->foreign_key_input_ != rhs.foreign_key_input_) return false;
            return this->operatorEquality(other);
        }

    protected:
        QueryTable<B> *runSelf() override;

        OperatorType getType() const override { return OperatorType::KEYED_SORT_MERGE_JOIN; }

    private:
        int alpha_idx_=-1, table_id_idx_ = -1, weight_idx_ = -1, is_new_idx_ = -1; // alpha_2_idx_ = -1,
        int alpha1_idx_ = -1;
        int alpha2_idx_ = -1;
        vector<pair<uint32_t, uint32_t> > join_idxs_; // lhs, rhs
        bool foreign_key_input_ = false; // default: lhs = fkey (input 0, F), if rhs = fk, foreign_key_input_ = true
        int foreign_key_cardinality_ = 0; // public bound on output size
        bool lhs_smaller_ = true;
        Field<B> zero_, one_;
        FieldType int_field_type_, bool_field_type_;
        bool is_secure_;
        map<int, int> rhs_field_mapping_; // normalized --> original
        map<int, int> lhs_field_mapping_; // normalized --> original
        QuerySchema lhs_projected_schema_, rhs_projected_schema_; // cache the schema of the smaller input relation
        long max_intermediate_cardinality_ = 0;
        QueryTable<B> *lhs_prime_;
        QueryTable<B> *rhs_prime_;
        Field<B> table_id_field_;

        pair<QueryTable<B> *, QueryTable<B> *> augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs);
        QueryTable<B> *distribute(QueryTable<B> *input, size_t target_size);
        QueryTable<B> *expand(QueryTable<B> *input);
        QuerySchema getAugmentedSchema();

        QueryTable<bool> *revertProjection(QueryTable<bool> *src, const map<int, int> &expr_map, const bool &is_lhs) const;
        QueryTable<Bit> *revertProjection(QueryTable<Bit> *src, const map<int, int> &expr_map, const bool &is_lhs) const;

        QueryTable<B> *projectJoinKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs);
        //QueryTable<B> *projectJoinKeyToFirstAttrOmpc(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs);

        void initializeAlphas(QueryTable<B> *dst); // update in place

        QueryTable<B> *unionAndSortTables();
        QueryTable<B> *unionAndMergeTables();

        // union tables with disparate schemas into a new table with augmented schema
        // unions into a bitonic sequence
        QueryTable<B> *unionTables(QueryTable<B> *lhs, QueryTable<B> *rhs, const QuerySchema & dst_schema);

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
