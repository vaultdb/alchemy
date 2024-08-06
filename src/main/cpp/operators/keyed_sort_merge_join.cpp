#include "operators/keyed_sort_merge_join.h"
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

// lhs assumed to be fkey
template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key,  Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(foreign_key, primary_key, predicate, sort) {
    this->output_cardinality_ = foreign_key->getOutputCardinality();
    setup();
}


template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate,
                                const SortDefinition &sort)  : Join<B>(foreign_key, primary_key, predicate, sort) {
    this->output_cardinality_ = foreign_key->tuple_cnt_;
    setup();
}


template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey != 0) {

    assert(fkey == 0 || fkey == 1);	
    this->output_cardinality_ = (fkey == 0) ? lhs->getOutputCardinality() : rhs->getOutputCardinality();
    setup();
}


template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
              const SortDefinition &sort)  : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey != 0) {

    assert(fkey == 0 || fkey == 1);
    this->output_cardinality_ = (fkey == 0) ? lhs->tuple_cnt_ : rhs->tuple_cnt_;
    setup();
}


template<typename B>
void KeyedSortMergeJoin<B>::setup() {
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

    max_intermediate_cardinality_ =  this->getChild(0)->getOutputCardinality() + this->getChild(1)->getOutputCardinality();

    foreign_key_cardinality_ =  this->getChild(foreign_key_input_)->getOutputCardinality();

    if(is_secure_) {
        int card_bits = ceil(log2(max_intermediate_cardinality_)) + 1; // + 1 for sign bit
        emp::Integer zero_tmp(card_bits, 0, emp::PUBLIC);
        emp::Integer one_tmp(card_bits, 1, emp::PUBLIC);
        zero_ = Field<B>(int_field_type_, zero_tmp);
        one_ = Field<B>(int_field_type_, one_tmp);
    }

}

template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    lhs->pinned_ = true;
    QueryTable<B> *rhs = this->getChild(1)->getOutput();


    this->start_time_ = clock_start();
    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    QuerySchema out_schema = this->output_schema_;

    pair<QueryTable<B> *, QueryTable<B> *> augmented =  augmentTables(lhs, rhs);
    QueryTable<B> *expanded_lhs, *expanded_rhs;

    expanded_lhs = expand(augmented.first);
    expanded_rhs = expand(augmented.second);

    delete augmented.first;
	delete augmented.second;

    size_t lhs_field_cnt = lhs_schema.getFieldCount();
    size_t rhs_field_cnt = rhs_schema.getFieldCount();
    QueryTable<B> *lhs_reverted = revertProjection(expanded_lhs, lhs_field_mapping_, true);
    QueryTable<B> *rhs_reverted = revertProjection(expanded_rhs, rhs_field_mapping_, false);

    delete expanded_lhs;
    delete expanded_rhs;

    this->output_ = QueryTable<B>::getTable(foreign_key_cardinality_, out_schema);

    for(int i = 0; i < lhs_field_cnt; ++i)
        this->output_->cloneColumn(i, 0, lhs_reverted, i, 0);

    for(int i = 0; i < rhs_field_cnt; ++i)
        this->output_->cloneColumn(i + lhs_field_cnt, 0, rhs_reverted, i, 0);


    for(int i = 0; i < foreign_key_cardinality_; i++) {
        B dummy = lhs_reverted->getDummyTag(i) | rhs_reverted->getDummyTag(i);
        this->output_->setDummyTag(i, dummy);
    }

    delete lhs_reverted;
    delete rhs_reverted;

    lhs->pinned_ = false;
    this->output_->order_by_ = this->sort_definition_;
    return this->output_;
}

