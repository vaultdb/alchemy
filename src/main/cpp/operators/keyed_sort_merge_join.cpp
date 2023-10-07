#include "keyed_sort_merge_join.h"
#include "query_table/table_factory.h"
#include "operators/project.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "operators/sort.h"
#include "util/data_utilities.h"
#include "util/field_utilities.h"
#include "query_table/field/field_factory.h"
#include "util/system_configuration.h"
#include "operators/union.h"
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
    this->output_cardinality_ = foreign_key->getTupleCount();
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
    this->output_cardinality_ = (fkey == 0) ? lhs->getTupleCount() : rhs->getTupleCount();
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
    bit_packed_ = SystemConfiguration::getInstance().bitPackingEnabled();

    updateCollation();
}

template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    lhs->pinned_ = true;
    QueryTable<B> *rhs = this->getChild(1)->getOutput();


    this->start_time_ = clock_start();

	max_intermediate_cardinality_ =  lhs->getTupleCount() + rhs->getTupleCount();
    if(is_secure_ && bit_packed_) {
        int card_bits = ceil(log2(max_intermediate_cardinality_)) + 1; // + 1 for sign bit
        emp::Integer zero_tmp(card_bits, 0, emp::PUBLIC);
        emp::Integer one_tmp(card_bits, 1, emp::PUBLIC);
        zero_ = Field<B>(int_field_type_, zero_tmp);
        one_ = Field<B>(int_field_type_, one_tmp);
    }

    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    QuerySchema out_schema = this->output_schema_;

    foreign_key_cardinality_ = foreign_key_input_ ? rhs->getTupleCount() : lhs->getTupleCount();

	pair<QueryTable<B> *, QueryTable<B> *> augmented =  augmentTables(lhs, rhs);
    QueryTable<B> *s1, *s2;

	s1 = obliviousExpand(augmented.first, true);
	s2 = obliviousExpand(augmented.second, false);

    delete augmented.first;
	delete augmented.second;


    this->output_ = TableFactory<B>::getTable(foreign_key_cardinality_, out_schema, storage_model_);

    size_t lhs_field_cnt = lhs_schema.getFieldCount();
    QueryTable<B> *lhs_reverted = revertProjection(s1, lhs_field_mapping_, true);
    QueryTable<B> *rhs_reverted = revertProjection(s2, rhs_field_mapping_, false);

    delete s1;
    delete s2;

    for(int i = 0; i < foreign_key_cardinality_; i++) {
        B dummy_tag = lhs_reverted->getDummyTag(i) | rhs_reverted->getDummyTag(i);
        this->output_->cloneRow(!dummy_tag, i, 0, lhs_reverted, i);
        this->output_->cloneRow(!dummy_tag, i, lhs_field_cnt, rhs_reverted, i);
        this->output_->setDummyTag(i, dummy_tag);
    }

    delete lhs_reverted;
    delete rhs_reverted;

    lhs->pinned_ = false;
    this->output_->setSortOrder(this->sort_definition_);
    return this->output_;


}

