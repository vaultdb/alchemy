#ifndef _KEYED_SORT_MERGE_JOIN_
#define _KEYED_SORT_MERGE_JOIN_

#include <expression/generic_expression.h>
#include "operators/operator.h"
#include "operators/join.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <utility>
#include <vector>
#include <string>

namespace  vaultdb {
template<typename B>
class KeyedSortMergeJoin :
    public Join<B> {
public:

    KeyedSortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key, Expression<B> *predicate,
                  const SortDefinition &sort = SortDefinition());

    KeyedSortMergeJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate,
                  const SortDefinition &sort = SortDefinition());

    KeyedSortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                  const SortDefinition &sort = SortDefinition());

    KeyedSortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
                  const SortDefinition &sort = SortDefinition());
    KeyedSortMergeJoin(const KeyedSortMergeJoin & src) : Join<B>(src) {
        foreign_key_input_ = src.foreignKeyChild();
        setup();
    }

    Operator<B> *clone() const override {
        return new KeyedSortMergeJoin<B>(*this);
    }

    void updateCollation() override {
        // just taking the first instance of equi-join key
        // NYI: refine sort order in optimizer
        this->getChild()->updateCollation();
        this->getChild(1)->updateCollation();

        SortDefinition sort_def;
        // algo pushes dummy tags to end
        sort_def.push_back(ColumnSort(-1, SortDirection::ASCENDING));

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

    inline bool sortCompatible() const{
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

    // Override the getParameters method
    string getParameters() const override {
        string parameters = Join<B>::getParameters(); // Call the base class method

        // Check the sortCompatible condition and append the relevant message
        if (sortCompatible()) {
            parameters += " - sortCompatible";
        } else {
            parameters += " - not sort compatible, need sort in smj";
        }

        return parameters;
    }

    bool operator==(const Operator<B> &other) const override {
        if (other.getType() != OperatorType::KEYED_SORT_MERGE_JOIN) {
            return false;
        }

        auto rhs = dynamic_cast<const KeyedSortMergeJoin<B> &>(other);

        if(*this->predicate_ != *rhs.predicate_ || this->foreign_key_input_ != rhs.foreign_key_input_) return false;
        return this->operatorEquality(other);
    }

protected:
    QueryTable<B> *runSelf() override;

    OperatorType getType() const override { return OperatorType::KEYED_SORT_MERGE_JOIN; }

private:
    int alpha_idx_=-1, table_id_idx_ = -1, weight_idx_ = -1, is_new_idx_ = -1; // alpha_2_idx_ = -1,
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
    int64_t max_intermediate_cardinality_ = 0;
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

} // namespace vaultdb

#endif
