#include "sort_merge_join.h"
#include "query_table/query_table.h"
#include "operators/project.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "operators/sort.h"
#include "util/data_utilities.h"
#include "util/field_utilities.h"
#include "query_table/field/field_factory.h"
#include "util/system_configuration.h"
#include "util/logger.h"

using namespace vaultdb;
using namespace Logging;

template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort) {
    setup();
}

template<typename B>
void SortMergeJoin<B>::setup() {
    is_secure_ = std::is_same_v<B, emp::Bit>;
    int_field_type_ = is_secure_ ? FieldType::SECURE_INT : FieldType::INT;
    bool_field_type_ = is_secure_ ? FieldType::SECURE_BOOL : FieldType::BOOL;

    auto p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    join_idxs_  = join_visitor.getEqualities();

    lhs_smaller_ = (this->getChild(0)->getOutputSchema().size() < this->getChild(1)->getOutputSchema().size());

    one_ = FieldFactory<B>::getOne(int_field_type_);
    zero_ = FieldFactory<B>::getZero(int_field_type_);

    updateCollation();

    max_intermediate_cardinality_ =  this->getChild(0)->getOutputCardinality() * this->getChild(1)->getOutputCardinality();

    if(is_secure_) {
        int card_bits = ceil(log2(max_intermediate_cardinality_)) + 1; // + 1 for sign bit
        emp::Integer zero_tmp(card_bits, 0, emp::PUBLIC);
        emp::Integer one_tmp(card_bits, 1, emp::PUBLIC);
        zero_ = Field<B>(int_field_type_, zero_tmp);
        one_ = Field<B>(int_field_type_, one_tmp);
    }

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    lhs->pinned_ = true;
    QueryTable<B> *rhs = this->getChild(1)->getOutput();

    this->start_time_ = clock_start();

    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    QuerySchema out_schema = this->output_schema_;

	Logger* log = get_log();

    log->write("LHS: " + DataUtilities::printTable(lhs, -1, true), Level::INFO);
	log->write("RHS: " + DataUtilities::printTable(rhs, -1, true), Level::INFO);

	pair<QueryTable<B> *, QueryTable<B> *> augmented =  augmentTables(lhs, rhs);
    QueryTable<B> *s1, *s2;

	log->write("LHS augmented: " + DataUtilities::printTable(augmented.first, -1, true), Level::INFO);
	log->write("RHS augmented: " + DataUtilities::printTable(augmented.second, -1, true), Level::INFO);

	s1 = obliviousExpand(augmented.first, true);
	s2 = obliviousExpand(augmented.second, false);

	log->write("LHS expanded: " + DataUtilities::printTable(s1, -1, true), Level::INFO);
	log->write("RHS expanded: " + DataUtilities::printTable(s2, -1, true), Level::INFO);
    delete augmented.first;
	delete augmented.second;

    this->output_ = QueryTable<B>::getTable(max_intermediate_cardinality_, out_schema);

    size_t lhs_field_cnt = lhs_schema.getFieldCount();
    QueryTable<B> *lhs_reverted = revertProjection(s1, lhs_field_mapping_, true);
    QueryTable<B> *rhs_reverted = revertProjection(s2, rhs_field_mapping_, false);

    delete s1;
    delete s2;

    for(int i = 0; i < max_intermediate_cardinality_; i++) {
        B dummy_tag = lhs_reverted->getDummyTag(i) | rhs_reverted->getDummyTag(i);
        this->output_->cloneRow(!dummy_tag, i, 0, lhs_reverted, i);
        this->output_->cloneRow(!dummy_tag, i, lhs_field_cnt, rhs_reverted, i);
        this->output_->setDummyTag(i, dummy_tag);
    }

    delete lhs_reverted;
    delete rhs_reverted;

    lhs->pinned_ = false;
    this->output_->order_by_ = this->sort_definition_;

	log->write("Output: " + DataUtilities::printTable(this->output_, -1, true), Level::INFO);
    return this->output_;

}

template<typename B>
QuerySchema SortMergeJoin<B>::getAugmentedSchema() {

    QuerySchema augmented_schema = (lhs_smaller_) ? rhs_prime_->getSchema() : lhs_prime_->getSchema();
	
    if(!is_secure_) {
        QueryFieldDesc alpha(augmented_schema.getFieldCount(), "alpha", "", int_field_type_, 0);
        augmented_schema.putField(alpha);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field_ = Field<B>(FieldType::BOOL, true);
    }
    else {
        QueryFieldDesc alpha(augmented_schema.getFieldCount(), "alpha", "", int_field_type_);
	    int max_alpha =  this->getChild(0)->getOutputCardinality() + this->getChild(1)->getOutputCardinality();
        alpha.initializeFieldSizeWithCardinality(max_alpha);

        augmented_schema.putField(alpha);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::SECURE_BOOL);	

        augmented_schema.putField(table_id);
        table_id_field_ = Field<B>(FieldType::SECURE_BOOL, Bit(true));
    }

    augmented_schema.initializeFieldOffsets();	

	return augmented_schema;

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::unionTables(QueryTable<B> *lhs, QueryTable<B> *rhs, const QuerySchema & dst_schema) {

    auto unioned_len = lhs->tuple_cnt_ + rhs->tuple_cnt_;
    QueryTable<B> *unioned =  QueryTable<B>::getTable(unioned_len, dst_schema);

    int cursor = 0;
    for(int i = lhs->tuple_cnt_ - 1; i >= 0; --i) {
        unioned->cloneRow(cursor, 0, lhs, i);
        ++cursor;
    }


    for(int i = 0; i < rhs->tuple_cnt_; ++i) {
        unioned->cloneRow(cursor, 0, rhs, i);
        ++cursor;
    }

    return unioned;
}

