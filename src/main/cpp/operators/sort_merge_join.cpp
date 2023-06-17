#include "sort_merge_join.h"
#include "query_table/table_factory.h"
#include "operators/project.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "operators/sort.h"
#include "util/data_utilities.h"
#include "util/field_utilities.h"
#include "query_table/field/field_factory.h"

using namespace vaultdb;

template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs,  Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort), is_secure_(std::is_same_v<B, Bit>) {

    setup();
}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs,Expression<B> *predicate,
                                const SortDefinition &sort)  : Join<B>(lhs, rhs, predicate, sort), is_secure_(std::is_same_v<B, Bit>) {

    setup();
}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort),
                                        is_secure_(std::is_same_v<B, Bit>), foreign_key_input_((fkey == 0) ? false :  true) {

    assert(fkey == 0 || fkey == 1);
    setup();
}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
              const SortDefinition &sort)  : Join<B>(lhs, rhs, predicate, sort),
                      is_secure_(std::is_same_v<B, Bit>), foreign_key_input_((fkey == 0) ? false :  true) {

    assert(fkey == 0 || fkey == 1);
    setup();
}


template<typename B>
void SortMergeJoin<B>::setup() {
    GenericExpression<B> *p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    join_idxs_  = join_visitor.getEqualities();
    lhs_smaller_ = (this->getChild(0)->getOutputSchema().size() < this->getChild(1)->getOutputSchema().size()) ? true : false;
    FieldType int_field = (is_secure_) ? FieldType::SECURE_INT : FieldType::INT;

    one_ = FieldFactory<B>::getOne(int_field);
    zero_ = FieldFactory<B>::getZero(int_field);

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    QueryTable<B> *rhs = this->getChild(1)->getOutput();

    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    QuerySchema out_schema = QuerySchema::concatenate(lhs->getSchema(), rhs->getSchema());

    foreign_key_cardinality_ = foreign_key_input_ ? rhs->getTupleCount() : lhs->getTupleCount();

	pair<QueryTable<B> *, QueryTable<B> *> augmented =  augmentTables(lhs, rhs);
   // cout << "After aug have: " << augmented.first->reveal()->toString(5, false) << ", " << augmented.second->reveal()->toString() << endl;
    QueryTable<B> *s1, *s2;
    // lhs is FK
    if(!foreign_key_input_) {
        s1 = augmented.first;
        s2 = obliviousExpand(augmented.second, false);
        // cout << "Expanded: " << s2->reveal()->toString(5, false) << endl;
        delete augmented.second;
        QueryTable<B> *tmp = alignTable(s2);
        delete s2;
        s2 = tmp;
//        cout << "Aligned: " << s2->reveal()->toString(5, false) << endl;

    }
    else {
        s1 = obliviousExpand(augmented.first, true);
        delete augmented.first;
        QueryTable<B> *tmp = alignTable(s1);
        delete s1;
        s1 = tmp;
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
pair<QueryTable<B> *, QueryTable<B> *>  SortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {
    assert(lhs->storageModel() == rhs->storageModel());
    storage_model_ = lhs->storageModel();

    // only support row store for now - col store won't easily "stripe" the bits of the smaller relation over different schema
    assert(storage_model_ == StorageModel::ROW_STORE);

    vector<int> lhs_keys, rhs_keys;

    for(auto key_pair : join_idxs_) {
        // visitor always outputs lhs, rhs
        lhs_keys.emplace_back(key_pair.first);
        rhs_keys.emplace_back(key_pair.second - lhs->getSchema().getFieldCount());
    }


    // after projection have:
    // <join keys> <table_id> <payload> <alpha1> <alpha2>
    // if it is the smaller one, alphas go to the end - they were written earlier, but
    // the ones we're going to use as alphas are simply not initialized.
    QueryTable<B> *lhs_prime = projectSortKeyToFirstAttr(lhs, lhs_keys, true);
    QueryTable<B> *rhs_prime = projectSortKeyToFirstAttr(rhs, rhs_keys, false);


    // set up extended schema
    QuerySchema augmented_schema = (lhs_smaller_) ?  rhs_prime->getSchema() : lhs_prime->getSchema();

    Field<B> table_id_field;
    if(!is_secure_) {
        QueryFieldDesc alpha_1(augmented_schema.getFieldCount(), "alpha1", "", FieldType::INT, 0);
        augmented_schema.putField(alpha_1);
        QueryFieldDesc alpha_2(augmented_schema.getFieldCount(), "alpha2", "", FieldType::INT, 0);
        augmented_schema.putField(alpha_2);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field = Field<B>(FieldType::BOOL, true);
    }
    else {
        QueryFieldDesc alpha_1(augmented_schema.getFieldCount(), "alpha1", "", FieldType::SECURE_INT, 0);
        augmented_schema.putField(alpha_1);
        QueryFieldDesc alpha_2(augmented_schema.getFieldCount(), "alpha2", "", FieldType::SECURE_INT, 0);
        augmented_schema.putField(alpha_2);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::SECURE_BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field = Field<B>( FieldType::SECURE_BOOL, Bit(true));
    }

    augmented_schema.initializeFieldOffsets();

    table_id_idx_ = augmented_schema.getFieldCount() - 1;
    alpha_1_idx_ = augmented_schema.getFieldCount() - 3; // 3rd to last field
    alpha_2_idx_ = augmented_schema.getFieldCount() - 2; // 2nd to last field


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


    // implicitly deleting unioned table
    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size()); // join keys
    sort_def.emplace_back(augmented_schema.getFieldCount()-1, SortDirection::ASCENDING); // table_id ordinal
    Sort<B> sorter(unioned, sort_def);


    QueryTable<B> *sorted = sorter.run()->clone();


    initializeAlphas(sorted);


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

    Field<B> count = zero_;
	Field<B> prev_alpha_1 = zero_;
	Field<B> prev_alpha_2 = zero_;

    B prev_table_id =  dst->getField(0, table_id_idx_).template getValue<B>();
    B table_id;
    
    B fkey = (!foreign_key_input_) ? B(false) : B(true);

    // set correct alpha to 1 for first row
    // if table ID is false, update alpha 1
    Field<B> a1 = Field<B>::If(!prev_table_id, one_, zero_);
    Field<B> a2 = Field<B>::If(prev_table_id, one_, zero_);
    dst->setField(0, alpha_1_idx_, a1);
    dst->setField(0, alpha_2_idx_, a2);

    // cout << "A1: " << a1.reveal() << " written: " << dst->getField(0, alpha_1_idx_).reveal() << '\n';
    // cout << "A2: " << a2.reveal() << " written: " << dst->getField(0, alpha_2_idx_).reveal() << '\n';
    // cout << "First row: " << dst->reveal()->getPlainTuple(0).toString(true) << endl;

    // TODO: manually compute first row of this logic
    // otherwise need to read i-1 (where i=0) row and this results in invalid reads
    // corrupts output in MPC land
    for (int i = 0; i < dst->getTupleCount(); i++) {

        table_id = dst->getField(i, table_id_idx_).template getValue<B>();
        B is_foreign_key = (table_id == fkey);

        B same_group = joinMatch(dst, i-1, i);
        B result = ((table_id == prev_table_id) & same_group);

        count = count + one_;
        count = Field<B>::If(result, count, one_);

        Field<B> prev_count = Field<B>::If(same_group, dst->getField(i-1, alpha_1_idx_),zero_);
        dst->setField(i, alpha_1_idx_, Field<B>::If(is_foreign_key, count, prev_count));
        dst->setField(i, alpha_2_idx_, Field<B>::If(is_foreign_key, zero_, count));

        // cout << "Alphas for " << i << " " << dst->getField(i, alpha_1_idx_).reveal() << ", " << dst->getField(i, alpha_2_idx_).reveal() << endl;
        prev_table_id = table_id;
    }


    // cout << "After first pass, have: " << dst->reveal()->toString(5, false);

	prev_alpha_1 = dst->getField(dst->getTupleCount()-1, alpha_1_idx_);
	prev_alpha_2 = dst->getField(dst->getTupleCount()-1, alpha_2_idx_);

    for (int i = dst->getTupleCount() - 1; i >= 0; i--) {
        count = dst->getField(i, alpha_1_idx_);

        B same_group = joinMatch(dst, i, i+1);
        dst->setField(i, alpha_1_idx_, Field<B>::If(same_group, prev_alpha_1, count));
		prev_alpha_1 = dst->getField(i, alpha_1_idx_);

        count = dst->getField(i, alpha_2_idx_);
        dst->setField(i, alpha_2_idx_, Field<B>::If(same_group, prev_alpha_2, count));
		prev_alpha_2 = dst->getField(i, alpha_2_idx_);
    }
}



template<typename B>
int SortMergeJoin<B>::powerOfLessThanTwo(const int & n) const {
    int k = 1;
    while (k < n)
        k = k << 1;
    return k >> 1;
}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousDistribute(QueryTable<B> *input, size_t target_size) {
    QuerySchema schema = input->getSchema();

    // cout << "Input: " << input->reveal()->toString(5, false);


    SortDefinition sort_def;

    sort_def.emplace_back(is_new_idx_, SortDirection::ASCENDING);
    sort_def.emplace_back(weight_idx_, SortDirection::ASCENDING);
    Sort<B> sorted(input, sort_def);
    input = sorted.run();
    // cout << "Post-sort: " << input->reveal()->toString(5, false);

    QueryTable<B> *dst_table = TableFactory<B>::getTable(target_size, schema, storage_model_);

    for(int i = 0; i < input->getTupleCount(); i++) {
        dst_table->cloneRow(i, 0, input, i);
        dst_table->setDummyTag(i, input->getDummyTag(i));

    }

    // cout << "Cloned: " << dst_table->reveal()->toString(5, false);

    Field<B> table_idx = input->getField(0, table_id_idx_);

    for(int i = input->getTupleCount(); i < target_size; i++) {
        dst_table->setField(i, is_new_idx_, one_);
        dst_table->setField(i, table_id_idx_, table_idx);
//        dst_table->setField(i, alpha_2_idx_, one_);
    }

    int j = SortMergeJoin<B>::powerOfLessThanTwo(target_size);

    while(j >= 1) {
        for(int i = target_size - j - 1; i >= 0; i--) {

            Field<B> weight = dst_table->getField(i,weight_idx_);

            B result = weight >= FieldFactory<B>::getInt(i + j + 1);
            dst_table->compareSwap(result, i, i+j);
        }
        j = j/2;
    }

    return dst_table;

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::obliviousExpand(QueryTable<B> *input, bool is_lhs) {


    QuerySchema schema = input->getSchema();

    QueryFieldDesc weight(schema.getFieldCount(), "weight", "", is_secure_ ? FieldType::SECURE_INT : FieldType::INT);
    schema.putField(weight);
    QueryFieldDesc is_new(schema.getFieldCount(), "is_new", "", is_secure_ ? FieldType::SECURE_INT : FieldType::INT);
    schema.putField(is_new);
    schema.initializeFieldOffsets();

    QueryTable<B> *intermediate_table = TableFactory<B>::getTable(input->getTupleCount(), schema, input->storageModel());

    is_new_idx_ = schema.getFieldCount() - 1;
    weight_idx_ = schema.getFieldCount() - 2;


    bool table_id = (is_lhs) ? false : true;
    // should always be false
    bool is_foreign_key = (table_id == foreign_key_input_);

    //alpha1 for primary key table, alpha2 for foreign key table
    int read_offset = is_foreign_key ? alpha_2_idx_ : alpha_1_idx_;


    Field<B> s = one_;

    for(int i = 0; i < input->getTupleCount(); i++) {
        intermediate_table->cloneRow(i, 0, input, i);
        Field<B> cnt = input->getField(i, read_offset);
        B result = (cnt == zero_);
        intermediate_table->setField(i, weight_idx_,
                                     Field<B>::If(result, zero_, s));
        intermediate_table->setField(i, is_new_idx_,
                                     Field<B>::If(result, one_, zero_));
        intermediate_table->setDummyTag(i, input->getDummyTag(i));
		s = s + cnt;
    }	

    // cout << "Intermediate: " << intermediate_table->reveal()->toString(5, false);
    QueryTable<B> *dst_table = obliviousDistribute(intermediate_table, foreign_key_cardinality_);
    // cout << "Distributed: " << dst_table->reveal()->toString(5, false);

    schema = dst_table->getSchema();

    QueryTuple<B> tmp(&schema);

    tmp.setField(is_new_idx_, one_);

    for(int i = 0; i < foreign_key_cardinality_; i++) {

        B result = (dst_table->getField(i, is_new_idx_) == one_);

        tmp.setDummyTag(FieldUtilities::select(result, tmp.getDummyTag(), dst_table->getDummyTag(i)));

        for(int j = 0; j < schema.getFieldCount(); j++) {
            dst_table->setField(i, is_new_idx_, zero_);
            tmp.setField(j, Field<B>::If(result, tmp.getField(j), dst_table->getField(i, j)));
            dst_table->setField(i, j, Field<B>::If(result, tmp.getField(j), dst_table->getField(i, j)));

        }
        dst_table->setDummyTag(i, FieldUtilities::select(result, tmp.getDummyTag(), dst_table->getDummyTag(i)));
    }
	

    return dst_table;
}

template<typename B>
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

