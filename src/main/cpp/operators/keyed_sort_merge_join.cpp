#include "keyed_sort_merge_join.h"

#include <operators/support/join_equality_condition_visitor.h>

using namespace vaultdb;

template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key,
                                          const BoolExpression<B> & predicate)  : Join<B>(foreign_key, primary_key, predicate) {

}

template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(shared_ptr<QueryTable<B>> foreign_key, shared_ptr<QueryTable<B>> primary_key,
                                          const BoolExpression<B> & predicate) : Join<B>(foreign_key, primary_key, predicate) {

}

template<typename B>
shared_ptr<QueryTable<B>> KeyedSortMergeJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > lhs = Join<B>::children[0]->getOutput(); // fkey
    std::shared_ptr<QueryTable<B> > rhs = Join<B>::children[1]->getOutput(); // pkey
    JoinEqualityConditionVisitor<B> join_visitor(Join<B>::predicate_.root_);
    std::vector<std::pair<uint32_t, uint32_t> > equality_conditions = join_visitor.getEqualities();

    QuerySchema dst_schema = Join<B>::concatenateSchemas(*lhs->getSchema(), *rhs->getSchema(), true);

    size_t tuple_cnt = lhs->getTupleCount() + rhs->getTupleCount();

    shared_ptr<QueryTable<B> > dst_table(new QueryTable<B>(tuple_cnt, dst_schema));
    writeLeftTuples(lhs, dst_table);
    writeRightTuples(rhs, dst_table, std::vector<std::pair<uint32_t, uint32_t>>());

    // coalesce lhs_join_key and rhs_join_key into lhs_join_key slot(s)
    // sort by (lhs_join_key), table_id
    // TODO: extract equality predicates by walking the expression tree
    // need equality visitor for this
    // need to implement visit<T> method where T is the return type
    //

}

// for each tuple in lhs/fkey side, just copy left, set table_id bit to 1
template<typename B>
void KeyedSortMergeJoin<B>::writeLeftTuples(shared_ptr<QueryTable<B>> lhs, shared_ptr<QueryTable<B>> dst_table) {
            size_t table_id_idx = dst_table->getSchema()->getColumnCount() - 1;

            FieldType table_id_type = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
            Value table_id = (B) 1;
            Field<B> table_id_field(table_id_type, table_id, 0);

            for(size_t i = 0; i < lhs->getTupleCount(); ++i) {
                    QueryTuple<B> src_tuple = lhs->getTuple(i);
                    QueryTuple<B> dst_tuple = dst_table->getTuple(i);

                    Join<B>::write_left(true, src_tuple, dst_tuple);
                    dst_tuple.setDummyTag(src_tuple.getDummyTag());
                    dst_tuple.setField(table_id_idx, table_id_field);
                }
}


// for each one in rhs/pkey side, pad it with a table_id bit, set it to 0, then copy_right
template<typename B>
void KeyedSortMergeJoin<B>::writeRightTuples(shared_ptr<QueryTable<B>> src_table, shared_ptr<QueryTable<B>> dst_table,
                                             std::vector<std::pair<uint32_t, uint32_t> > equality_conditions) {

    FieldType table_id_type = (std::is_same_v<B, bool>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
    Value table_id = (B) 0;
    Field<B> table_id_field(table_id_type, table_id, 0);

    // augment the src schema with an extra bool for table_id
    QuerySchema src_schema = *src_table->getSchema();
    size_t table_id_idx = src_schema.getFieldCount();
    QuerySchema src_schema_augmented(table_id_idx + 1);

    for(size_t i = 0; i < src_schema.getFieldCount(); ++i) {
        QueryFieldDesc f = src_schema.getField(i);
        src_schema_augmented.putField(f);
    }

    QueryFieldDesc bool_field(table_id_idx, "table_id", "", table_id_type, 0);
    src_schema_augmented.putField(bool_field);

    QueryTuple<B> src_tuple, dst_tuple;


    for(size_t i = 0; i < src_table->getTupleCount(); ++i) {
         src_tuple = src_table->getTuple(i);
        QueryTuple<B> src_tuple_augmented(src_schema_augmented);
         memcpy(src_tuple_augmented.getData(), src_tuple.getData(), src_table->tuple_size_);
         src_tuple_augmented.setField(table_id_idx, table_id_field);

         dst_tuple = dst_table->getTuple(i);
         Join<B>::write_right(true, dst_tuple, src_tuple_augmented);
         dst_tuple.setDummyTag(src_tuple.getDummyTag());

         // copy out the value into its equality position on the rhs.  Use this for sorting
         for(std::pair<uint32_t, uint32_t> p : equality_conditions) {
             Field f = src_tuple.getField(p.second);
             dst_tuple.setField(p.first, f);
         }
    }



    }


// pad both sides so each tuple is the same length
// union together the two sides with one extra column indicating source table
// sort on join key, pkey rows go first
// count examples of current key with running count in new field
// if hit primary key, write down its muliplicity
// then create a new, augmented version of primary key side, its length equal to that of fkey reln
// 3 cases:
// pkey-fkey match: simply write multiplicity keys
// key exists on fkey side only, write a dummy on corresponding lhs cells

// make this easier - simply create output length tuples - concat primary key/foreign key side schema
// sort both sides on join key, pkey side goes first
// make a single pass, propagating most recent join key if match, o.w. mark dummy
// final output card: |fkey|
// last step sorts the output, truncates dummy values to realize |fkey| output card.