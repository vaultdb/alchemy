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
#include <iostream>

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

    max_intermediate_cardinality_ =  this->getChild(0)->getOutputCardinality();// * this->getChild(1)->getOutputCardinality();

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

    pair<QueryTable<B> *, QueryTable<B> *> augmented =  augmentTables(lhs, rhs);
    QueryTable<B> *expanded_lhs, *expanded_rhs;

    expanded_lhs = expand(augmented.first, true)->clone();
    expanded_rhs = expand(augmented.second, false)->clone();

    delete augmented.first;
    delete augmented.second;

    QueryTable<B> *aligned_rhs = alignTable(expanded_rhs)->clone();

    size_t lhs_field_cnt = lhs_schema.getFieldCount();
    QueryTable<B> *lhs_reverted = revertProjection(expanded_lhs, lhs_field_mapping_, true)->clone();
    QueryTable<B> *rhs_reverted = revertProjection(aligned_rhs, rhs_field_mapping_, false)->clone();

    delete expanded_rhs;
    delete expanded_lhs;

    this->output_ = QueryTable<B>::getTable(max_intermediate_cardinality_, out_schema);

    for(int i = 0; i < rhs_reverted->tuple_cnt_; i++) {
        B dummy_tag = lhs_reverted->getDummyTag(i) | rhs_reverted->getDummyTag(i);
        this->output_->cloneRow(!dummy_tag, i, 0, lhs_reverted, i);
        this->output_->cloneRow(!dummy_tag, i, lhs_field_cnt, rhs_reverted, i);
        this->output_->setDummyTag(i, dummy_tag);
    }

    delete lhs_reverted;
    delete rhs_reverted;

    lhs->pinned_ = false;
    this->output_->order_by_ = this->sort_definition_;

    return this->output_;
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
QuerySchema SortMergeJoin<B>::deriveAugmentedSchema() const {
    QuerySchema augmented_schema = deriveProjectedSchema();
    int write_cursor = augmented_schema.getFieldCount();
    Operator<B> *lhs_child = this->getChild(0);
    Operator<B> *rhs_child = this->getChild(1);

    QueryFieldDesc alpha1(write_cursor++, "alpha1", "", int_field_type_, 0);
    QueryFieldDesc alpha2(write_cursor++, "alpha2", "", int_field_type_, 0);
    QueryFieldDesc table_id(write_cursor, "table_id", "", bool_field_type_, 0);

    // Initialize field size with maximum cardinality for secure contexts
    if (is_secure_) {
        int max_alpha = lhs_child->getOutputCardinality() + rhs_child->getOutputCardinality();
        alpha1.initializeFieldSizeWithCardinality(max_alpha);
        alpha2.initializeFieldSizeWithCardinality(max_alpha);
    }

    // Add fields to the schema
    augmented_schema.putField(alpha1);
    augmented_schema.putField(alpha2);
    augmented_schema.putField(table_id);

    augmented_schema.initializeFieldOffsets();
    return augmented_schema;
}


template<typename B>
QuerySchema SortMergeJoin<B>::getAugmentedSchema() {
    QuerySchema augmented_schema = (lhs_smaller_) ? rhs_prime_->getSchema() : lhs_prime_->getSchema();

    if(!is_secure_) {
        QueryFieldDesc alpha1(augmented_schema.getFieldCount(), "alpha1", "", int_field_type_, 0);
        augmented_schema.putField(alpha1);
        QueryFieldDesc alpha2(augmented_schema.getFieldCount(), "alpha2", "", int_field_type_, 0);
        augmented_schema.putField(alpha2);

        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field_ = Field<B>(FieldType::BOOL, true);
    } else {
        int max_alpha = this->getChild(0)->getOutputCardinality() + this->getChild(1)->getOutputCardinality();

        QueryFieldDesc alpha1(augmented_schema.getFieldCount(), "alpha1", "", int_field_type_);
        alpha1.initializeFieldSizeWithCardinality(max_alpha);
        QueryFieldDesc alpha2(augmented_schema.getFieldCount() + 1, "alpha2", "", int_field_type_);
        alpha2.initializeFieldSizeWithCardinality(max_alpha);

        augmented_schema.putField(alpha1);
        augmented_schema.putField(alpha2);

        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::SECURE_BOOL);
        augmented_schema.putField(table_id);
        table_id_field_ = Field<B>(FieldType::SECURE_BOOL, Bit(true));
    }

    augmented_schema.initializeFieldOffsets();

    return augmented_schema;
}