template<typename B>
QuerySchema SortMergeJoin<B>::deriveAugmentedSchema() const {
    QuerySchema augmented_schema = deriveProjectedSchema();
    int write_cursor = augmented_schema.getFieldCount();
    Operator<B> *lhs_child = this->getChild(0);
    Operator<B> *rhs_child = this->getChild(1);


    if(!is_secure_) {
        QueryFieldDesc alpha1(write_cursor, "alpha1", "", int_field_type_, 0);
        augmented_schema.putField(alpha1);
        ++write_cursor;
        QueryFieldDesc alpha2(write_cursor, "alpha2", "", int_field_type_, 0);
        augmented_schema.putField(alpha2)
        ++write_cursor;
        QueryFieldDesc table_id(write_cursor, "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
    }
    else {
        QueryFieldDesc alpha1(write_cursor, "alpha1", "", int_field_type_);
        augmented_schema.putField(alpha1);
        ++write_cursor;
        QueryFieldDesc alpha2(write_cursor, "alpha2", "", int_field_type_);
        augmented_schema.putField(alpha2)
        ++write_cursor;

        alpha1.initializeFieldSizeWithCardinality(lhs_child->getOutputCardinality());
        alpha2.initializeFieldSizeWithCardinality(rhs_child->getOutputCardinality());

        QueryFieldDesc table_id(write_cursor, "table_id", "", FieldType::SECURE_BOOL);
        augmented_schema.putField(table_id);
    }
    augmented_schema.initializeFieldOffsets();

    return augmented_schema;
}

template<typename B>
QuerySchema SortMergeJoin<B>::deriveProjectedSchema() const {
    // pick bigger schema as starting point
    Operator<B> *lhs_child = this->getChild(0);
    Operator<B> *rhs_child = this->getChild(1);

    QuerySchema schema = (lhs_smaller_)  ? rhs_child->getOutputSchema() : lhs_child->getOutputSchema();
    int lhs_schema_fields = lhs_child->getOutputSchema().getFieldCount();
    vector<int> keys;
    int write_cursor = 0;
    QuerySchema projected_schema;

    for(auto key_pair : join_idxs_) {
        // visitor always outputs lhs, rhs
        int k = (lhs_smaller_) ? (key_pair.second  - lhs_schema_fields) : key_pair.first;
        QueryFieldDesc f = schema.getField(k);
        f.setOrdinal(write_cursor);
        ++write_cursor;
        projected_schema.putField(f);
        keys.emplace_back(k);
    }

    for(int i = 0; i < schema.getFieldCount(); i++) {
        if(std::find(keys.begin(), keys.end(), i) == keys.end()) {
            QueryFieldDesc f = schema.getField(i);
            f.setOrdinal(write_cursor);
            ++write_cursor;
            projected_schema.putField(f);
        }
    }

    projected_schema.initializeFieldOffsets();
    return projected_schema;
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::unionAndSortTables() {
    QuerySchema augmented_schema = deriveAugmentedSchema();
    auto unioned = unionTables(lhs_prime_, rhs_prime_, augmented_schema);

    // Set the table_id_field for distinguishing rows from lhs and rhs
    for (int i = lhs_prime_->tuple_cnt_; i < unioned->tuple_cnt_; ++i) {
        unioned->setField(i, table_id_idx_, table_id_field_);
    }

    delete lhs_prime_;
    delete rhs_prime_;

    // Sort based on join keys
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size());
    for(auto &join_idx : join_idxs_) {
        sort_def.emplace_back(join_idx.first, SortDirection::ASCENDING);  // Sorting on lhs keys
        sort_def.emplace_back(join_idx.second, SortDirection::ASCENDING); // Sorting on rhs keys
    }

    Sort<B> sorter(unioned, sort_def);
    sorter.setOperatorId(-2);

    return sorter.run()->clone();
}


// Calculate both alphas based on tid
template<typename B>
void SortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {
    Field<B> alpha = zero_;
    B one_b(true), zero_b(false);

    // Initialize alpha for each row based on join condition
    for (int i = 0; i < dst->tuple_cnt_; i++) {
        B dummy = dst->getDummyTag(i);
        B same_group = (i > 0) ? joinMatch(dst, i - 1, i) : one_b;

        // Update alpha based on the match with previous row
        alpha = Field<B>::If(same_group, Field<B>::If(!dummy, alpha + one_, alpha),
                             Field<B>::If(!dummy, one_, zero_));

        dst->setField(i, alpha_idx_, alpha);
    }

    // Copy up max alpha in reverse order to handle duplicates correctly
    for (int i = dst->tuple_cnt_ - 2; i >= 0; i--) {
        B same_group = joinMatch(dst, i, i + 1);
        B dummy = dst->getDummyTag(i);

        // Carry the alpha value from the next row if it's the same group and not a dummy
        alpha = Field<B>::If(same_group & !dummy, dst->getField(i + 1, alpha_idx_), dst->getField(i, alpha_idx_));
        dst->setField(i, alpha_idx_, alpha);
    }
}


template<typename B>
QueryTable<B> *SortMergeJoin<B>::projectJoinKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs) {

    ExpressionMapBuilder<B> builder(src->getSchema());
    int write_cursor = 0;
    map<int, int> field_mapping;

    for(auto key : join_cols) {
        builder.addMapping(key, write_cursor);
        field_mapping[write_cursor] = key;
        ++write_cursor;
    }	

    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
        if(std::find(join_cols.begin(), join_cols.end(),i) == join_cols.end()) {
            builder.addMapping(i, write_cursor);
            field_mapping[write_cursor] = i;
            ++write_cursor;
        }
    }

    Project<B> projection(src->clone(), builder.getExprs());
    projection.setOperatorId(-2);
    auto output = projection.run()->clone();

    if(is_lhs) {
        lhs_projected_schema_ = projection.getOutputSchema();
        lhs_field_mapping_ = field_mapping;
    }
    else {
        rhs_field_mapping_ = field_mapping;
        rhs_projected_schema_ = projection.getOutputSchema();
    }
    return output;
}

