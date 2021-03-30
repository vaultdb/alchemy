#include "fkey_pkey_join.h"

template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> *foreignKey, Operator<B> *primaryKey, shared_ptr<BinaryPredicate<B> > predicateClass)
        : Join<B>(foreignKey, primaryKey, predicateClass) {}

template<typename B>
KeyedJoin<B>::KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, shared_ptr<BinaryPredicate<B> > predicateClass)
        : Join<B>(foreignKey, primaryKey, predicateClass) {}



template<typename B>
std::shared_ptr<QueryTable<B> > KeyedJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > foreign_key_table = Join<B>::children[0]->getOutput();
    std::shared_ptr<QueryTable<B> > primary_key_table = Join<B>::children[1]->getOutput();
    QueryTuple<B> lhs_tuple, rhs_tuple;
    QueryTuple<B> dst_tuple;

    uint32_t output_tuple_cnt = foreign_key_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = *foreign_key_table->getSchema();
    QuerySchema rhs_schema = *primary_key_table->getSchema();
    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema);



    assert(foreign_key_table->isEncrypted() == primary_key_table->isEncrypted()); // only support all plaintext or all MPC

    // output size, colCount, isEncrypted
    Join<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(output_tuple_cnt, output_schema));

    // TODO: create dst tuple and populate with LHS value
    // TODO: (cont'd) then RHS overwrites only the second half
    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < foreign_key_table->getTupleCount(); ++i) {
        lhs_tuple = (*foreign_key_table)[i];

        dst_tuple = Join<B>::output->getTuple(i);

        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        rhs_tuple = (*primary_key_table)[0];
        B predicateEval = Join<B>::predicate->predicateCall(lhs_tuple, rhs_tuple);
        B dst_dummy_tag = Join<B>::compareTuples(lhs_tuple, rhs_tuple, predicateEval);
        dst_tuple.setDummyTag(dst_dummy_tag);

        // unconditional write to first one to initialize it
        Join<B>::write_left(dst_dummy_tag, dst_tuple, lhs_tuple);
        Join<B>::write_right(dst_dummy_tag, dst_tuple, rhs_tuple);

        for(uint32_t j = 1; j < primary_key_table->getTupleCount(); ++j) {
            rhs_tuple = (*primary_key_table)[j];
            predicateEval = Join<B>::predicate->predicateCall(lhs_tuple, rhs_tuple);
             B this_dummy_tag = Join<B>::compareTuples(lhs_tuple, rhs_tuple, predicateEval);
             // only write if we haven't written before,
             // otherwise dummies after the first true write will clobber it
            this_dummy_tag = this_dummy_tag & dst_dummy_tag;
            Join<B>::write_right(this_dummy_tag, dst_tuple, rhs_tuple);
            dst_dummy_tag = this_dummy_tag;

        }

    }
    return Join<B>::output;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;


