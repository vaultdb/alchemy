#include "sort_merge_join.h"
#include "query_table/table_factory.h"
#include "operators/project.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "operators/sort.h"
#include "util/data_utilities.h"
#include "util/field_utilities.h"
#include "query_table/field/field_factory.h"
#include "util/system_configuration.h"

using namespace vaultdb;

// lhs assumed to be fkey
template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key,  Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(foreign_key, primary_key, predicate, sort) {
    this->output_cardinality_ = foreign_key->getOutputCardinality();
    setup();
}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate,
                                const SortDefinition &sort)  : Join<B>(foreign_key, primary_key, predicate, sort) {
    this->output_cardinality_ = foreign_key->getTupleCount();
    setup();
}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey != 0) {

    assert(fkey == 0 || fkey == 1);
    this->output_cardinality_ = (fkey == 0) ? lhs->getOutputCardinality() : rhs->getOutputCardinality();
    setup();
}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
              const SortDefinition &sort)  : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey != 0) {

    assert(fkey == 0 || fkey == 1);
    this->output_cardinality_ = (fkey == 0) ? lhs->getTupleCount() : rhs->getTupleCount();
    setup();
}


template<typename B>
void SortMergeJoin<B>::setup() {
    is_secure_ = std::is_same_v<B, emp::Bit>;
    int_field_type_ = is_secure_ ? FieldType::SECURE_INT : FieldType::INT;

    auto p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    join_idxs_  = join_visitor.getEqualities();
    lhs_smaller_ = (this->getChild(0)->getOutputSchema().size() < this->getChild(1)->getOutputSchema().size());

    one_ = FieldFactory<B>::getOne(int_field_type_);
    zero_ = FieldFactory<B>::getZero(int_field_type_);
    bit_packed_ = SystemConfiguration::getInstance().bitPackingEnabled();
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
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
    QuerySchema out_schema = QuerySchema::concatenate(lhs->getSchema(), rhs->getSchema());

    foreign_key_cardinality_ = foreign_key_input_ ? rhs->getTupleCount() : lhs->getTupleCount();

	pair<QueryTable<B> *, QueryTable<B> *> augmented =  augmentTables(lhs, rhs);

    QueryTable<B> *s1, *s2;
    // lhs is FK
    if(!foreign_key_input_) {
        s1 = augmented.first;
        s2 = obliviousExpand(augmented.second, false);
        delete augmented.second;

    }
    else {
        s1 = obliviousExpand(augmented.first, true);
        delete augmented.first;
        s2 = augmented.second;
    }

    this->output_ = TableFactory<B>::getTable(foreign_key_cardinality_, out_schema, storage_model_);

    size_t lhs_field_cnt = lhs_schema.getFieldCount();

    QueryTable<B> *lhs_reverted = revertProjection(s1, lhs_field_mapping_, true);
    QueryTable<B> *rhs_reverted = revertProjection(s2, rhs_field_mapping_, false);

    delete s1;
    delete s2;

    for(int i = 0; i < foreign_key_cardinality_; i++) {
        this->output_->cloneRow(i, 0, lhs_reverted, i);
        this->output_->cloneRow(i, lhs_field_cnt, rhs_reverted, i);
        this->output_->setDummyTag(i, lhs_reverted->getDummyTag(i) | rhs_reverted->getDummyTag(i));
    }

    delete lhs_reverted;
    delete rhs_reverted;
    return Join<B>::output_;


}

template<typename B>