template<typename B>
pair<QueryTable<B> *, QueryTable<B> *>  SortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {

    assert(lhs->storageModel() == rhs->storageModel());

    vector<int> lhs_keys, rhs_keys;

    for(auto key_pair : join_idxs_) {
        // visitor always outputs lhs, rhs
        lhs_keys.emplace_back(key_pair.first);
        rhs_keys.emplace_back(key_pair.second - lhs->getSchema().getFieldCount());
    }

    lhs_prime_ = projectJoinKeyToFirstAttr(lhs, lhs_keys, true);
    rhs_prime_ = projectJoinKeyToFirstAttr(rhs, rhs_keys, false);

    // set up extended schema
    QuerySchema augmented_schema = getAugmentedSchema();

	table_id_idx_ = augmented_schema.getFieldCount() - 1;
    alpha_idx_ = augmented_schema.getFieldCount() - 2;

    QueryTable<B> sorted = unionAndSortTables();
    initializeAlphas(sorted);

    // sort by table ID followed by join key
    SortDefinition sort_def;
    sort_def.emplace_back(table_id_idx_, SortDirection::ASCENDING);
    for(int i = 0; i < join_idxs_.size(); ++i) {
        sort_def.emplace_back(i, SortDirection::ASCENDING);
    }

    Sort<B> sort_by_table_id(sorted, sort_def);
    sort_by_table_id.setOperatorId(-2);
    QueryTable<B> *sorted_table = sort_by_table_id.run();

    pair<QueryTable<B> *, QueryTable<B> *> output;
    //split union table into S1 and S2
    output.first = QueryTable<B>::getTable(lhs->tuple_cnt_, augmented_schema);
    output.second = QueryTable<B>::getTable(rhs->tuple_cnt_, augmented_schema);

    // copy over first |s1| col entries from sorted_table to s1
    // remaining ones go to s2
    int read_offset = output.first->tuple_cnt_;
    int field_cnt = augmented_schema.getFieldCount();

    for(int i = 0; i < field_cnt; ++i) {
        output.first->cloneColumn(i, 0, sorted_table, i);
        output.second->cloneColumn(i, 0, sorted_table, i, read_offset);
    }
    output.first->cloneColumn(-1, 0, sorted_table, -1);
    output.second->cloneColumn(-1, 0, sorted_table, -1, read_offset);


    return output;

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousDistribute(QueryTable<B> *input, size_t target_size) {
    return nullptr;

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousExpand(QueryTable<B> *input, bool is_lhs) {
    return nullptr;
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::revertProjection(QueryTable<B> *src, const map<int, int> &expr_map,
                                                       const bool &is_lhs) const {
    return nullptr;
}

template class vaultdb::SortMergeJoin<bool>;
template class vaultdb::SortMergeJoin<emp::Bit>;
