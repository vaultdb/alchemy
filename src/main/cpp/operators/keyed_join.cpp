#include "keyed_join.h"

#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>


#include <util/data_utilities.h>
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> *foreignKey, Operator<B> *primaryKey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreignKey, primaryKey, predicate, sort) {}


// fkey = 0 --> lhs, fkey = 1 --> rhs
template<typename B>
KeyedJoin<B>::KeyedJoin(Operator<B> * lhs, Operator<B> * rhs, const int & fkey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
    assert(fkey == 0 || fkey == 1);
}

template<typename B>
KeyedJoin<B>::KeyedJoin(shared_ptr<QueryTable<B> > foreignKey, shared_ptr<QueryTable<B> > primaryKey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(foreignKey, primaryKey, predicate, sort) {}

template<typename B>
KeyedJoin<B>::KeyedJoin(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs, const int & fkey, Expression<B> *predicate, const SortDefinition & sort)
        : Join<B>(lhs, rhs, predicate, sort), foreign_key_input_(fkey) {
    assert(fkey == 0 || fkey == 1);
}


template<typename B>
std::shared_ptr<QueryTable<B> > KeyedJoin<B>::runSelf() {


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
shared_ptr<QueryTable<B>> KeyedJoin<B>::foreignKeyPrimaryKeyJoin() {

    shared_ptr<QueryTable<B> > lhs_table = Operator<B>::children_[0]->getOutput(); // foreign key
    shared_ptr<QueryTable<B> > rhs_table = Operator<B>::children_[1]->getOutput(); // primary key

    uint32_t output_tuple_cnt = lhs_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = *lhs_table->getSchema();
    QuerySchema rhs_schema = *rhs_table->getSchema();
    shared_ptr<QuerySchema> output_schema = std::make_shared<QuerySchema>(Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false));
    Join<B>::output_ = shared_ptr<QueryTable<B> >(new QueryTable<B>(output_tuple_cnt, output_schema, lhs_table->getSortOrder()));
    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;

    QueryTuple<B> joined(output_schema);

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < lhs_table->getTupleCount(); ++i) {
        lhs_dummy_tag = lhs_table->getDummyTag(i);


        //QueryTuple<B> dst_tuple = (*Join<B>::output_)[i];

          Join<B>::write_left(Join<B>::output_.get(), i, lhs_table.get(), i);
          Join<B>::write_left(joined,  (*lhs_table)[i]);


          dst_dummy_tag = true; // dummy by default, no tuple comparisons yet

        for(uint32_t j = 0; j < rhs_table->getTupleCount(); ++j) {
            rhs_dummy_tag = rhs_table->getDummyTag(j);
            Join<B>::write_right(joined, (*rhs_table)[j]);
            selected = Join<B>::predicate_->call(joined).template getValue<B>();
            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);

            Join<B>::write_right(to_update, Operator<B>::output_.get(), i, rhs_table.get(), j);
            Operator<B>::output_->setDummyTag(i, dst_dummy_tag);
        }
    }

    return Join<B>::output_;

}

template<typename B>
shared_ptr<QueryTable<B>> KeyedJoin<B>::primaryKeyForeignKeyJoin() {

    std::shared_ptr<QueryTable<B> > lhs_table = Operator<B>::children_[0]->getOutput(); // primary key
    std::shared_ptr<QueryTable<B> > rhs_table = Operator<B>::children_[1]->getOutput(); // foreign key

    uint32_t output_tuple_cnt = rhs_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = *lhs_table->getSchema();
    QuerySchema rhs_schema = *rhs_table->getSchema();


    shared_ptr<QuerySchema> output_schema = std::make_shared<QuerySchema>(Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false));
    QueryTuple<B> joined(output_schema);

    SortDefinition output_sort;
    for(ColumnSort s : rhs_table->getSortOrder()) {
        ColumnSort s_prime(s.first + lhs_table->getSchema()->getFieldCount(), s.second);
        output_sort.template emplace_back(s_prime);
    }

    Join<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(output_tuple_cnt, output_schema, output_sort));
    B selected, to_update, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < rhs_table->getTupleCount(); ++i) {
        rhs_dummy_tag = rhs_table->getDummyTag(i);

        Join<B>::write_right(Operator<B>::output_.get(), i, rhs_table.get(), i);
        Join<B>::write_right(joined, (*rhs_table)[i]);

        dst_dummy_tag = true; // no comparisons yet, so it is a dummy by default


        for(uint32_t j = 0; j < lhs_table->getTupleCount(); ++j) {

            lhs_dummy_tag = lhs_table->getDummyTag(j);
            Join<B>::write_left(joined, (*lhs_table)[j]);

            selected = Join<B>::predicate_->call(joined).template getValue<B>();

            to_update = selected & (!lhs_dummy_tag) & (!rhs_dummy_tag);
            dst_dummy_tag = FieldUtilities::select(to_update, false, dst_dummy_tag);


            Join<B>::write_left(to_update, Operator<B>::output_.get(), i, lhs_table.get(), j);
            Operator<B>::output_->setDummyTag(i, dst_dummy_tag);

        }
    }



    return Join<B>::output_;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;