QuerySchema SortMergeJoin<B>::getAugmentedSchema() {
	QueryTable<B> * lhs = this->getChild(0)->getOutput();
	QueryTable<B> * rhs = this->getChild(1)->getOutput();
	vector<int> lhs_keys, rhs_keys;

    for(auto key_pair : join_idxs_) {
        // visitor always outputs lhs, rhs
        lhs_keys.emplace_back(key_pair.first);
        rhs_keys.emplace_back(key_pair.second - lhs->getSchema().getFieldCount());
    }
	lhs_prime = projectSortKeyToFirstAttr(lhs, lhs_keys, true);
	rhs_prime = projectSortKeyToFirstAttr(rhs, rhs_keys, false);

	QuerySchema augmented_schema = (lhs_smaller_) ? rhs_prime->getSchema() : lhs_prime->getSchema();	
	
    if(!is_secure_) {
        QueryFieldDesc alpha(augmented_schema.getFieldCount(), "alpha", "", int_field_type_, 0);
        augmented_schema.putField(alpha);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field = Field<B>(FieldType::BOOL, true);
    }
    else {
        QueryFieldDesc alpha(augmented_schema.getFieldCount(), "alpha", "", int_field_type_);
	int max_alpha =  lhs->getTupleCount() + rhs->getTupleCount();
        if(bit_packed_) alpha.initializeFieldSizeWithCardinality(max_alpha);	

        augmented_schema.putField(alpha);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::SECURE_BOOL);	

        augmented_schema.putField(table_id);
        table_id_field = Field<B>( FieldType::SECURE_BOOL, Bit(true));
    }

    augmented_schema.initializeFieldOffsets();	

	return augmented_schema;

}

template<typename B>
pair<QueryTable<B> *, QueryTable<B> *>  SortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {
    assert(lhs->storageModel() == rhs->storageModel());
    storage_model_ = lhs->storageModel();
    // cout << "Augment tables, start AND gates: " << this->system_conf_.andGateCount() << endl;

    // only support row store for now - col store won't easily "stripe" the bits of the smaller relation over different schema
    assert(storage_model_ == StorageModel::ROW_STORE);


    // set up extended schema
    QuerySchema augmented_schema = getAugmentedSchema();

	table_id_idx_ = augmented_schema.getFieldCount() - 1;
    alpha_idx_ = augmented_schema.getFieldCount() - 2;
 
    int output_cursor = 0;
    QueryTable<B> *unioned = TableFactory<B>::getTable(lhs->getTupleCount() + rhs->getTupleCount(), augmented_schema, storage_model_);
    for(int i = 0; i < lhs_prime->getTupleCount(); ++i) {
        unioned->cloneRow(output_cursor, 0, lhs_prime, i);
        unioned->setDummyTag(output_cursor, lhs_prime->getDummyTag(i));
        ++output_cursor;

    }

    for(int i = 0; i < rhs_prime->getTupleCount(); ++i) {
        unioned->cloneRow(output_cursor, 0, rhs_prime, i);
        unioned->setDummyTag(output_cursor, rhs_prime->getDummyTag(i));
        unioned->setField(output_cursor, table_id_idx_, table_id_field);
        ++output_cursor;
    }

    delete lhs_prime;
    delete rhs_prime;

    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size()); // join keys
    // sort s.t. fkey entries are first, pkey entries are second
    SortDirection dir = (foreign_key_input_) ? SortDirection::DESCENDING : SortDirection::ASCENDING;
    sort_def.emplace_back(table_id_idx_, dir);
    Sort<B> sorter(unioned, sort_def);


    QueryTable<B> *sorted = sorter.run()->clone();

    if(is_secure_ && bit_packed_) initializeAlphasPacked(sorted);
    else initializeAlphas(sorted);


    sort_def.clear();
    sort_def.emplace_back(table_id_idx_, SortDirection::ASCENDING);
    for(int i = 0; i < join_idxs_.size(); ++i) {
        sort_def.emplace_back(i, SortDirection::ASCENDING);
    }

    Sort<B> sort_by_table_id(sorted, sort_def);
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
QueryTable<B> *SortMergeJoin<B>::projectSortKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols, const int & is_lhs) {


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

    if(is_lhs)  lhs_projected_schema_ = projection.getOutputSchema();
    else rhs_projected_schema_ = projection.getOutputSchema();

    return projection.run()->clone();
}

