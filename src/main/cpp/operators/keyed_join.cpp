#include "keyed_join.h"
#include "secure_tuple.h"

using namespace vaultdb;

template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> *foreignKey, Operator<B> *primaryKey, const BoolExpression<B> & predicate, const SortDefinition & sort)
        : Join<B>(foreignKey, primaryKey, predicate, sort) {}

template<typename B>
KeyedJoin<B>::KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const BoolExpression<B> & predicate, const SortDefinition & sort)
        : Join<B>(foreignKey, primaryKey, predicate, sort) {}



template<typename B>
std::shared_ptr<QueryTable<B> > KeyedJoin<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > foreign_key_table = Join<B>::children_[0]->getOutput();
    std::shared_ptr<QueryTable<B> > primary_key_table = Join<B>::children_[1]->getOutput();
    QueryTuple<B> lhs_tuple(*foreign_key_table->getSchema()), rhs_tuple(*primary_key_table->getSchema());

    uint32_t output_tuple_cnt = foreign_key_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = *foreign_key_table->getSchema();
    QuerySchema rhs_schema = *primary_key_table->getSchema();
    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);


    assert(foreign_key_table->isEncrypted() == primary_key_table->isEncrypted()); // only support all plaintext or all MPC

    // output size, colCount, is_encrypted
    Join<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(output_tuple_cnt, output_schema, foreign_key_table->getSortOrder()));

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < foreign_key_table->getTupleCount(); ++i) {
        lhs_tuple = (*foreign_key_table)[i];

        QueryTuple<B> dst_tuple = Join<B>::output_->getTuple(i);

        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        rhs_tuple = (*primary_key_table)[0];

        B predicateEval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);
        B dst_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicateEval);

        // unconditional write to first one to initialize it
        Join<B>::write_left(true, dst_tuple, lhs_tuple);
        Join<B>::write_right(true, dst_tuple, rhs_tuple);
        dst_tuple.setDummyTag(dst_dummy_tag);

        for(uint32_t j = 1; j < primary_key_table->getTupleCount(); ++j) {
            rhs_tuple = (*primary_key_table)[j];

            predicateEval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);

            B this_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicateEval);
            Join<B>::write_right(!this_dummy_tag, dst_tuple, rhs_tuple);
        }



    }
    return Join<B>::output_;

}

template<typename B>
string KeyedJoin<B>::getOperatorType() const {
    return "KeyedJoin";
}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;