template<typename B>
QuerySchema KeyedSortMergeJoin<B>::deriveProjectedSchema() const {
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
QuerySchema KeyedSortMergeJoin<B>::deriveAugmentedSchema() const {
    QuerySchema augmented_schema = deriveProjectedSchema();
    int write_cursor = augmented_schema.getFieldCount();
    Operator<B> *lhs_child = this->getChild(0);
    Operator<B> *rhs_child = this->getChild(1);


    if(!is_secure_) {
        QueryFieldDesc alpha(write_cursor, "alpha", "", int_field_type_, 0);
        augmented_schema.putField(alpha);
        ++write_cursor;
        QueryFieldDesc table_id(write_cursor, "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
    } else {
        QueryFieldDesc alpha(write_cursor, "alpha", "", int_field_type_);
        int max_alpha =  lhs_child->getOutputCardinality() + rhs_child->getOutputCardinality();
        alpha.initializeFieldSizeWithCardinality(max_alpha);
        ++write_cursor;

        augmented_schema.putField(alpha);
        QueryFieldDesc table_id(write_cursor, "table_id", "", FieldType::SECURE_BOOL);
        augmented_schema.putField(table_id);
    }
    augmented_schema.initializeFieldOffsets();

    return augmented_schema;
}

template<typename B>
QuerySchema KeyedSortMergeJoin<B>::getAugmentedSchema() {

    QuerySchema augmented_schema = (lhs_smaller_) ? rhs_prime_->getSchema() : lhs_prime_->getSchema();
	
    if(!is_secure_) {
        QueryFieldDesc alpha(augmented_schema.getFieldCount(), "alpha", "", int_field_type_, 0);
        augmented_schema.putField(alpha);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field_ = Field<B>(FieldType::BOOL, true);
    } else {
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
pair<QueryTable<B> *, QueryTable<B> *>  KeyedSortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {
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

    auto sorted = sortCompatible() ? unionAndMergeTables() : unionAndSortTables();
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
QueryTable<B> *KeyedSortMergeJoin<B>::unionAndSortTables() {
    QuerySchema augmented_schema = deriveAugmentedSchema();
    int unioned_len = lhs_prime_->tuple_cnt_ + rhs_prime_->tuple_cnt_;
    auto unioned = unionTables(lhs_prime_, rhs_prime_, augmented_schema);
    int cursor = lhs_prime_->tuple_cnt_;

    while(cursor < unioned_len) {
        unioned->setField(cursor, table_id_idx_, table_id_field_);
        ++cursor;
    }

    delete lhs_prime_;
    delete rhs_prime_;

    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size()); // join keys

    bool lhs_is_foreign_key = (foreign_key_input_ == 0);
    // sort s.t. fkey entries are first, pkey entries are second
    sort_def.emplace_back(table_id_idx_, lhs_is_foreign_key ? SortDirection::ASCENDING : SortDirection::DESCENDING);

    Sort<B> sorter(unioned, sort_def);
    sorter.setOperatorId(-2);

   return sorter.run()->clone();
}

template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::unionAndMergeTables() {
    QuerySchema augmented_schema = deriveAugmentedSchema();
    int unioned_len = lhs_prime_->tuple_cnt_ + rhs_prime_->tuple_cnt_;
    bool lhs_is_foreign_key = (foreign_key_input_ == 0);
    // always FK --> PK
    QueryTable<B> *unioned;
    if(lhs_is_foreign_key) {
        unioned = unionTables(lhs_prime_, rhs_prime_, augmented_schema);
        int cursor = lhs_prime_->tuple_cnt_;
        while(cursor < unioned_len) {
            unioned->setField(cursor, table_id_idx_, table_id_field_); // NYI: add batch write method for this
            ++cursor;
        }
    } else {
        unioned = unionTables(rhs_prime_, lhs_prime_, augmented_schema);
        int cursor = 0;

        while (cursor < rhs_prime_->tuple_cnt_) {
            unioned->setField(cursor, table_id_idx_, table_id_field_);
            ++cursor;
        }

    }

    delete lhs_prime_;
    delete rhs_prime_;
    int counter = 0;

    // do bitonic merge instead of full sort
    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size()); // join keys
    // sort s.t. fkey entries are first, pkey entries are second

    sort_def.emplace_back(table_id_idx_, lhs_is_foreign_key ? SortDirection::ASCENDING : SortDirection::DESCENDING);

    Sort<B> sorter(unioned, sort_def);
    sorter.setOperatorId(-2);
    auto normalized = sorter.normalizeTable(unioned); // normalize to move up table_id field
    sorter.bitonicMerge(normalized, sorter.getSortOrder(), 0, normalized->tuple_cnt_, true, counter);
    auto collated_union =  sorter.denormalizeTable(normalized);
    collated_union->order_by_ = sort_def;

    delete normalized;
    return collated_union;

}

template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::unionTables(QueryTable<B> *lhs, QueryTable<B> *rhs, const QuerySchema & dst_schema) {

    auto unioned_len = lhs->tuple_cnt_ + rhs->tuple_cnt_;
    auto unioned =  QueryTable<B>::getTable(unioned_len, dst_schema);

    // always FK --> PK
    // NYI: add conditional so we only reshuffle columns for smaller relation
    SystemConfiguration *sys_conf = &SystemConfiguration::getInstance();
    int cursor = 0;
    // Initialize non-key columns in unioned to make sure we have pages in the buffer pool.
    if(sys_conf->bp_enabled_) {
        for(int i = join_idxs_.size(); i < unioned->getSchema().getFieldCount(); ++i) {
            unioned->cloneColumn(i, -1, unioned, i);
        }
    }

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
QueryTable<B> *KeyedSortMergeJoin<B>::projectJoinKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs) {

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


    if(is_lhs) {
        lhs_projected_schema_ = projection.getOutputSchema();
        lhs_field_mapping_ = field_mapping;
    } else {
        rhs_field_mapping_ = field_mapping;
        rhs_projected_schema_ = projection.getOutputSchema();
    }

    auto output = projection.run();
    return output->clone();
}

template<typename B>
void KeyedSortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {

    B one_b(true), zero_b(false);

	B table_id = dst->getField(0, table_id_idx_).template getValue<B>();
    B fkey_check = ((!foreign_key_input_) ? zero_b : one_b);
	B is_foreign_key = (table_id == fkey_check);
    B dummy = dst->getDummyTag(0);
    B same_group;

    // initialize
    Field<B> alpha_1 = Field<B>::If(is_foreign_key & !dummy, one_, zero_);
    Field<B> alpha_2 = Field<B>::If(!is_foreign_key & !dummy, one_, zero_);

    dst->setField(0, alpha_idx_, Field<B>::If(is_foreign_key, alpha_2, alpha_1));

    // rhs has table_id = true
    // NYI: we can likely cut down on our circuits if we say table_id = true is always fkey (or pkey)
    for(int i = 1; i < dst->tuple_cnt_; i++) {
		table_id = dst->getField(i, table_id_idx_).template getValue<B>();
		is_foreign_key = (table_id == fkey_check);
        dummy = dst->getDummyTag(i);

		same_group = joinMatch(dst,  i-1, i);
        Field<B> incr = Field<B>::If(is_foreign_key & !dummy, alpha_1 + one_, alpha_1);
        Field<B> reset = Field<B>::If(is_foreign_key & !dummy, one_, zero_);
		alpha_1 = Field<B>::If(same_group, incr, reset);

        incr =  Field<B>::If(!is_foreign_key, alpha_2 + one_, zero_);
        reset = Field<B>::If(!is_foreign_key, one_, zero_);
		alpha_2 = Field<B>::If(same_group, incr, reset);

        Field<B> to_write = Field<B>::If(is_foreign_key, alpha_2, alpha_1);
        dst->setField(i, alpha_idx_, to_write);
	}

    int last_idx = dst->tuple_cnt_ - 1;

    table_id = dst->getField(last_idx, table_id_idx_).template getValue<B>();
    is_foreign_key = (table_id == fkey_check);
    same_group = one_b;
    dummy = dst->getDummyTag(last_idx);

    // inputs are sorted fkey --> pkey
    //if we are on the last entry and it is fkey, then we know there are no matches so we set alpha_2 to 0
    alpha_1 = Field<B>::If(is_foreign_key, zero_, Field<B>::If(!is_foreign_key & !dummy, one_, zero_));
    dst->setField(last_idx, alpha_idx_, Field<B>::If(is_foreign_key, alpha_1, dst->getField(last_idx, alpha_idx_)));
    dst->setField(last_idx, alpha_idx_, Field<B>::If(dst->getDummyTag(last_idx), zero_, dst->getField(last_idx, alpha_idx_)));

    // copy up max alpha
	for(int i = last_idx-1; i >= 0; i--) {
        table_id = dst->getField(i, table_id_idx_).template getValue<B>();
		is_foreign_key = (table_id == fkey_check);
		B same_group = joinMatch(dst,  i+1, i);
        B dummy = dst->getDummyTag(i);

		alpha_1 = Field<B>::If(is_foreign_key & same_group, alpha_1, Field<B>::If(!is_foreign_key & !dummy, one_, zero_));
		dst->setField(i, alpha_idx_, Field<B>::If(is_foreign_key, alpha_1, dst->getField(i, alpha_idx_)));
		dst->setField(i, alpha_idx_, Field<B>::If(dst->getDummyTag(i), zero_, dst->getField(i, alpha_idx_)));
    }

}




template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::distribute(QueryTable<B> *input, size_t target_size) {
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
QueryTable<B> *KeyedSortMergeJoin<B>::expand(QueryTable<B> *input) {
    QueryTable<B> *intermediate_table = input->clone();
    weight_idx_ = input->getSchema().getFieldCount();
    is_new_idx_ = weight_idx_ + 1;

    QueryFieldDesc weight(weight_idx_, "weight", "", int_field_type_);
    if(is_secure_) weight.initializeFieldSizeWithCardinality(max_intermediate_cardinality_);
    intermediate_table->appendColumn(weight);

    QueryFieldDesc is_new(is_new_idx_, "is_new", "", bool_field_type_);
    intermediate_table->appendColumn(is_new);


    Field<B> s = one_;
    Field<B> one_b(bool_field_type_, B(true)), zero_b(bool_field_type_, B(false));

    for(int i = 0; i < input->tuple_cnt_; i++) {
        Field<B> cnt = input->getField(i, alpha_idx_); // cnt = alpha
        B null_val = (cnt == zero_); // is count empty?  If no, set weight to running count, else set weight to zero
        intermediate_table->setField(i, weight_idx_,
                                     Field<B>::If(null_val, zero_, s));
        intermediate_table->setField(i, is_new_idx_,
                                     Field<B>::If(null_val, one_b, zero_b));

        intermediate_table->setDummyTag(i, input->getDummyTag(i) | null_val);
        s = s + cnt;
    }

    QueryTable<B> *dst_table = distribute(intermediate_table, foreign_key_cardinality_);

    auto schema = dst_table->getSchema();
    // creates a row with self-managed memory
    QueryTuple<B> tmp_row = dst_table->getRow(0);

    tmp_row.setField(is_new_idx_, one_b);
    int weight_width = schema.getField(weight_idx_).size() + schema.getField(weight_idx_).bitPacked();
    Field<B> bound = s - one_;

    for(int i = 0; i < foreign_key_cardinality_; i++) {

        B new_row = dst_table->getField(i, is_new_idx_).template getValue<B>();
        // if it is a new row, copy down previous value
        QueryTuple<B> table_row = dst_table->getRow(i);
        tmp_row = QueryTuple<B>::If(new_row, tmp_row, table_row);

        dst_table->setRow(i, tmp_row);
        dst_table->setField(i, is_new_idx_, zero_b);

        Field<B> write_index = FieldFactory<B>::getInt(i, weight_width);
        B end_matches = (write_index >= bound);
        B dummy_tag = tmp_row.getDummyTag()| end_matches;
        dst_table->setDummyTag(i, dummy_tag);
    }
    return dst_table;
}


template<>
QueryTable<bool> *KeyedSortMergeJoin<bool>::revertProjection(QueryTable<bool> *src, const map<int, int> &expr_map,
                                                       const bool &is_lhs) const {
    QueryTable<bool> *to_project;

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
        int tuple_cnt = src->tuple_cnt_;
        to_project = QueryTable<bool>::getTable(tuple_cnt, synthetic_schema);

        for(int i = 0; i < tuple_cnt; ++i) {
            auto r = src->serializeRow(i);
            to_project->deserializeRow(i, r);
        }
    } else {
        to_project = src->clone();
    }

    ExpressionMapBuilder<bool> builder(src->getSchema());
    for(auto pos : expr_map) {
        builder.addMapping(pos.first, pos.second);
    }

    Project<bool> projection(to_project, builder.getExprs());
    projection.setOperatorId(-2);
    return projection.run()->clone();

}

// if we are in OMPC packed wires mode,
// then we need to serialize each row into a bit array using unpackRow, then manually project it to the correct offsets.*/
template<>
QueryTable<Bit> *KeyedSortMergeJoin<Bit>::revertProjection(QueryTable<Bit> *src, const map<int, int> &expr_map,
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

    auto row_len = running_offset + 1;
    auto row_cnt = src->tuple_cnt_;
    auto dst =  QueryTable<Bit>::getTable(row_cnt, dst_schema);

    Integer dst_row(row_len, 0);

    for(int i = 0; i < row_cnt; ++i) {
        auto unpacked = src->unpackRow(i); // NYI: only unpack the cols we need
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


template class vaultdb::KeyedSortMergeJoin<bool>;
template class vaultdb::KeyedSortMergeJoin<emp::Bit>;