template<typename B>
void SortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {	
	Field<B> prev_alpha = zero_;

    B prev_table_id =  dst->getField(0, table_id_idx_).template getValue<B>();
    B table_id = prev_table_id;


    B fkey = B(foreign_key_input_);

    // set correct alpha to 1 for first row
    // if table ID is false, update alpha 1
    Field<B> prev_count;
    Field<B> count = Field<B>::If(fkey == table_id, one_, zero_);
    // cache last fkey row count for this join key
    prev_count = count;
    dst->setField(0, alpha_idx_, count);

    for (int i = 1; i < dst->getTupleCount(); i++) {
        table_id = dst->getField(i, table_id_idx_).template getValue<B>();
        B is_foreign_key = (table_id == fkey); // ~ 3 gates
        B same_group = joinMatch(dst, i-1, i); // ~ |join keys| gates
        B same_group_table = ((table_id == prev_table_id) & same_group); // ~ 3 gates

        count = Field<B>::If(same_group_table, count + one_, one_); // ~ 64 gates
        // if pkey entry and not matched, reset prev_count to zero
        B no_match = !same_group & !is_foreign_key; // ~ 3 gates
        prev_count = Field<B>::If(no_match, zero_, prev_count); // ~ 32 gates
        Field<B> to_write = Field<B>::If(is_foreign_key, count, prev_count); // ~ 32 gates

        dst->setField(i, alpha_idx_, to_write);
        prev_table_id = table_id;
        prev_count = Field<B>::If(is_foreign_key, count, prev_count); // carry over previous count, ~ 32 gates
    }

	prev_alpha = dst->getField(dst->getTupleCount()-1, alpha_idx_);

    for (int i = dst->getTupleCount() - 2; i >= 0; i--) {
        count = dst->getField(i, alpha_idx_);
        B same_group = joinMatch(dst, i, i+1);
        dst->setField(i, alpha_idx_, Field<B>::If(same_group, prev_alpha, count));
     	prev_alpha = dst->getField(i, alpha_idx_);

    }


}

template<>
void SortMergeJoin<bool>::initializeAlphasPacked(PlainTable *dst) {
    throw; // should never get here - packing not supported for plaintext
}


template<>
void SortMergeJoin<Bit>::initializeAlphasPacked(SecureTable *dst) {
    // cout << "initialize alphas start gates: " << this->system_conf_.andGateCount() << endl;
    Integer prev_alpha = zero_.getValue<Integer>();

    Bit prev_table_id =  dst->getField(0, table_id_idx_).template getValue<Bit>();
    Bit table_id = prev_table_id;


    Bit fkey = Bit(foreign_key_input_);

    // set correct alpha to 1 for first row
    // if table ID is false, update alpha 1
    Integer prev_count;
    Integer one = one_.getValue<Integer>();
    Integer zero = zero_.getValue<Integer>();

    Integer count = emp::If(fkey == table_id, one, zero);

    // cache last fkey row count for this join key
    prev_count = count;
    dst->setField(0, alpha_idx_, SecureField(int_field_type_, count));

    for (int i = 1; i < dst->getTupleCount(); i++) {
        table_id = dst->getPackedField(i, table_id_idx_).template getValue<Bit>();
        Bit is_foreign_key = (table_id == fkey); // ~ 3 gates
        Bit same_group = joinMatch(dst, i-1, i); // ~ |join keys| gates
        Bit same_group_table = ((table_id == prev_table_id) & same_group); // ~ 3 gates

        count = emp::If(same_group_table, count + one, one); // ~ 32 gates
        // if pkey entry and not matched, reset prev_count to zero
        Bit no_match = !same_group & !is_foreign_key; // ~ 3 gates
        prev_count = emp::If(no_match, zero, prev_count); // ~ 32 gates
        Integer to_write = emp::If(is_foreign_key, count, prev_count); // ~ 32 gates

        dst->setPackedField(i, alpha_idx_, SecureField(int_field_type_, to_write));
        prev_table_id = table_id;
        prev_count = emp::If(is_foreign_key, count, prev_count); // carry over previous count, ~ 32 gates
    }

    prev_alpha = dst->getPackedField(dst->getTupleCount()-1, alpha_idx_).getValue<Integer>();

    for (int i = dst->getTupleCount() - 2; i >= 0; i--) {
        count = dst->getPackedField(i, alpha_idx_).getValue<Integer>();
        Bit same_group = joinMatch(dst, i, i+1);
        Integer to_write = emp::If(same_group, prev_alpha, count);
        dst->setPackedField(i, alpha_idx_, SecureField(int_field_type_, to_write));
        prev_alpha = to_write;
    }


}