template<typename B>
QuerySchema KeyedSortMergeJoin<B>::deriveAugmentedSchema() const {
    // pick bigger schema as starting point
    Operator<B> *lhs_child = this->getChild(0);
    Operator<B> *rhs_child = this->getChild(1);

    QuerySchema schema = (lhs_smaller_)  ? rhs_child->getOutputSchema() : lhs_child->getOutputSchema();
    int lhs_schema_fields = lhs_child->getOutputSchema().getFieldCount();
    vector<int> keys;
    int write_cursor = 0;
    QuerySchema augmented_schema;


    for(auto key_pair : join_idxs_) {
        // visitor always outputs lhs, rhs
        int k = (lhs_smaller_) ? (key_pair.second  - lhs_schema_fields) : key_pair.first;
        QueryFieldDesc f = schema.getField(k);
        f.setOrdinal(write_cursor);
        ++write_cursor;
        augmented_schema.putField(f);
        keys.emplace_back(k);
    }

    for(int i = 0; i < schema.getFieldCount(); i++) {
        if(std::find(keys.begin(), keys.end(), i) == keys.end()) {
            QueryFieldDesc f = schema.getField(i);
            f.setOrdinal(write_cursor);
            ++write_cursor;
            augmented_schema.putField(f);
        }
    }


    if(!is_secure_) {
        QueryFieldDesc alpha(write_cursor, "alpha", "", int_field_type_, 0);
        augmented_schema.putField(alpha);
        ++write_cursor;
        QueryFieldDesc table_id(write_cursor, "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
    }
    else {
        QueryFieldDesc alpha(write_cursor, "alpha", "", int_field_type_);
        int max_alpha =  lhs_child->getOutputCardinality() + rhs_child->getOutputCardinality();
        if(bit_packed_) alpha.initializeFieldSizeWithCardinality(max_alpha);
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
	QueryTable<B> * lhs = this->getChild(0)->getOutput();
	QueryTable<B> * rhs = this->getChild(1)->getOutput();
	vector<int> lhs_keys, rhs_keys;	

    for(auto key_pair : join_idxs_) {
        // visitor always outputs lhs, rhs
        lhs_keys.emplace_back(key_pair.first);
        rhs_keys.emplace_back(key_pair.second - lhs->getSchema().getFieldCount());
    }
    lhs_prime_ = projectSortKeyToFirstAttr(lhs, lhs_keys, true);
    rhs_prime_ = projectSortKeyToFirstAttr(rhs, rhs_keys, false);

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
	    int max_alpha =  lhs->getTupleCount() + rhs->getTupleCount();
        if(bit_packed_) alpha.initializeFieldSizeWithCardinality(max_alpha);	

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
    storage_model_ = lhs->storageModel();

    // only support row store for now - col store won't easily "stripe" the bits of the smaller relation over different schema
    assert(storage_model_ == StorageModel::ROW_STORE);


    // set up extended schema
    QuerySchema augmented_schema = getAugmentedSchema();

	table_id_idx_ = augmented_schema.getFieldCount() - 1;
    alpha_idx_ = augmented_schema.getFieldCount() - 2;

    auto sorted = sortCompatible() ? unionAndMergeTables() : unionAndSortTables();

    if(is_secure_ && bit_packed_) initializeAlphasPacked(sorted);
    else initializeAlphas(sorted);


    SortDefinition sort_def;
    sort_def.emplace_back(table_id_idx_, SortDirection::ASCENDING);
    for(int i = 0; i < join_idxs_.size(); ++i) {
        sort_def.emplace_back(i, SortDirection::ASCENDING);
    }

    Sort<B> sort_by_table_id(sorted, sort_def);
    sort_by_table_id.setOperatorId(-2);
    QueryTable<B> *sort_table = sort_by_table_id.run();

    pair<QueryTable<B> *, QueryTable<B> *> output;
    //split union table into S1 and S2
    auto s1 = TableFactory<B>::getTable(lhs->getTupleCount(), augmented_schema, storage_model_);
    auto s2 = TableFactory<B>::getTable(rhs->getTupleCount(), augmented_schema, storage_model_);
    int read_cursor = 0;

    for (int i = 0; i < lhs->getTupleCount(); i++) {
        s1->cloneRow(i, 0, sort_table, read_cursor);
        s1->setDummyTag(i, sort_table->getDummyTag(read_cursor));
        ++read_cursor;
    }

    for (int i = 0; i < rhs->getTupleCount(); i++) {
        s2->cloneRow(i, 0, sort_table, read_cursor);
        s2->setDummyTag(i, sort_table->getDummyTag(read_cursor));
        ++read_cursor;
    }
    output.first = s1;
    output.second = s2;


    return output;
}


template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::unionAndSortTables() {
    int cursor = 0;
    QuerySchema augmented_schema = deriveAugmentedSchema();
    int unioned_len = lhs_prime_->getTupleCount() + rhs_prime_->getTupleCount();

    QueryTable<B> *unioned =   TableFactory<B>::getTable(unioned_len, augmented_schema, storage_model_);

    for(int i = 0; i < lhs_prime_->getTupleCount(); ++i) {
        unioned->cloneRow(cursor, 0, lhs_prime_, i);
        unioned->setDummyTag(cursor, lhs_prime_->getDummyTag(i));
        ++cursor;
    }

    for(int i = 0; i < rhs_prime_->getTupleCount(); ++i) {
        unioned->cloneRow(cursor, 0, rhs_prime_, i);
        unioned->setDummyTag(cursor, rhs_prime_->getDummyTag(i));
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
    int cursor = 0;
    QuerySchema augmented_schema = deriveAugmentedSchema();
    int unioned_len = lhs_prime_->getTupleCount() + rhs_prime_->getTupleCount();
    bool lhs_is_foreign_key = (foreign_key_input_ == 0);
    QueryTable<B> *unioned =   TableFactory<B>::getTable(unioned_len, augmented_schema, storage_model_);

    if(lhs_is_foreign_key) {
        // always FK --> PK

        for(int i = lhs_prime_->getTupleCount() - 1; i >= 0; --i) {
            unioned->cloneRow(cursor, 0, lhs_prime_, i);
            unioned->setDummyTag(cursor, lhs_prime_->getDummyTag(i));
            ++cursor;
        }
        for(int i = 0; i < rhs_prime_->getTupleCount(); ++i) {
            unioned->cloneRow(cursor, 0, rhs_prime_, i);
            unioned->setDummyTag(cursor, rhs_prime_->getDummyTag(i));
            unioned->setField(cursor, table_id_idx_, table_id_field_);
            ++cursor;
        }
    }
    else {
        for(int i = rhs_prime_->getTupleCount() - 1; i >= 0; --i) {
            unioned->cloneRow(cursor, 0, rhs_prime_, i);
            unioned->setDummyTag(cursor, rhs_prime_->getDummyTag(i));
            unioned->setField(cursor, table_id_idx_, table_id_field_);
            ++cursor;
        }

        for(int i = 0; i < lhs_prime_->getTupleCount(); ++i) {
            unioned->cloneRow(cursor, 0, lhs_prime_, i);
            unioned->setDummyTag(cursor, lhs_prime_->getDummyTag(i));
            ++cursor;
        }
    }

    delete lhs_prime_;
    delete rhs_prime_;


    // do bitonic merge instead of full sort
    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size()); // join keys
    // sort s.t. fkey entries are first, pkey entries are second

    sort_def.emplace_back(table_id_idx_, lhs_is_foreign_key ? SortDirection::ASCENDING : SortDirection::DESCENDING);

    Sort<B> sorter(unioned, sort_def);
    sorter.setOperatorId(-2);
    auto normalized = sorter.normalizeTable(unioned); // normalize to move up table_id field

    int counter = 0;
    sorter.bitonicMergeNormalized(normalized, sorter.getSortOrder(), 0, normalized->getTupleCount(), true, counter);
    unioned =  sorter.denormalizeTable(normalized);
    unioned->setSortOrder(sort_def);

    delete normalized;
    return unioned;

}

template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::projectSortKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs) {


    ExpressionMapBuilder<B> builder(src->getSchema());
    int write_cursor = 0;
    for(auto key : join_cols) {
        builder.addMapping(key, write_cursor);
        if(is_lhs) { lhs_field_mapping_[write_cursor] = key; }
        else { rhs_field_mapping_[write_cursor] = key; }
        ++write_cursor;
    }	

    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
        if(std::find(join_cols.begin(), join_cols.end(),i) == join_cols.end()) {
            builder.addMapping(i, write_cursor);
            if(is_lhs) { lhs_field_mapping_[write_cursor] = i; }
            else { rhs_field_mapping_[write_cursor] = i; }
            ++write_cursor;
        }
    }

    Project<B> projection(src->clone(), builder.getExprs());
    projection.setOperatorId(-2);

    if(is_lhs)  lhs_projected_schema_ = projection.getOutputSchema();
    else rhs_projected_schema_ = projection.getOutputSchema();	

    return projection.run()->clone();
}

