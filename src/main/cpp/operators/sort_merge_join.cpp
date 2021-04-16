#include "sort_merge_join.h"

using namespace vaultdb;

template<typename B>
SortMergeJoin<B>::SortMergeJoin(Operator<B> *foreign_key, Operator<B> *primary_key,
                                const shared_ptr<BinaryPredicate<B>> &predicateClass)  : Join<B>(foreign_key, primary_key, predicateClass) {

}

template<typename B>
SortMergeJoin<B>::SortMergeJoin(shared_ptr<QueryTable<B>> foreign_key, shared_ptr<QueryTable<B>> primary_key,
                                shared_ptr<BinaryPredicate<B>> predicate_class) : Join<B>(foreign_key, primary_key, predicate_class) {

}

template<typename B>
shared_ptr<QueryTable<B>> SortMergeJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > lhs = Join<B>::children[0]->getOutput(); // fkey
    std::shared_ptr<QueryTable<B> > rhs = Join<B>::children[1]->getOutput(); // pkey

    QuerySchema output_schema = Join<B>::concatenateSchemas(*lhs->getSchema(), *rhs->getSchema(), true);



}

template<typename B>
shared_ptr<QueryTable<B>>
SortMergeJoin<B>::augmentTable(shared_ptr<SecureTable> &src_table, const QuerySchema &dst_schema,
                               const bool &foreign_key) {
    size_t tuple_cnt = src_table->getTupleCount();
    shared_ptr<PlainTable> dst_table(new PlainTable(tuple_cnt, dst_schema));
    size_t write_offset = 0; // fkey case
    if(!foreign_key)
        write_offset = dst_schema.size() - src_table->tuple_size_; // table_id and extra dummy tag cancel each other out

    Bit *src_ptr = (Bit *) src_table->tuple_data_.data();
    Bit *dst_ptr = (Bit *) src_table->tuple_data_.data() + write_offset;
    Bit table_id(foreign_key);
    int32_t read_dummy_offset = src_table->getSchema()->getFieldOffset(-1);
    int32_t write_dummy_offset = dst_table->getSchema()->getFieldOffset(-1);

    size_t src_tuple_size = src_table->tuple_size_; // in bytes
    size_t dst_tuple_size = dst_table->tuple_size_;
    for(size_t i = 0; i < tuple_cnt; ++i) {
        memcpy(dst_ptr, src_ptr, src_tuple_size);
        // copy out the dummy tag
        *(dst_ptr + write_dummy_offset) = *(src_ptr + read_dummy_offset);
        // write to table_id
        *(dst_ptr + write_dummy_offset - 1) = table_id;

        src_ptr += src_tuple_size;
        dst_ptr += dst_tuple_size;

    }

    return dst_table;

}

template<typename B>
shared_ptr<QueryTable<B>>
SortMergeJoin<B>::augmentTable(shared_ptr<PlainTable> &src_table, const QuerySchema &dst_schema,
                               const bool &foreign_key) {
}

// pad both sides so each tuple is the same length
// union together the two sides with one extra column indicating source table
// sort on join key, fkey rows go first
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