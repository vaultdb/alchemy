#include "keyed_sort_merge_join.h"
#include "sort.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

#include <expression/visitor/join_equality_condition_visitor.h>

using namespace vaultdb;

template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key,
                                          const BoolExpression<B> & predicate, const SortDefinition & sort)  : Join<B>(foreign_key, primary_key, predicate, sort) {

    JoinEqualityConditionVisitor<B> join_visitor(Join<B>::predicate_.root_);
    equality_conditions_ = join_visitor.getEqualities();

}

template<typename B>
KeyedSortMergeJoin<B>::KeyedSortMergeJoin(shared_ptr<QueryTable<B>> foreign_key, shared_ptr<QueryTable<B>> primary_key,
                                          const BoolExpression<B> & predicate, const SortDefinition & sort) : Join<B>(foreign_key, primary_key, predicate, sort) {

}

template<typename B>
shared_ptr<QueryTable<B>> KeyedSortMergeJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > lhs = Join<B>::children_[0]->getOutput(); // fkey
    std::shared_ptr<QueryTable<B> > rhs = Join<B>::children_[1]->getOutput(); // pkey
    lhs_schema_ = lhs->getSchema();
    rhs_schema_ = rhs->getSchema();

    QuerySchema dst_schema = Join<B>::concatenateSchemas(*lhs->getSchema(), *rhs->getSchema(), true);

    size_t tuple_cnt = lhs->getTupleCount() + rhs->getTupleCount();

    Operator<B>::output_ = shared_ptr<QueryTable<B> >(new QueryTable<B>(tuple_cnt, dst_schema));
    writeLeftTuples(lhs, Operator<B>::output_);
    writeRightTuples(rhs, Operator<B>::output_);

    SortDefinition s;
    for(pair<uint32_t, uint32_t> p : equality_conditions_) {
        s.template emplace_back(p.first, SortDirection::ASCENDING);
    }

    // sort returns values ordered by sort key
    // first has pkey tuple, then fkey entries
    Sort<B> augmented(Operator<B>::output_, s);
    Operator<B>::output_ = augmented.run();

    distribute_values(Operator<B>::output_);

    // TODO: preserve original sort order where possible here.  Investigate fusing it with any sort op in parent
    SortDefinition  dummies_to_end{ColumnSort(-1, SortDirection::ASCENDING)};
    Sort<B> dummy_pushdown(Operator<B>::output_, dummies_to_end);
    Operator<B>::output_ = dummy_pushdown.run();

    Operator<B>::output_->resize(lhs->getTupleCount());
    return Operator<B>::output_;
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
void KeyedSortMergeJoin<B>::writeRightTuples(shared_ptr<QueryTable<B>> src_table, shared_ptr<QueryTable<B>> dst_table) {

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
         for(std::pair<uint32_t, uint32_t> p : equality_conditions_) {
             Field f = src_tuple.getField(p.second);
             dst_tuple.setField(p.first, f);
         }
    }



    }
// iterate over first sorted table and copy down values from primary key to foreign one
template<typename B>
void KeyedSortMergeJoin<B>::distributeValues(shared_ptr<QueryTable<bool> > dst_table) {
    // start with pkey data empty
    int table_id_idx = dst_table->getSchema()->getFieldCount() - 1;
    PlainTuple last_primary_key;
    bool pkey_init = false;

    size_t lhs_field_cnt = lhs_schema_.getFieldCount();
    size_t rhs_field_cnt = rhs_schema_.getFieldCount();

    for(uint32_t i = 0; i < dst_table->getTupleCount(); ++i) {
        PlainTuple d = dst_table->getTuple(i);
        bool table_id = d.getField(table_id_idx).template getValue<bool>();
        if(!table_id) { // primary key
            last_primary_key = d;
            d.setDummyTag(true); //
            pkey_init = true;
        }
        else { // copy down the latest value and then evaluating predicate
            if(pkey_init) {
                QueryTuple<B>::writeSubset(d, last_primary_key, lhs_field_cnt,  rhs_field_cnt, lhs_field_cnt);
                bool predicate_eval = Join<B>::predicate_.callBoolExpression(d);
                bool dummy_tag = Join<B>::get_dummy_tag(d, last_primary_key, predicate_eval);
                d.setDummyTag(dummy_tag);
            }
        }

    }

}

template<typename B>
void KeyedSortMergeJoin<B>::distributeValues(shared_ptr<QueryTable<emp::Bit> > dst_table) {
    //TODO:
    throw;
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