template<typename B>
void KeyedSortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {
	Field<B> alpha_1 = zero_;
	Field<B> alpha_2 = zero_;
    B one_b(true), zero_b(false);

	B table_id = dst->getField(0, table_id_idx_).template getValue<B>();
    B fkey_check = ((foreign_key_input_ == 0) ? zero_b : one_b);

	B is_foreign_key = (table_id == fkey_check);

    B dummy = dst->getDummyTag(0);

    B same_group = one_b;

    alpha_1 = Field<B>::If(same_group, Field<B>::If(is_foreign_key & !dummy, alpha_1 + one_, alpha_1),
                           Field<B>::If(is_foreign_key & !dummy, one_, zero_));
    alpha_2 = Field<B>::If(same_group, Field<B>::If(!is_foreign_key, alpha_2 + one_, zero_),
                           Field<B>::If(!is_foreign_key, one_, zero_));

    dst->setPackedField(0, alpha_idx_, Field<B>::If(is_foreign_key, alpha_2, alpha_1));


    for(int i = 1; i < dst->getTupleCount(); i++) {
		table_id = dst->getPackedField(i, table_id_idx_).template getValue<B>();
		is_foreign_key = (table_id == fkey_check);
        B dummy = dst->getDummyTag(i);

		B same_group = joinMatch(dst,  i-1, i);
		alpha_1 = Field<B>::If(same_group, Field<B>::If(is_foreign_key & !dummy, alpha_1 + one_, alpha_1),
                               Field<B>::If(is_foreign_key & !dummy, one_, zero_));
		alpha_2 = Field<B>::If(same_group, Field<B>::If(!is_foreign_key, alpha_2 + one_, zero_),
                               Field<B>::If(!is_foreign_key, one_, zero_));
		dst->setPackedField(i, alpha_idx_, Field<B>::If(is_foreign_key, alpha_2, alpha_1));
	}

    int last_idx = dst->getTupleCount() - 1;

    table_id = dst->getPackedField(last_idx, table_id_idx_).template getValue<B>();
    is_foreign_key = (table_id == fkey_check);
    same_group = one_b;
    dummy = dst->getDummyTag(last_idx);
    // inputs are sorted fkey --> pkey
    //if we are on the last entry and it is fkey, then we know there are no matches so we set alpha_2 to 0
    alpha_1 = Field<B>::If(is_foreign_key, zero_, Field<B>::If(!is_foreign_key & !dummy, one_, zero_));
    dst->setPackedField(last_idx, alpha_idx_, Field<B>::If(is_foreign_key, alpha_1, dst->getPackedField(last_idx, alpha_idx_)));
    dst->setPackedField(last_idx, alpha_idx_, Field<B>::If(dst->getDummyTag(last_idx), zero_, dst->getPackedField(last_idx, alpha_idx_)));


	for(int i = last_idx-1; i >= 0; i--) {
        table_id = dst->getPackedField(i, table_id_idx_).template getValue<B>();
		is_foreign_key = (table_id == fkey_check);
		B same_group = joinMatch(dst,  i+1, i);
        B dummy = dst->getDummyTag(i);
		alpha_1 = Field<B>::If(is_foreign_key & same_group, alpha_1, Field<B>::If(!is_foreign_key & !dummy, one_, zero_));
		dst->setPackedField(i, alpha_idx_, Field<B>::If(is_foreign_key, alpha_1, dst->getPackedField(i, alpha_idx_)));
		dst->setPackedField(i, alpha_idx_, Field<B>::If(dst->getDummyTag(i), zero_, dst->getPackedField(i, alpha_idx_)));
    }
}