template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousDistribute(QueryTable<B> *input, size_t target_size) {
    QuerySchema schema = input->getSchema();

    SortDefinition sort_def{ ColumnSort(is_new_idx_, SortDirection::ASCENDING), ColumnSort(weight_idx_, SortDirection::ASCENDING)};
    Sort<B> sorted(input, sort_def);
    input = sorted.run();

    QueryTable<B> *dst_table = TableFactory<B>::getTable(target_size, schema, storage_model_);

    for(int i = 0; i < input->getTupleCount(); i++) {
        dst_table->cloneRow(i, 0, input, i);
        dst_table->setDummyTag(i, input->getDummyTag(i));

    }


    Field<B> table_idx = input->getField(0, table_id_idx_);

    for(int i = input->getTupleCount(); i < target_size; i++) {
        dst_table->setField(i, is_new_idx_, one_);
        dst_table->setField(i, table_id_idx_, table_idx);
    }
        // fails somewhere in here on A
    int j = Sort<B>::powerOfTwoLessThan(target_size);
    int weight_width = (is_secure_ && bit_packed_) ? zero_.template getValue<Integer>().size() : 32;

    while(j >= 1) {
        for(int i = target_size - j - 1; i >= 0; i--) {

            Field<B> weight = dst_table->getPackedField(i,weight_idx_);

            B result = weight >= FieldFactory<B>::getInt(i + j + 1, weight_width);
            dst_table->compareSwap(result, i, i+j);
        }
        j = j/2;
    }

    return dst_table;

}


template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousExpand(QueryTable<B> *input, bool is_lhs) {

    if(is_secure_ && bit_packed_) return obliviousExpandPacked(input, is_lhs);

    //cout << "Starting initialize  in obliviousExpand with and gates: " << this->system_conf_.andGateCount() << endl;

    QuerySchema schema = input->getSchema();

    QueryFieldDesc weight(schema.getFieldCount(), "weight", "", int_field_type_);
    schema.putField(weight);
    QueryFieldDesc is_new(schema.getFieldCount(), "is_new", "", int_field_type_);
    schema.putField(is_new);
    schema.initializeFieldOffsets();

    QueryTable<B> *intermediate_table = TableFactory<B>::getTable(input->getTupleCount(), schema, input->storageModel());

    is_new_idx_ = schema.getFieldCount() - 1;
    weight_idx_ = schema.getFieldCount() - 2;

    bool table_id = !(is_lhs);
    // should always be false
    bool is_foreign_key = (table_id == foreign_key_input_);

    Field<B> s = one_;

    for(int i = 0; i < input->getTupleCount(); i++) {
        intermediate_table->cloneRow(i, 0, input, i);
        Field<B> cnt = input->getPackedField(i, alpha_idx_);
        B result = (cnt == zero_);
        intermediate_table->setField(i, weight_idx_,
                                     Field<B>::If(result, zero_, s));
        intermediate_table->setField(i, is_new_idx_,
                                     Field<B>::If(result, one_, zero_));
        intermediate_table->setDummyTag(i, input->getDummyTag(i));
		s = s + cnt;
    }	
    // cout << "Calling obliviousDistribute with and gate count: " << this->system_conf_.andGateCount() << endl;

    QueryTable<B> *dst_table = obliviousDistribute(intermediate_table, foreign_key_cardinality_);
    // cout << "Exited obliviousDistribute with and gate count: " << this->system_conf_.andGateCount() << endl;

    schema = dst_table->getSchema();

    QueryTuple<B> tmp(&schema);

    tmp.setField(is_new_idx_, one_);

    for(int i = 0; i < foreign_key_cardinality_; i++) {

        B result = (dst_table->getField(i, is_new_idx_) == one_);

        tmp.setDummyTag(FieldUtilities::select(result, tmp.getDummyTag(), dst_table->getDummyTag(i)));

        for(int j = 0; j < schema.getFieldCount(); j++) {
            dst_table->setField(i, is_new_idx_, zero_);
            Field<B> to_write = Field<B>::If(result, tmp.getPackedField(j), dst_table->getPackedField(i, j));

            tmp.setField(j, to_write);
            dst_table->setField(i, j, to_write);

        }
        dst_table->setDummyTag(i, FieldUtilities::select(result, tmp.getDummyTag(), dst_table->getDummyTag(i)));
    }

    // cout << "Ending obliviousExpand with and gates: " << this->system_conf_.andGateCount() << endl;


    return dst_table;
}

