#include "keyed_join.h"

#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include "query_table/query_table.h"

#include <util/data_utilities.h>
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> *foreign_key, Operator<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreign_key, primary_key, predicate, sort) {
            this->output_cardinality_ = foreign_key->getOutputCardinality();
            this->updateCollation();
        }


// fkey = 0 --> lhs, fkey = 1 --> rhs
template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> * lhs, Operator<B> * rhs, const int & fkey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
    assert(fkey == 0 || fkey == 1);
    this->output_cardinality_ = (fkey == 0) ? lhs->getOutputCardinality() : rhs->getOutputCardinality();
    this->updateCollation();
}

template<typename B>
KeyedJoin<B>::KeyedJoin(QueryTable<B> *foreign_key, QueryTable<B> *primary_key, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreign_key, primary_key, predicate, sort) {

            this->output_cardinality_ = foreign_key->getTupleCount();
            this->updateCollation();
}

template<typename B>
KeyedJoin<B>::KeyedJoin(QueryTable<B> *lhs, QueryTable<B> *rhs, const int & fkey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
    assert(fkey == 0 || fkey == 1);
    this->output_cardinality_ = (fkey == 0) ? lhs->getTupleCount() : rhs->getTupleCount();
    this->updateCollation();
}


template<typename B>
QueryTable<B> *KeyedJoin<B>::runSelf() {

    if(foreign_key_input_ == 0){
        return foreignKeyPrimaryKeyJoin();
    }

    return primaryKeyForeignKeyJoin();
}


template<typename B>
QueryTable<B> *KeyedJoin<B>::foreignKeyPrimaryKeyJoin() {

    QueryTable<B> *lhs_table = this->getChild(0)->getOutput(); // foreign key
    lhs_table->pinned_ = true;
    QueryTable<B> *rhs_table = this->getChild(1)->getOutput(); // primary key

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();


    uint32_t output_tuple_cnt = lhs_table->getTupleCount(); // foreignKeyTable = foreign key
    this->output_ = new QueryTable<B>(output_tuple_cnt, this->output_schema_, this->sort_definition_);

    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;
    int rhs_col_offset = this->output_->getSchema().getFieldCount() - rhs_table->getSchema().getFieldCount();
    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < lhs_table->getTupleCount(); ++i) {

        lhs_dummy_tag = lhs_table->getField(i, -1).template getValue<B>();
        this->output_->cloneRow(i, 0, lhs_table, i); // Join<B>::write_left(this->output_, i, lhs_table, i);
        dst_dummy_tag = true; // dummy by default, no matches found yet

        for(uint32_t j = 0; j < rhs_table->getTupleCount(); ++j) {
            rhs_dummy_tag = rhs_table->getField(j, -1).template getValue<B>();
            selected = Join<B>::predicate_->call(lhs_table, i, rhs_table, j).template getValue<B>();

            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);
            this->output_->cloneRow(to_update, i, rhs_col_offset, rhs_table, j);   //Join<B>::write_right(to_update, Operator<B>::output_, i, rhs_table, j);
       }

        this->output_->setDummyTag(i, dst_dummy_tag);

    }

    lhs_table->pinned_ = false; // operator will automatically delete children
    return this->output_;

}

template<typename B>
QueryTable<B> *KeyedJoin<B>::primaryKeyForeignKeyJoin() {

    QueryTable<B> *lhs_table = Operator<B>::getChild(0)->getOutput(); // primary key
    lhs_table->pinned_ = true;
    QueryTable<B> *rhs_table = Operator<B>::getChild(1)->getOutput(); // foreign key

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();



    uint32_t output_tuple_cnt = rhs_table->getTupleCount(); // foreignKeyTable = foreign key

    this->output_ = new QueryTable<B>(output_tuple_cnt, this->output_schema_, this->sort_definition_);
    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;
    int rhs_col_offset = this->output_->getSchema().getFieldCount() - rhs_table->getSchema().getFieldCount();

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < rhs_table->getTupleCount(); ++i) {
        rhs_dummy_tag = rhs_table->getDummyTag(i);
        this->output_->cloneRow(i, rhs_col_offset, rhs_table, i);  // Join<B>::write_right(Operator<B>::output_, i, rhs_table, i);
        dst_dummy_tag = B(true); // no comparisons yet, so it is a dummy by default

        for(uint32_t j = 0; j < lhs_table->getTupleCount(); ++j) {
            lhs_dummy_tag = lhs_table->getDummyTag(j);
            selected = Join<B>::predicate_->call(lhs_table, j, rhs_table, i).template getValue<B>();
            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);
            this->output_->cloneRow(to_update, i, 0, lhs_table, j); //    Join<B>::write_left(to_update, Operator<B>::output_, i, lhs_table, j);


        }
        this->output_->setDummyTag(i, dst_dummy_tag);
    }

    lhs_table->pinned_ = false;
    return this->output_;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;