template<>
void KeyedSortMergeJoin<bool>::initializeAlphasPacked(PlainTable *dst) {
    throw; // should never get here - packing not supported for plaintext
}


template<>
void KeyedSortMergeJoin<Bit>::initializeAlphasPacked(SecureTable *dst) {
	Integer zero = zero_.getValue<Integer>();
	Integer one = one_.getValue<Integer>();

	Integer alpha_1 = zero;
	Integer alpha_2 = zero;
	Bit one_b(true), zero_b(false);

	Bit table_id = dst->getField(0, table_id_idx_).template getValue<Bit>();
    Bit fkey_check = (foreign_key_input_ == 0 ? zero_b : one_b);
	Bit is_foreign_key = (table_id == fkey_check);

	for(int i = 0; i < dst->getTupleCount(); i++) {
		table_id = dst->getPackedField(i, table_id_idx_).template getValue<Bit>();
		is_foreign_key = (table_id == fkey_check);
		Bit same_group = joinMatch(dst, (i==0 ? i : i-1), i);
		Bit dummy = dst->getDummyTag(i);
		alpha_1 = emp::If(same_group, emp::If(is_foreign_key & !dummy, alpha_1 + one, alpha_1),
                          emp::If(is_foreign_key & !dummy, one, zero));

		alpha_2 = emp::If(same_group, emp::If(!is_foreign_key, alpha_2 + one, zero), emp::If(!is_foreign_key, one, zero));

		Integer to_write = emp::If(is_foreign_key, alpha_2, alpha_1);
		dst->setPackedField(i, alpha_idx_, SecureField(int_field_type_, to_write));

	}

    int cutoff = dst->getTupleCount() -1;
	for(int i = cutoff; i >= 0; i--) {
		table_id = dst->getPackedField(i, table_id_idx_).template getValue<Bit>();
		is_foreign_key = (table_id == fkey_check);
		Bit same_group = joinMatch(dst, (i == cutoff) ? i : i+1, i);
        Bit dummy = dst->getDummyTag(i);
		alpha_1 = emp::If(is_foreign_key & same_group, alpha_1, emp::If(!is_foreign_key & !dummy, one, zero));

		Integer to_write = emp::If(is_foreign_key, alpha_1, dst->getPackedField(i, alpha_idx_).template getValue<Integer>());
		dst->setPackedField(i, alpha_idx_, SecureField(int_field_type_, to_write));
		to_write = emp::If(dst->getDummyTag(i), zero, dst->getPackedField(i, alpha_idx_).template getValue<Integer>());
		dst->setPackedField(i, alpha_idx_, SecureField(int_field_type_, to_write));

	}

}





