#include "keyed_join.h"

#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>
#include <query_table/plain_tuple.h>

#include <util/data_utilities.h>
#include <util/field_utilities.h>
#include "query_table/table_factory.h"

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

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = emp::CircuitExecution::circ_exec->num_and();

    uint32_t output_tuple_cnt = lhs_table->getTupleCount(); // foreignKeyTable = foreign key
    this->output_ = TableFactory<B>::getTable(output_tuple_cnt, this->output_schema_, lhs_table->storageModel(), lhs_table->getSortOrder());

    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;


    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < lhs_table->getTupleCount(); ++i) {
        lhs_dummy_tag = lhs_table->getDummyTag(i);
        Join<B>::write_left(Join<B>::output_, i, lhs_table, i);
        dst_dummy_tag = true; // dummy by default, no tuple comparisons yet

        for(uint32_t j = 0; j < rhs_table->getTupleCount(); ++j) {
            rhs_dummy_tag = rhs_table->getDummyTag(j);

            selected = Join<B>::predicate_->call(lhs_table, i, rhs_table, j).template getValue<B>();

            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);

            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);

            Join<B>::write_right(to_update, Operator<B>::output_, i, rhs_table, j);
            Operator<B>::output_->setDummyTag(i, dst_dummy_tag);


        }

    }

    return this->output_;

}

template<typename B>
QueryTable<B> *KeyedJoin<B>::primaryKeyForeignKeyJoin() {

    QueryTable<B> *lhs_table = Operator<B>::getChild(0)->getOutput(); // primary key
    QueryTable<B> *rhs_table = Operator<B>::getChild(1)->getOutput(); // foreign key

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = emp::CircuitExecution::circ_exec->num_and();

    uint32_t output_tuple_cnt = rhs_table->getTupleCount(); // foreignKeyTable = foreign key

    SortDefinition output_sort;
    for(ColumnSort s : rhs_table->getSortOrder()) {
        ColumnSort s_prime(s.first + lhs_table->getSchema().getFieldCount(), s.second);
        output_sort.template emplace_back(s_prime);
    }

    this->output_ = TableFactory<B>::getTable(output_tuple_cnt, this->output_schema_, lhs_table->storageModel(), output_sort);
    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < rhs_table->getTupleCount(); ++i) {
        rhs_dummy_tag = rhs_table->getDummyTag(i);
        Join<B>::write_right(Operator<B>::output_, i, rhs_table, i);
        dst_dummy_tag = B(true); // no comparisons yet, so it is a dummy by default


        for(uint32_t j = 0; j < lhs_table->getTupleCount(); ++j) {
            lhs_dummy_tag = lhs_table->getDummyTag(j);


            selected = Join<B>::predicate_->call(lhs_table, j, rhs_table, i).template getValue<B>();

            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);

            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);


            Join<B>::write_left(to_update, Operator<B>::output_, i, lhs_table, j);
            Operator<B>::output_->setDummyTag(i, dst_dummy_tag);

        }

    }



    return this->output_;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;

