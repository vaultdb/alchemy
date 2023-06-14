#include "sort_merge_join.h"
#include "query_table/table_factory.h"
#include "operators/project.h"
#include "expression/visitor/join_equality_condition_visitor.h"
#include "operators/sort.h"
#include "util/data_utilities.h"

using namespace vaultdb;

template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *lhs, Operator<B> *rhs, Expression<B> *predicate,
                                const SortDefinition &sort) : Join<B>(lhs, rhs, predicate, sort) {



    JoinEqualityConditionVisitor<B> join_visitor(this->predicate.root_);
    join_idxs_  = join_visitor.getEqualities();



}


template<typename B>
SortMergeJoin<B>::SortMergeJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, Expression<B> *predicate,
              const SortDefinition &sort)  : Join<B>(lhs, rhs, predicate, sort) {

    JoinEqualityConditionVisitor<B> join_visitor(this->predicate.root_);
    join_idxs_  = join_visitor.getEqualities();


}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::runSelf() {
    QueryTable<B> *lhs = this->getChild(0)->output_;
    QueryTable<B> *rhs = this->getChild(1)->output_;

    this->output_ =  augmentTables(lhs, rhs);

    return this->output_;


}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::augmentTables(QueryTable<B> *lhs, QueryTable<B> *rhs) {
    QuerySchema lhs_schema = lhs->getSchema();
    QuerySchema rhs_schema = rhs->getSchema();
    assert(lhs->storageModel() == rhs->storageModel());

    QuerySchema augmented_schema = (lhs->tuple_size_ > rhs->tuple_size_) ? lhs_schema : rhs_schema;

    Field<B> table_id_field;
    int out_schema_size = max(lhs->tuple_size_, rhs->tuple_size_);
    if(std::is_same_v<B, bool>) {
        out_schema_size += 1 + 8; // 2 alphas @ 32 bits, 1 bool
        QueryFieldDesc alpha_1(augmented_schema.getFieldCount(), "alpha1", "", FieldType::INT, 0);
        augmented_schema.putField(alpha_1);
        QueryFieldDesc alpha_2(augmented_schema.getFieldCount(), "alpha1", "", FieldType::INT, 0);
        augmented_schema.putField(alpha_2);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field = Field<B>(true, FieldType::BOOL, 0);

    }
    else {
        out_schema_size += 65 * TypeUtilities::getEmpBitSize();
        QueryFieldDesc alpha_1(augmented_schema.getFieldCount(), "alpha1", "", FieldType::SECURE_INT, 0);
        augmented_schema.putField(alpha_1);
        QueryFieldDesc alpha_2(augmented_schema.getFieldCount(), "alpha1", "", FieldType::SECURE_INT, 0);
        augmented_schema.putField(alpha_2);
        QueryFieldDesc table_id(augmented_schema.getFieldCount(), "table_id", "", FieldType::SECURE_BOOL, 0);
        augmented_schema.putField(table_id);
        table_id_field = Field<B>(Bit(true), FieldType::SECURE_INT, 0);
    }

    augmented_schema.initializeFieldOffsets();
    vector<int> lhs_keys, rhs_keys;

    // Does not check that both keys belong to different tables
    for(auto key_pair : join_idxs_) {
        int k1 = key_pair.first;
        bool lhs_key = false, rhs_key = false;
        if(k1 < lhs_schema.getFieldCount()) {
            lhs_keys.emplace_back(k1);
            lhs_key = true;
        }
        else {
            rhs_keys.emplace_back(k1 - lhs_schema.getFieldCount());
            rhs_key = true;
        }

        int k2 = key_pair.first;
        if(k2 < lhs_schema.getFieldCount()) {
            lhs_keys.emplace_back(k2);
            lhs_key = true;

        }
        else {
            rhs_keys.emplace_back(k2 - lhs_schema.getFieldCount());
            rhs_key = true;
        }

    }


    // now have:
    // <join keys> <table_id> <payload> <alpha1> <alpha2>
    // if it is the smaller one, alphas go to the end - they were written earlier, but
    // the ones we're going to use as alphas are simply not initialized.

    QueryTable<B> *lhs_prime = projectSortKeyToFirstAttr(lhs, lhs_keys);
    QueryTable<B> *rhs_prime = projectSortKeyToFirstAttr(rhs, rhs_keys);

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


    // implicitly deleting output table
    SortDefinition  sort_def = DataUtilities::getDefaultSortDefinition(join_idxs_.size() + 1);
    Sort<B> sorter(output, sort_def);


    QueryTable<B> *sorted = sorter.run()->clone();

    initializeAlphas(sorted);

    return sorted;

}