template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::obliviousDistribute(QueryTable<B> *input, size_t target_size) {
    QuerySchema schema = input->getSchema();

    SortDefinition sort_def{ ColumnSort(is_new_idx_, SortDirection::ASCENDING), ColumnSort(weight_idx_, SortDirection::ASCENDING)};
    Sort<B> sorted(input, sort_def);
    sorted.setOperatorId(-2);
    auto sorted_input = sorted.run();

    QueryTable<B> *dst_table = TableFactory<B>::getTable(target_size, schema, storage_model_);

    for(int i = 0; i < sorted_input->getTupleCount(); i++) {
        dst_table->cloneRow(i, 0, sorted_input, i);
        dst_table->setDummyTag(i, sorted_input->getDummyTag(i));
    }

    Field<B> table_idx = sorted_input->getField(0, table_id_idx_);
    Field<B> one_b(bool_field_type_, B(true));

    for(int i = sorted_input->getTupleCount(); i < target_size; i++) {
        dst_table->setField(i, is_new_idx_, one_b);
        dst_table->setField(i, table_id_idx_, table_idx);
    }

    int j = Sort<B>::powerOfTwoLessThan(target_size);
    int weight_width = (is_secure_ && bit_packed_) ? zero_.template getValue<Integer>().size() : 32;


    while(j >= 1) {
        for(int i = target_size - j - 1; i >= 0; i--) {

            Field<B> weight = dst_table->getPackedField(i,weight_idx_);
            Field<B> pos = FieldFactory<B>::getInt(i + j + 1, weight_width);
            B result = (weight >= pos);
            dst_table->compareSwap(result, i, i+j);

        }
        j = j/2;
    }

    return dst_table;

}