template<>
SecureTable *SortMergeJoin<Bit>::obliviousExpandPacked(SecureTable *input, bool is_lhs) {

    // cout << "Starting initialize  in obliviousExpandPacked with and gates: " << this->system_conf_.andGateCount() << endl;

    QuerySchema schema = input->getSchema();

    QueryFieldDesc weight(schema.getFieldCount(), "weight", "", int_field_type_);
    weight.initializeFieldSizeWithCardinality(max_intermediate_cardinality_);
    schema.putField(weight);
    QueryFieldDesc is_new(schema.getFieldCount(), "is_new", "", int_field_type_);
    is_new.initializeFieldSizeWithCardinality(max_intermediate_cardinality_);
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
    Integer s = one;
    Integer cnt;

    for(int i = 0; i < input->getTupleCount(); i++) {
        intermediate_table->cloneRow(i, 0, input, i);
        cnt = input->getPackedField(i, alpha_idx_).getValue<Integer>();
        Bit result = (cnt == zero);

        Integer weight_int = emp::If(result, zero, s);
        Integer is_new_int = emp::If(result, one, zero);

        intermediate_table->setPackedField(i, weight_idx_, SecureField(weight.getType(), weight_int));
        intermediate_table->setPackedField(i, is_new_idx_, SecureField(is_new.getType(), is_new_int));
        intermediate_table->setDummyTag(i, input->getDummyTag(i));
        s = s + cnt;
    }
    // cout << "Calling obliviousDistribute with and gate count: " << this->system_conf_.andGateCount() << endl;

    SecureTable *dst_table = obliviousDistribute(intermediate_table, foreign_key_cardinality_);
    // cout << "Exited obliviousDistribute with and gate count: " << this->system_conf_.andGateCount() << endl;

    schema = dst_table->getSchema();

    SecureTuple tmp(&schema);
	
    tmp.setField(is_new_idx_, one_);

    for(int i = 0; i < foreign_key_cardinality_; i++) {

        Bit result = (dst_table->getPackedField(i, is_new_idx_).getValue<Integer>() == one);

        tmp.setDummyTag(FieldUtilities::select(result, tmp.getDummyTag(), dst_table->getDummyTag(i)));

        for(int j = 0; j < schema.getFieldCount(); j++) {
            tmp.setField(j, SecureField::If(result, tmp.getField(j), dst_table->getField(i, j)));
            dst_table->setField(i, j, SecureField::If(result, tmp.getField(j), dst_table->getField(i, j)));

        }
        dst_table->setPackedField(i, is_new_idx_, zero_);
        dst_table->setDummyTag(i, FieldUtilities::select(result, tmp.getDummyTag(), dst_table->getDummyTag(i)));
    }

    // cout << "Ending obliviousExpand with and gates: " << this->system_conf_.andGateCount() << endl;

    return dst_table;
}

template<>
PlainTable *SortMergeJoin<bool>::obliviousExpandPacked(PlainTable *input, bool is_lhs) {
    throw; // should never get here - this method for secure runs only
}

/*template<typename B>
QueryTable<B> *SortMergeJoin<B>::alignTable(QueryTable<B> *input) {
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
QueryTable<B> *SortMergeJoin<B>::revertProjection(QueryTable<B> *s, const map<int, int> &expr_map,
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
    return projection.run()->clone();

}

template class vaultdb::SortMergeJoin<bool>;
template class vaultdb::SortMergeJoin<emp::Bit>;