template<typename B>
QueryTable<B> *SortMergeJoin<B>::projectSortKeyToFirstAttr(QueryTable<B> *src, vector<int> join_cols) {

    vector<int> cols_covered = join_cols;

    ExpressionMapBuilder builder(src->getSchema());
    int write_cursor = 0;
    for(auto key : join_cols) {
        builder.addMapping(key, write_cursor);
        ++write_cursor;
    }

    int table_col_id = src->getSchema().getField("table_id").getOrdinal();
    // move up table_id to be after keys
    builder.addMapping(table_col_id, write_cursor);
    ++write_cursor;
    cols_covered.emplace_back(table_col_id);


    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
        if(std::find(cols_covered.begin(), cols_covered.end(),i) == cols_covered.end()) {
            builder.addMapping(i, write_cursor);
            ++write_cursor;
        }
    }

    Project<B> projection(src->clone(), builder.getExprs());
    return projection.run()->clone();
}


template<typename B>
void SortMergeJoin<B>::initializeAlphas(QueryTable<B> *dst) {

    int table_id_ordinal = dst->getSchema().getField("table_id");
    int alpha_1_ordinal = dst->getSchema().getFieldCount() - 2; // second to last field
    int alpha_2_ordinal = dst->getSchema().getFieldCount() - 1; // last field

    Field<B> one = (dst->isEncrypted()) ? Field<B>(FieldType::SECURE_INT, 1, 0) : Field<B>(FieldType::INT, 1, 0);
    Field<B> zero = (dst->isEncrypted()) ? Field<B>(FieldType::SECURE_INT, 0, 0) : Field<B>(FieldType::INT, 0, 0);

    B prev_table_id = dst->getField(0, table_id_ordinal);
    for (int i = 1; i < dst->getTupleCount(); i++) {
        Field<B> table_id = dst->getField(i, table_id_ordinal);
        B is_foreign_key = table_id.template getValue<B>();

        // do they have the same join key?
        B same_group = joinMatch(dst, i - 1, i, join_idxs_.size());
        B result = ((dst->getField(i, table_id) == dst->getField(i - 1, table_id)) & same_group);

        count = count + one;
        count = Field<B>::If(result, count, one);

        Field<B> prev_count = Field<B>::If(same_group, dst->getField(i - 1, alpha_1_ordinal), zero);
        dst->setField(i, alpha_1_ordinal, Field<B>::If(is_foreign_key, count, prev_count));
        dst->setField(i, alpha_2_ordinal, Field<B>::If(is_foreign_key, zero, count));

        prev_table_id = table_id;
    }

    for (int i = dst->getTupleCount() - 2; i >= 0; i--) {
        count = dst->getField(i, alpha_1_ordinal);
        B same_group = joinMatch(dst, i, i + 1, join_idxs_.size());

        Field<B> prev_count = dst->getField(i + 1, alpha_1_ordinal);
        dst->setField(i, alpha_1_ordinal, Field<B>::If(same_group, prev_count, count));

        count = dst->getField(i, alpha_2_ordinal);
        prev_count = dst->getField(i + 1, alpha_2_ordinal);
        dst->setField(i, alpha_2_ordinal, Field<B>::If(same_group, prev_count, count));

    }
}