template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::obliviousExpand(QueryTable<B> *input, bool is_lhs) {

    if(is_secure_ && bit_packed_) return obliviousExpandPacked(input, is_lhs);


    QuerySchema schema = input->getSchema();

    QueryFieldDesc weight(schema.getFieldCount(), "weight", "", int_field_type_);
    schema.putField(weight);
    QueryFieldDesc is_new(schema.getFieldCount(), "is_new", "", bool_field_type_);
    schema.putField(is_new);
    schema.initializeFieldOffsets();

    QueryTable<B> *intermediate_table = TableFactory<B>::getTable(input->getTupleCount(), schema, input->storageModel());

    is_new_idx_ = schema.getFieldCount() - 1;
    weight_idx_ = schema.getFieldCount() - 2;

    Field<B> s = one_;
    Field<B> one_b(bool_field_type_, B(true)), zero_b(bool_field_type_, B(false));
    for(int i = 0; i < input->getTupleCount(); i++) {
        intermediate_table->cloneRow(i, 0, input, i); // copy input row
        Field<B> cnt = input->getPackedField(i, alpha_idx_); // cnt = alpha
        B null_val = (cnt == zero_); // is count empty?  If no, set weight to running count, else set weight to zero
        intermediate_table->setField(i, weight_idx_,
                                     Field<B>::If(null_val, zero_, s));
        intermediate_table->setField(i, is_new_idx_,
                                     Field<B>::If(null_val, one_b, zero_b));
        intermediate_table->setDummyTag(i, input->getDummyTag(i) | null_val);
		s = s + cnt;
    }
    QueryTable<B> *dst_table = obliviousDistribute(intermediate_table, foreign_key_cardinality_);
    schema = dst_table->getSchema();
    QueryTuple<B> tmp(&schema);

    tmp.setField(is_new_idx_, one_b);

    for(int i = 0; i < foreign_key_cardinality_; i++) {

        B result = (dst_table->getField(i, is_new_idx_) == one_b);
        tmp.setDummyTag(FieldUtilities::select(result, tmp.getDummyTag(), dst_table->getDummyTag(i)));

        for(int j = 0; j < schema.getFieldCount(); j++) {
            dst_table->setField(i, is_new_idx_, zero_b);
            Field<B> to_write = Field<B>::If(result, tmp.getPackedField(j), dst_table->getPackedField(i, j));
            tmp.setPackedField(j, to_write);
            dst_table->setPackedField(i, j, to_write);

        }
		Field<B> write_index = FieldFactory<B>::getInt(i);
		B end_matches = write_index >= s-one_;
        dst_table->setDummyTag(i, FieldUtilities::select(result, tmp.getDummyTag() | end_matches, dst_table->getDummyTag(i) | end_matches));
    }

    return dst_table;
}

template<>
SecureTable *KeyedSortMergeJoin<Bit>::obliviousExpandPacked(SecureTable *input, bool is_lhs) {

    QuerySchema schema = input->getSchema();

    QueryFieldDesc weight(schema.getFieldCount(), "weight", "", int_field_type_);
    weight.initializeFieldSizeWithCardinality(max_intermediate_cardinality_);
    schema.putField(weight);
    QueryFieldDesc is_new(schema.getFieldCount(), "is_new", "", bool_field_type_);
    schema.putField(is_new);
    schema.initializeFieldOffsets();

    QueryTable<Bit> *intermediate_table = TableFactory<Bit>::getTable(input->getTupleCount(), schema, input->storageModel());

    is_new_idx_ = schema.getFieldCount() - 1;
    weight_idx_ = schema.getFieldCount() - 2;


    bool table_id = !(is_lhs);
    // should always be false
    bool is_foreign_key = (table_id == foreign_key_input_);

    Integer zero = zero_.getValue<Integer>();
    Integer one = one_.getValue<Integer>();
    Bit one_b(true), zero_b(false);

    Integer s = one;
    Integer cnt;

    for(int i = 0; i < input->getTupleCount(); i++) {
        intermediate_table->cloneRow(i, 0, input, i);
        cnt = input->getPackedField(i, alpha_idx_).getValue<Integer>();
        Bit zero_cnt = (cnt == zero);

        Integer weight_int = emp::If(zero_cnt, zero, s);
        Bit is_new_bit = emp::If(zero_cnt, one_b, zero_b);

        intermediate_table->setPackedField(i, weight_idx_, SecureField(weight.getType(), weight_int));
        intermediate_table->setField(i, is_new_idx_, SecureField(bool_field_type_, is_new_bit));
        intermediate_table->setDummyTag(i, input->getDummyTag(i) | zero_cnt);
        s = s + cnt;

    }


    SecureTable *dst_table = obliviousDistribute(intermediate_table, foreign_key_cardinality_);

    schema = dst_table->getSchema();

    SecureTuple tmp_row(&schema);
    tmp_row.setField(is_new_idx_, SecureField(bool_field_type_, one_b));
    // initialize it to first row in dst_table
    memcpy(tmp_row.getData(), ((RowTable<Bit> *) dst_table)->tuple_data_.data(), dst_table->tuple_size_bytes_);


	size_t start_gates = this->system_conf_.andGateCount();

    for(int i = 0; i < foreign_key_cardinality_; i++) {
        Bit is_new_bit = dst_table->getField(i, is_new_idx_).getValue<Bit>();
        Bit new_write = (is_new_bit == one_b);
        tmp_row.setDummyTag(FieldUtilities::select(new_write, tmp_row.getDummyTag(), dst_table->getDummyTag(i)));

        for(int j = 0; j < schema.getFieldCount(); j++) {
            SecureField to_write = SecureField::If(new_write, tmp_row.getPackedField(j), dst_table->getPackedField(i, j));
            tmp_row.setPackedField(j, to_write);
            dst_table->setPackedField(i, j, to_write);

        }
        dst_table->setPackedField(i, is_new_idx_, SecureField(bool_field_type_, zero_b));
		
		Integer write_index(schema.getField(weight_idx_).size() + 1, i);
		
		Bit end_matches = write_index >= (s - one);
        dst_table->setDummyTag(i, FieldUtilities::select(new_write, tmp_row.getDummyTag() | end_matches,
                                                         dst_table->getDummyTag(i) | end_matches));
    }

    return dst_table;
}

