#include "keyed_join.h"
#include "secure_tuple.h" // needed for specialization
#include "plain_tuple.h"

#include <util/data_utilities.h>
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> *foreignKey, Operator<B> *primaryKey, const BoolExpression<B> & predicate, const SortDefinition & sort)
        : Join<B>(foreignKey, primaryKey, predicate, sort) {}


// fkey = 0 --> lhs, fkey = 1 --> rhs
template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> * lhs, Operator<B> * rhs, const int & fkey, const BoolExpression<B> & predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), forign_key_input_(fkey) {
    assert(fkey == 0 || fkey == 1);
}

 template<typename B>
KeyedJoin<B>::KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, const BoolExpression<B> & predicate, const SortDefinition & sort)
        : Join<B>(foreignKey, primaryKey, predicate, sort) {}

template<typename B>
KeyedJoin<B>::KeyedJoin(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs, const int & fkey, const BoolExpression<B> & predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), forign_key_input_(fkey) {
            assert(fkey == 0 || fkey == 1);
        }


template<typename B>
std::shared_ptr<QueryTable<B> > KeyedJoin<B>::runSelf() {

    std::shared_ptr<QueryTable<B> > lhs_table = Join<B>::children_[0]->getOutput();
    std::shared_ptr<QueryTable<B> > rhs_table = Join<B>::children_[1]->getOutput();

    if(forign_key_input_ == 0){
        return foreignKeyPrimaryKeyJoin();
    }

    return primaryKeyForeignKeyJoin();
}

template<typename B>
string KeyedJoin<B>::getOperatorType() const {
    return "KeyedJoin";
}

template<typename B>
shared_ptr<QueryTable<B>> KeyedJoin<B>::foreignKeyPrimaryKeyJoin() {

    std::shared_ptr<QueryTable<B> > lhs_table = Join<B>::children_[0]->getOutput(); // foreign key
    std::shared_ptr<QueryTable<B> > rhs_table = Join<B>::children_[1]->getOutput(); // primary key
    QueryTuple<B> lhs_tuple(*lhs_table->getSchema()), rhs_tuple(*rhs_table->getSchema());

    uint32_t output_tuple_cnt = lhs_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = *lhs_table->getSchema();
    QuerySchema rhs_schema = *rhs_table->getSchema();
    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);
    Join<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(output_tuple_cnt, output_schema, lhs_table->getSortOrder()));

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < lhs_table->getTupleCount(); ++i) {
        lhs_tuple = (*lhs_table)[i];

        QueryTuple<B> dst_tuple = Join<B>::output_->getTuple(i);

        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        rhs_tuple = (*rhs_table)[0];

        B predicate_eval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);
        B dst_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicate_eval);

        // unconditional write to first one to initialize it
        Join<B>::write_left(true, dst_tuple, lhs_tuple);
        Join<B>::write_right(true, dst_tuple, rhs_tuple);
        dst_tuple.setDummyTag(dst_dummy_tag);

        for(uint32_t j = 1; j < rhs_table->getTupleCount(); ++j) {
            rhs_tuple = (*rhs_table)[j];
            predicate_eval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);
            B this_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicate_eval);

            Join<B>::write_right(!this_dummy_tag, dst_tuple, rhs_tuple);
            Join<B>::update_dummy_tag(dst_tuple, predicate_eval, this_dummy_tag);
        }



    }

    return Join<B>::output_;

}

template<typename B>
shared_ptr<QueryTable<B>> KeyedJoin<B>::primaryKeyForeignKeyJoin() {
    std::shared_ptr<QueryTable<B> > lhs_table = Join<B>::children_[0]->getOutput(); // primary key
    std::shared_ptr<QueryTable<B> > rhs_table = Join<B>::children_[1]->getOutput(); // foreign key
    QueryTuple<B> lhs_tuple(*lhs_table->getSchema()), rhs_tuple(*rhs_table->getSchema());

    uint32_t output_tuple_cnt = rhs_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = *lhs_table->getSchema();
    QuerySchema rhs_schema = *rhs_table->getSchema();
    size_t lhs_attribute_cnt = lhs_schema.getFieldCount();
    size_t rhs_attribute_cnt = rhs_schema.getFieldCount();


    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);
    QueryTuple<B> joined(output_schema); // create a tuple with self-managed storage

    SortDefinition output_sort;
    for(ColumnSort s : rhs_table->getSortOrder()) {
        ColumnSort s_prime(s.first + lhs_table->getSchema()->getFieldCount(), s.second);
        output_sort.template emplace_back(s_prime);
    }

    // output size, colCount, is_encrypted
    Join<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(output_tuple_cnt, output_schema, output_sort));

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < rhs_table->getTupleCount(); ++i) {
        rhs_tuple = (*rhs_table)[i];

        QueryTuple<B> dst_tuple = Join<B>::output_->getTuple(i);

        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        lhs_tuple = (*lhs_table)[0];

        B predicate_eval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);
        B dst_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicate_eval);

        // unconditional write to first one to initialize it
        QueryTuple<B>::writeSubset(lhs_tuple, dst_tuple, 0, lhs_attribute_cnt, 0);
        QueryTuple<B>::writeSubset(rhs_tuple, dst_tuple, 0, rhs_attribute_cnt, lhs_attribute_cnt);
        dst_tuple.setDummyTag(dst_dummy_tag);


        for(uint32_t j = 1; j < lhs_table->getTupleCount(); ++j) {
            lhs_tuple = (*lhs_table)[j];

            QueryTuple<B>::writeSubset(lhs_tuple, joined, 0, lhs_attribute_cnt, 0);
            QueryTuple<B>::writeSubset(rhs_tuple, joined, 0, rhs_attribute_cnt, lhs_attribute_cnt);


            predicate_eval = Join<B>::predicate_.callBoolExpression(joined);
            dst_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicate_eval);

            Join<B>::write_left(!dst_dummy_tag, dst_tuple, lhs_tuple);
            Join<B>::update_dummy_tag(dst_tuple, predicate_eval, dst_dummy_tag);

        }

    }


    return Join<B>::output_;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;