template<typename B>
pair<QueryTable<B> *, QueryTable<B> *>  SortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {
    assert(lhs->storageModel() == rhs->storageModel());

    vector<int> lhs_keys, rhs_keys;

    for(auto key_pair : join_idxs_) {
        lhs_keys.emplace_back(key_pair.first);
        rhs_keys.emplace_back(key_pair.second - lhs->getSchema().getFieldCount());
    }

    lhs_prime_ = projectJoinKeyToFirstAttr(lhs, lhs_keys, true);
    rhs_prime_ = projectJoinKeyToFirstAttr(rhs, rhs_keys, false);


    // set up extended schema
    QuerySchema augmented_schema = getAugmentedSchema();

    table_id_idx_ = augmented_schema.getFieldCount() - 1;
    alpha2_idx_ = augmented_schema.getFieldCount() - 2;
    alpha1_idx_ = augmented_schema.getFieldCount() - 3;

    auto sorted = unionAndSortTables();

    // Calculate count of join keys in LHS and RHS tables
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
QueryTable<B> *SortMergeJoin<B>::unionAndSortTables() {

    QuerySchema augmented_schema = deriveAugmentedSchema();
    int unioned_len = lhs_prime_->tuple_cnt_ + rhs_prime_->tuple_cnt_;

    auto unioned = unionTables(lhs_prime_, rhs_prime_, augmented_schema);

    // Assign table_id to the right hand side of the union
    for (int cursor = lhs_prime_->tuple_cnt_; cursor < unioned_len; ++cursor) {
        unioned->setField(cursor, table_id_idx_, table_id_field_);
    }

    delete lhs_prime_;
    delete rhs_prime_;

    // Define the sort definition based on join keys
    SortDefinition sort_def;
    sort_def.emplace_back(0, SortDirection::ASCENDING); // Sorting by join key

    // Add table_id as a secondary sort key
    sort_def.emplace_back(table_id_idx_, SortDirection::ASCENDING);

    // Execute the sort
    Sort<B> sorter(unioned, sort_def);
    sorter.setOperatorId(-2);

    return sorter.run()->clone();
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::unionTables(QueryTable<B> *lhs, QueryTable<B> *rhs, const QuerySchema & dst_schema) {

    auto unioned_len = lhs->tuple_cnt_ + rhs->tuple_cnt_;
    auto unioned =  QueryTable<B>::getTable(unioned_len, dst_schema);

    int cursor = 0;
    for(int i = lhs->tuple_cnt_ - 1; i >= 0; --i) {
        auto r = lhs->serializeRow(i);
        unioned->deserializeRow(cursor, r);
        ++cursor;

    }

    for(int i = 0; i < rhs->tuple_cnt_; ++i) {
        auto r = rhs->serializeRow(i);
        unioned->deserializeRow(cursor, r);
        ++cursor;
    }
    return unioned;
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
void SortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {

    B one_b(true), zero_b(false);

    B table_id = dst->getField(0, table_id_idx_).template getValue<B>();
    B dummy = dst->getDummyTag(0);
    B same_group;

    // initialize join key count for LHS (Alpha 1) and RHS (Alpha 2)
    Field<B> alpha_1 = Field<B>::If(!table_id & !dummy, one_, zero_);
    Field<B> alpha_2 = Field<B>::If(table_id & !dummy, one_, zero_);

    dst->setField(0, alpha1_idx_, alpha_1);
    dst->setField(0, alpha2_idx_, alpha_2);

    // Iterate and count occurances
    for(int i = 1; i < dst->tuple_cnt_; i++) {
        table_id = dst->getField(i, table_id_idx_).template getValue<B>();
        dummy = dst->getDummyTag(i);

        same_group = joinMatch(dst,  i-1, i);
        Field<B> incr = Field<B>::If(!table_id & !dummy, alpha_1 + one_, alpha_1);
        Field<B> reset = Field<B>::If(!table_id & !dummy, one_, zero_);
        alpha_1 = Field<B>::If(same_group, incr, reset);

        incr =  Field<B>::If(table_id, alpha_2 + one_, zero_);
        reset = Field<B>::If(table_id, one_, zero_);
        alpha_2 = Field<B>::If(same_group, incr, reset);

        dst->setField(i, alpha1_idx_, alpha_1);
        dst->setField(i, alpha2_idx_, alpha_2);
    }

    int last_idx = dst->tuple_cnt_ - 1;

    Field<B> back_prop_alpha_1 = dst->getField(0, alpha1_idx_);
    Field<B> back_prop_alpha_2 = dst->getField(0, alpha1_idx_);

    // Back propagate max alpha in LHS and RHS for each join key
    for (int i = last_idx - 1; i >= 0; --i) {
        same_group = joinMatch(dst,  i+1, i);

        back_prop_alpha_1 = Field<B>::If(!same_group, dst->getField(i, alpha1_idx_), back_prop_alpha_1);
        back_prop_alpha_2 = Field<B>::If(!same_group, dst->getField(i, alpha2_idx_), back_prop_alpha_2);

        dst->setField(i, alpha1_idx_, back_prop_alpha_1);
        dst->setField(i, alpha2_idx_, back_prop_alpha_2);
    }

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::distribute(QueryTable<B> *input, size_t target_size) {

    Logging::Logger* log = Logging::get_log();

    QuerySchema schema = input->getSchema();

    SortDefinition sort_def{ ColumnSort(is_new_idx_, SortDirection::ASCENDING), ColumnSort(weight_idx_, SortDirection::ASCENDING)};
    Sort<B> sorted(input, sort_def);
    sorted.setOperatorId(-2);
    auto dst_table = sorted.run();
    int base_card = dst_table->tuple_cnt_;
    dst_table->resize(target_size);

    Field<B> table_idx = dst_table->getField(0, table_id_idx_);
    Field<B> one_b(bool_field_type_, B(true));


    for(int i = base_card; i < target_size; i++) {
        dst_table->setField(i, is_new_idx_, one_b);
        dst_table->setField(i, table_id_idx_, table_idx);
    }

    int j = Sort<B>::powerOfTwoLessThan(target_size);
    int weight_width = (is_secure_) ? zero_.getInt().size() : 32;

    while(j >= 1) {
        for(int i = target_size - j - 1; i >= 0; i--) {

            Field<B> weight = dst_table->getField(i, weight_idx_);
            Field<B> pos = FieldFactory<B>::getInt(i + j + 1, weight_width);
            B result = (weight >= pos);
            dst_table->compareSwap(result, i, i+j);

        }
        j = j/2;
    }

    return dst_table->clone();
}


template<typename B>
QueryTable<B> *SortMergeJoin<B>::expand(QueryTable<B> *input, bool is_lhs) {

    // Sort dummies to end to prevent offset error

    // Sort individual table dummies to end before expand
    SortDefinition sort_def;
    sort_def.emplace_back(-1, SortDirection::ASCENDING);
    sort_def.emplace_back(0, SortDirection::ASCENDING);
    Sort<B> sort_by_dummy(input, sort_def);
    sort_by_dummy.setOperatorId(-2);
    QueryTable<B> *intermediate_table = sort_by_dummy.run()->clone();
    //QueryTable<B> *intermediate_table = input->clone();

    weight_idx_ = input->getSchema().getFieldCount();
    is_new_idx_ = weight_idx_ + 1;

    // Append new columns for weight and is_new
    QueryFieldDesc weight(weight_idx_, "weight", "", int_field_type_);
    if(is_secure_) weight.initializeFieldSizeWithCardinality(max_intermediate_cardinality_);
    intermediate_table->appendColumn(weight);

    QueryFieldDesc is_new(is_new_idx_, "is_new", "", bool_field_type_);
    intermediate_table->appendColumn(is_new);

    Field<B> s = one_;
    Field<B> one_b(bool_field_type_, B(true)), zero_b(bool_field_type_, B(false));

    // Iterate and set fields based on alpha values
    for(int i = 0; i < input->tuple_cnt_; i++) {

        // Allocate alpha1 * alpha2 rows for new table
        Field<B> cnt1 = intermediate_table->getField(i, alpha1_idx_);
        Field<B> cnt2 = intermediate_table->getField(i, alpha2_idx_);
        Field<B> total_cnt;

        total_cnt = is_lhs ? cnt2 : cnt1;

        B null_val = (total_cnt == zero_);

        intermediate_table->setField(i, weight_idx_, Field<B>::If(null_val, zero_, s));
        intermediate_table->setField(i, is_new_idx_, Field<B>::If(null_val, one_b, zero_b));
        intermediate_table->setDummyTag(i, input->getDummyTag(i) | null_val);
        s = s + total_cnt;
    }

    QueryTable<B> *dst_table = distribute(intermediate_table, max_intermediate_cardinality_);

    auto schema = dst_table->getSchema();
    // creates a row with self-managed memory
    QueryTuple<B> tmp_row = dst_table->getRow(0);

    tmp_row.setField(is_new_idx_, one_b);
    int weight_width = schema.getField(weight_idx_).size() + schema.getField(weight_idx_).bitPacked();
    Field<B> bound = s - one_;

    for(int i = 0; i < max_intermediate_cardinality_; i++) {

        B new_row = dst_table->getField(i, is_new_idx_).template getValue<B>();
        // if it is a new row, copy down previous value
        QueryTuple<B> table_row = dst_table->getRow(i);
        tmp_row = QueryTuple<B>::If(new_row, tmp_row, table_row);

        dst_table->setRow(i, tmp_row);
        dst_table->setField(i, is_new_idx_, zero_b);

        Field<B> write_index = FieldFactory<B>::getInt(i, weight_width);
        B end_matches = (write_index >= bound);
        B dummy_tag = tmp_row.getDummyTag() | end_matches;
        dst_table->setDummyTag(i, dummy_tag);
    }
    return dst_table;
}


template<>
QueryTable<bool> *SortMergeJoin<bool>::revertProjection(QueryTable<bool> *src, const map<int, int> &expr_map,
                                                        const bool &is_lhs) const {

    // create a synthetic schema.  pad it to make it the "right" row length for projection
    // for use with smaller width row
    if(lhs_smaller_ == is_lhs) {
        QuerySchema synthetic_schema = (is_lhs) ? lhs_projected_schema_ : rhs_projected_schema_;
        int delta = src->getSchema().size() - synthetic_schema.size();
        if(delta > 0) {
            QueryFieldDesc synthetic_attr(synthetic_schema.getFieldCount(), "anon", "",
                                          is_secure_ ? FieldType::SECURE_STRING : FieldType::STRING,
                                          delta / 8);
            synthetic_schema.putField(synthetic_attr);
            delta -= synthetic_attr.getStringLength() * 8;
        }

        while(delta > 0) { // only not byte-aligned in secure mode
            QueryFieldDesc f(synthetic_schema.getFieldCount(), "anon", "",  FieldType::SECURE_BOOL,0);
            synthetic_schema.putField(f);
            --delta;
        }

        synthetic_schema.initializeFieldOffsets();
        src->setSchema(synthetic_schema);

    }

    ExpressionMapBuilder<bool> builder(src->getSchema());
    for(auto pos : expr_map) {
        builder.addMapping(pos.first, pos.second);
    }


    Project<bool> projection(src->clone(), builder.getExprs());
    projection.setOperatorId(-2);

    return projection.run()->clone();

}

// if we are in OMPC packed wires mode,
// then we need to serialize each row into a bit array using unpackRow, then manually project it to the correct offsets.*/
template<>
QueryTable<Bit> *SortMergeJoin<Bit>::revertProjection(QueryTable<Bit> *src, const map<int, int> &expr_map,
                                                      const bool &is_lhs) const {
    map<int, int> src_field_offsets_bits;
    map<int, int> dst_to_src;
    int child_id = is_lhs ? 0 : 1;
    QuerySchema dst_schema = this->getChild(child_id)->getOutputSchema();
    QuerySchema src_schema = (is_lhs) ? lhs_projected_schema_ : rhs_projected_schema_;
    size_t running_offset = 0;

    for(int i = 0; i < src_schema.getFieldCount(); ++i) {
        src_field_offsets_bits[i] =  running_offset;
        QueryFieldDesc f = src_schema.getField(i);
        running_offset += f.size();
    }

    src_field_offsets_bits[-1] = running_offset; // dummy tag

    for(auto pos : expr_map) {
        dst_to_src[pos.second] = pos.first;
    }

    // if we are in OMPC packed wires mode, then we need to unpackRow into a bit array using unpackRow, then manually project the bit array to the correct offsets.
    auto row_len = running_offset + 1;
    auto row_cnt = src->tuple_cnt_;
    auto dst =  QueryTable<Bit>::getTable(row_cnt, dst_schema);

    Integer dst_row(row_len, 0);

    for(int i = 0; i < row_cnt; ++i) {
        auto unpacked = src->unpackRow(i); // TODO: only unpack the cols we need
        Bit *write_ptr = dst_row.bits.data();
        for(int j = 0; j < dst_schema.getFieldCount(); ++j) {
            int src_ordinal = dst_to_src[j];
            int src_offset = src_field_offsets_bits.at(src_ordinal);
            Bit *read_ptr = unpacked.bits.data() + src_offset;
            int write_size = dst_schema.getField(j).size();
            memcpy(write_ptr, read_ptr, write_size * sizeof(emp::Bit));
            write_ptr += write_size;
        }
        // copy out dummy tag
        *(dst_row.bits.data() + dst_row.size() - 1) = unpacked[unpacked.size() - 1];
        // if this table has the smaller of the two input schemas, then the dummy tag is at the end.
        dst->packRow(i, dst_row);
    }
    return  dst;
}

template<typename B>
QueryTable<B>* SortMergeJoin<B>::alignTable(QueryTable<B> *input) {

    // Update input schema with new idx col
    QuerySchema schema = input->getSchema();
    QueryFieldDesc new_idx_field(schema.getFieldCount(), "new_idx", "", int_field_type_, 0);
    if (is_secure_) new_idx_field.initializeFieldSizeWithCardinality(max_intermediate_cardinality_);
    schema.putField(new_idx_field);
    schema.initializeFieldOffsets();
    new_idx_col = input->getSchema().getFieldCount();
    input->appendColumn(new_idx_field);
    QueryTable<B> *intermediate_table = input->clone();

    // Calculate position of first row
    Field<B> q = zero_;
    B same_group;
    Field<B> alpha1 = intermediate_table->getField(0, alpha1_idx_);
    Field<B> alpha2 = intermediate_table->getField(0, alpha2_idx_);
    Field<B> new_idx = ((q / alpha1) + (q % alpha1) * alpha2);
    intermediate_table->setField(0, new_idx_col, new_idx);

    // Iterate through the table to calculate the new positions
    for (int i = 1; i < intermediate_table->tuple_cnt_; i++) {

        same_group = joinMatch(intermediate_table, i-1, i);

        // Reset q if the join key has changed
        q = Field<B>::If(!same_group, zero_, q + one_);

        // Compute the new position based on alpha values
        alpha1 = intermediate_table->getField(i, alpha1_idx_);
        alpha2 = intermediate_table->getField(i, alpha2_idx_);

        new_idx = ((q / alpha1) + (q % alpha1) * alpha2);

        // Store the computed new index in the new column
        intermediate_table->setField(i, new_idx_col, new_idx);
    }

    // Align Sort by dummy tag, join key, and the new_idx column
    SortDefinition sort_def;
    sort_def.emplace_back(-1, SortDirection::ASCENDING); // Sort dummy vals to end
    sort_def.emplace_back(0, SortDirection::ASCENDING); // Join key
    sort_def.emplace_back(new_idx_col, SortDirection::ASCENDING); // New idx col
    Sort<B> sort_new_col(intermediate_table, sort_def);
    sort_new_col.setOperatorId(-2);
    QueryTable<B> *sorted_table = sort_new_col.run()->clone();

    return sorted_table;
}


template class vaultdb::SortMergeJoin<bool>;
template class vaultdb::SortMergeJoin<emp::Bit>;
