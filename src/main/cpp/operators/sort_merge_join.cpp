#include "sort_merge_join.h"
#include "query_table/table_factory.h"
#include "operators/project.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "operators/sort.h"
#include "util/data_utilities.h"

using namespace vaultdb;

template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, const int & fkey, Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {

    GenericExpression<B> *p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    join_idxs_  = join_visitor.getEqualities();



}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate,
              const SortDefinition &sort)  : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {

    GenericExpression<B> *p = (GenericExpression<B> *) this->predicate_;
    JoinEqualityConditionVisitor<B> join_visitor(p->root_);
    join_idxs_  = join_visitor.getEqualities();


}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->getOutput();
    QueryTable<B> *rhs = this->getChild(1)->getOutput();

    this->output_ =  augmentTables(lhs, rhs);

    return this->output_;


}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {
    assert(lhs->storageModel() == rhs->storageModel());

    vector<int> lhs_keys, rhs_keys;

    for(auto key_pair : join_idxs_) {
        // visitor always outputs lhs, rhs
        assert(key_pair.first < lhs->getSchema().getFieldCount());
        assert(key_pair.second >= lhs->getSchema().getFieldCount());

        lhs_keys.emplace_back(key_pair.first);
        rhs_keys.emplace_back(key_pair.second - lhs->getSchema().getFieldCount());
    }


    // now have:
    // <join keys> <table_id> <payload> <alpha1> <alpha2>
    // if it is the smaller one, alphas go to the end - they were written earlier, but
    // the ones we're going to use as alphas are simply not initialized.

    QueryTable<B> *lhs_prime = projectSortKeyToFirstAttr(lhs, lhs_keys);
    QueryTable<B> *rhs_prime = projectSortKeyToFirstAttr(rhs, rhs_keys);

    // set up extended schema
    QuerySchema augmented_schema = (lhs_prime->tuple_size_ > rhs_prime->tuple_size_) ? lhs_prime->getSchema() : rhs_prime->getSchema();

    Field<B> table_id_field;
    if(std::is_same_v<B, bool>) {
        QueryFieldDesc alpha_1(augmented_schema.getFieldCount(), "alpha1", "", FieldType::INT, 0);
        augmented_schema.putField(alpha_1);
        QueryFieldDesc alpha_2(augmented_schema.getFieldCount(), "alpha1", "", FieldType::INT, 0);
        augmented_schema.putField(alpha_2);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field = Field<B>(FieldType::BOOL, true, 0);

    }
    else {
        QueryFieldDesc alpha_1(augmented_schema.getFieldCount(), "alpha1", "", FieldType::SECURE_INT, 0);
        augmented_schema.putField(alpha_1);
        QueryFieldDesc alpha_2(augmented_schema.getFieldCount(), "alpha1", "", FieldType::SECURE_INT, 0);
        augmented_schema.putField(alpha_2);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::SECURE_BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field = Field<B>( FieldType::SECURE_INT, Bit(true), 0);
    }

    augmented_schema.initializeFieldOffsets();

    int output_cursor = 0;
    QueryTable<B> *output = TableFactory<B>::getTable(lhs->getTupleCount() + rhs->getTupleCount(), augmented_schema, lhs->storageModel());
    for(int i = 0; i < lhs_prime->getTupleCount(); ++i) {
        output->cloneRow(output_cursor, 0, lhs_prime, i);
        ++output_cursor;
        output->setDummyTag(output_cursor, lhs_prime->getDummyTag(i));
    }

    for(int i = 0; i < rhs_prime->getTupleCount(); ++i) {
        output->cloneRow(output_cursor, 0, rhs_prime, i);
        output->setDummyTag(output_cursor, rhs_prime->getDummyTag(i));
        output->setField(output_cursor, augmented_schema.getFieldCount() - 1, table_id_field);
        ++output_cursor;
    }

    std::cout << "Concatenated output: " << output->getOstringStream() << endl;

    // implicitly deleting output table
    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size()); // join keys
    sort_def.emplace_back(augmented_schema.getFieldCount()-1, SortDirection::ASCENDING); // table_id ordinal
    Sort<B> sorter(output, sort_def);


    QueryTable<B> *sorted = sorter.run()->clone();

    initializeAlphas(sorted);

    return sorted;

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::projectSortKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols) {

    ExpressionMapBuilder<B> builder(src->getSchema());
    int write_cursor = 0;
    for(auto key : join_cols) {
        builder.addMapping(key, write_cursor);
        ++write_cursor;
    }



    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
        if(std::find(join_cols.begin(), join_cols.end(),i) == join_cols.end()) {
            builder.addMapping(i, write_cursor);
            ++write_cursor;
        }
    }

    Project<B> projection(src->clone(), builder.getExprs());
    return projection.run()->clone();
}


template<typename B>
void SortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {

    int table_id_idx = dst->getSchema().getFieldCount() - 1;
    int alpha_1_idx = dst->getSchema().getFieldCount() - 3; // 3rd to last field
    int alpha_2_idx = dst->getSchema().getFieldCount() - 2; // 2nd to last field

    Field<B> one = (dst->isEncrypted()) ? Field<B>(FieldType::SECURE_INT, 1, 0) : Field<B>(FieldType::INT, 1, 0);
    Field<B> zero = (dst->isEncrypted()) ? Field<B>(FieldType::SECURE_INT, 0, 0) : Field<B>(FieldType::INT, 0, 0);

    Field<B> count = zero;

    B prev_table_id =  dst->getField(0, table_id_idx).template getValue<B>();
    B table_id;
    // right now have table_id == true for rhs
    B fkey = (this->foreign_key_input_ == 0) ? B(false) : B(true);

    // set correct alpha to 1 for first row
    // if table ID is false, update alpha 1
    Field<B> a1 = Field<B>::If(!prev_table_id, one, zero);
    Field<B> a2 = Field<B>::If(prev_table_id, one, zero);
    dst->setField(0, alpha_1_idx, a1);
    dst->setField(0, alpha_2_idx, a2);

    for (int i = 1; i < dst->getTupleCount(); i++) {

        table_id = dst->getField(i, table_id_idx).template getValue<B>();
        B is_foreign_key = (table_id == fkey);

        B same_group = joinMatch(dst, i-1, i, join_idxs_.size());
        B result = ((table_id == prev_table_id) & same_group);

        count = count + one;
        count = Field<B>::If(result, count, one);

        Field<B> prev_count = Field<B>::If(same_group, dst->getField(i-1, alpha_1_idx),zero);
        dst->setField(i, alpha_1_idx, Field<B>::If(is_foreign_key, count, prev_count));
        dst->setField(i, alpha_2_idx, Field<B>::If(is_foreign_key, zero, count));


        prev_table_id = table_id;
    }

    for (int i = dst->getTupleCount() - 1; i >= 0; i--) {
        count = dst->getField(i, alpha_1_idx);

        B same_group = joinMatch(dst, i, i+1, join_idxs_.size());

        Field<B> prev_count = dst->getField(i+1, alpha_1_idx);
        dst->setField(i, alpha_1_idx, Field<B>::If(same_group, prev_count, count));

        count = dst->getField(i, alpha_2_idx);
        prev_count = dst->getField(i+1,alpha_2_idx);
        dst->setField(i, alpha_2_idx, Field<B>::If(same_group, prev_count, count));
    }


}


template class vaultdb::SortMergeJoin<bool>;
template class vaultdb::SortMergeJoin<emp::Bit>;