template<>
PlainTable *KeyedSortMergeJoin<bool>::obliviousExpandPacked(PlainTable *input, bool is_lhs) {
    throw; // should never get here - this method for secure runs only
}

/*template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::alignTable(QueryTable<B> *input) {
    QuerySchema schema = input->getSchema();

    QueryFieldDesc weight(schema.getFieldCount(), "ii", "", is_secure_ ? FieldType::SECURE_INT : FieldType::INT);


    schema.putField(weight);
    schema.initializeFieldOffsets();

    size_t ii_idx = schema.getFieldCount() - 1;

    Field<B> count = zero_ - one_;


    QueryTable<B> *dst_table = TableFactory<B>::getTable(input->getTupleCount(), schema, storage_model_);


    for(int i = 0; i < input->getTupleCount(); i++) {

        B same_group = joinMatch(input, i-1, i);

        count = Field<B>::If(same_group, count+one_, zero_);

        Field<B> alpha_1 = input->getField(i, alpha_1_idx_); input->getField(i, alpha_1_idx_);
        Field<B> alpha_2 = input->getField(i, alpha_2_idx_);

        alpha_2 = Field<B>::If(alpha_2 == zero_, one_, alpha_2);

        dst_table->setField(i, ii_idx, count/alpha_2 + (count % alpha_2) * alpha_1);
        dst_table->setDummyTag(i, input->getDummyTag(i));

        dst_table->cloneRow(i, 0, input, i);
    }

    SortDefinition s =  DataUtilities::getDefaultSortDefinition(join_idxs_.size());
    s.emplace_back(ii_idx, SortDirection::ASCENDING);
    Sort<B> sorter(dst_table, s);
    return sorter.run()->clone();
}
*/

template<typename B>
QueryTable<B> *KeyedSortMergeJoin<B>::revertProjection(QueryTable<B> *s, const map<int, int> &expr_map,
                                                  const bool &is_lhs) const {


    RowTable<B> *src = (RowTable<B> *) s;
    // // create a synthetic schema.  pad it to make it the "right" row length for projection
    // for use with smaller width row
    if(lhs_smaller_ == is_lhs) {
        QuerySchema synthetic_schema = (is_lhs) ? lhs_projected_schema_ : rhs_projected_schema_;
        int delta = s->getSchema().size() - synthetic_schema.size();
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
        s->setSchema(synthetic_schema);

    }

    ExpressionMapBuilder<B> builder(s->getSchema());
    for(auto pos : expr_map) {
        builder.addMapping(pos.first, pos.second);
    }

    Project<B> projection(src->clone(), builder.getExprs());
    projection.setOperatorId(-2);
    return projection.run()->clone();

}

template class vaultdb::KeyedSortMergeJoin<bool>;
template class vaultdb::KeyedSortMergeJoin<emp::Bit>;

