#include "keyed_join.h"

#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>


#include <util/data_utilities.h>
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> *foreign_key, Operator<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreign_key, primary_key, predicate, sort) {}


// fkey = 0 --> lhs, fkey = 1 --> rhs
template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> * lhs, Operator<B> * rhs, const int & fkey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
    assert(fkey == 0 || fkey == 1);
}

template<typename B>
KeyedJoin<B>::KeyedJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreign_key, primary_key, predicate, sort) {}

template<typename B>
KeyedJoin<B>::KeyedJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
    assert(fkey == 0 || fkey == 1);
}


template<typename B>
QueryTable<B> *KeyedJoin<B>::runSelf() {


    if(foreign_key_input_ == 0){
        return foreignKeyPrimaryKeyJoin();
    }

    return primaryKeyForeignKeyJoin();
}

template<typename B>
string KeyedJoin<B>::getOperatorType() const {
    return "KeyedJoin";
}

template<typename B>
QueryTable<B> *KeyedJoin<B>::foreignKeyPrimaryKeyJoin() {

    QueryTable<B> *lhs_table = Operator<B>::getChild(0)->getOutput(); // foreign key
    QueryTable<B> *rhs_table = Operator<B>::getChild(1)->getOutput(); // primary key

    uint32_t output_tuple_cnt = lhs_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = lhs_table->getSchema();
    QuerySchema rhs_schema = rhs_table->getSchema();
    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);
    Join<B>::output_ = new QueryTable<B>(output_tuple_cnt, output_schema, lhs_table->getSortOrder());
    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;

    QueryTuple<B> joined(&output_schema);

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < lhs_table->getTupleCount(); ++i) {
        lhs_dummy_tag = lhs_table->getDummyTag(i);


          Join<B>::write_left(Join<B>::output_, i, lhs_table, i);
          Join<B>::write_left(joined, lhs_table, i);


          dst_dummy_tag = true; // dummy by default, no tuple comparisons yet

        for(uint32_t j = 0; j < rhs_table->getTupleCount(); ++j) {
            rhs_dummy_tag = rhs_table->getDummyTag(j);
            Join<B>::write_right(joined, rhs_table, j);

            selected = Join<B>::predicate_->call(joined).template getValue<B>();
            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);

            Join<B>::write_right(to_update, Operator<B>::output_, i, rhs_table, j);
            Operator<B>::output_->setDummyTag(i, dst_dummy_tag);
        }
    }

    return Join<B>::output_;

}

template<typename B>
QueryTable<B> *KeyedJoin<B>::primaryKeyForeignKeyJoin() {

    QueryTable<B> *lhs_table = Operator<B>::getChild(0)->getOutput(); // primary key
    QueryTable<B> *rhs_table = Operator<B>::getChild(1)->getOutput(); // foreign key

    uint32_t output_tuple_cnt = rhs_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = lhs_table->getSchema();
    QuerySchema rhs_schema = rhs_table->getSchema();


    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);
    QueryTuple<B> joined(&output_schema);

    SortDefinition output_sort;
    for(ColumnSort s : rhs_table->getSortOrder()) {
        ColumnSort s_prime(s.first + lhs_table->getSchema().getFieldCount(), s.second);
        output_sort.template emplace_back(s_prime);
    }

    Join<B>::output_ = new QueryTable<B>(output_tuple_cnt, output_schema, output_sort);
    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < rhs_table->getTupleCount(); ++i) {
        rhs_dummy_tag = rhs_table->getDummyTag(i);

        Join<B>::write_right(Operator<B>::output_, i, rhs_table, i);
        Join<B>::write_right(joined, rhs_table, i);


        dst_dummy_tag = B(true); // no comparisons yet, so it is a dummy by default


        for(uint32_t j = 0; j < lhs_table->getTupleCount(); ++j) {

            lhs_dummy_tag = lhs_table->getDummyTag(j);
            Join<B>::write_left(joined, lhs_table, j);

            selected = Join<B>::predicate_->call(joined).template getValue<B>();

            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);


            Join<B>::write_left(to_update, Operator<B>::output_, i, lhs_table, j);
            Operator<B>::output_->setDummyTag(i, dst_dummy_tag);

        }
    }



    return Join<B>::output_;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;

