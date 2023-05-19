#include "keyed_join.h"

#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>


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

    shared_ptr<QueryTable<B> > lhs_table = Operator<B>::children_[0]->getOutput(); // foreign key
    shared_ptr<QueryTable<B> > rhs_table = Operator<B>::children_[1]->getOutput(); // primary key

    QuerySchema lhs_schema = *lhs_table->getSchema();
    QuerySchema rhs_schema = *rhs_table->getSchema();
    shared_ptr<QuerySchema> output_schema = std::make_shared<QuerySchema>(Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false));
    Join<B>::output_ = shared_ptr<QueryTable<B> >(new QueryTable<B>(lhs_table->getTupleCount(), output_schema, lhs_table->getSortOrder()));
    B predicate_eval, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;

    auto dst_pos = Join<B>::output_->begin();
    QueryTuple<B> joined(output_schema);

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(auto lhs_pos = lhs_table->begin(); lhs_pos != lhs_table->end(); ++lhs_pos) {
        lhs_dummy_tag =  lhs_pos->getDummyTag();

        auto rhs_pos = rhs_table->begin();
        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        rhs_dummy_tag = rhs_pos->getDummyTag();

        Join<B>::write_left(*dst_pos,  *lhs_pos);
        Join<B>::write_right(*dst_pos, *rhs_pos);
        joined = *dst_pos;

        predicate_eval = Join<B>::predicate_.callBoolExpression(joined);

        dst_dummy_tag = (!predicate_eval) | lhs_dummy_tag | rhs_dummy_tag;

        // unconditional write to first one to initialize it
        dst_pos->setDummyTag(dst_dummy_tag);

        while(rhs_pos != rhs_table->end()) {
            rhs_dummy_tag =  rhs_pos->getDummyTag();

            Join<B>::write_right(joined, *rhs_pos);
            predicate_eval = Join<B>::predicate_.callBoolExpression(joined);
            dst_dummy_tag = (!predicate_eval) | lhs_dummy_tag | rhs_dummy_tag;

            Join<B>::write_right(!dst_dummy_tag, *dst_pos,  *rhs_pos);
            Join<B>::update_dummy_tag(*dst_pos, predicate_eval, dst_dummy_tag);
            ++rhs_pos;
        }

        ++dst_pos;

    }

    return Join<B>::output_;

}

template<typename B>
shared_ptr<QueryTable<B>> KeyedJoin<B>::primaryKeyForeignKeyJoin() {
    std::shared_ptr<QueryTable<B> > lhs_table = Operator<B>::children_[0]->getOutput(); // primary key
    std::shared_ptr<QueryTable<B> > rhs_table = Operator<B>::children_[1]->getOutput(); // foreign key

    QuerySchema lhs_schema = *lhs_table->getSchema();
    QuerySchema rhs_schema = *rhs_table->getSchema();


    shared_ptr<QuerySchema> output_schema = std::make_shared<QuerySchema>(Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false));
    QueryTuple<B> joined(output_schema);

    SortDefinition output_sort;
    for(ColumnSort s : rhs_table->getSortOrder()) {
        ColumnSort s_prime(s.first + lhs_table->getSchema()->getFieldCount(), s.second);
        output_sort.template emplace_back(s_prime);
    }

    // output size, colCount, is_encrypted
    Join<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(rhs_table->tuple_cnt_, output_schema, output_sort));
    B predicate_eval, lhs_dummy_tag, rhs_dummy_tag, dst_dummy_tag;
    auto dst_pos = Join<B>::output_->begin();

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(auto rhs_pos = rhs_table->begin(); rhs_pos != rhs_table->end(); ++rhs_pos ) {
        rhs_dummy_tag = rhs_pos->getDummyTag();

        auto lhs_pos = lhs_table->begin();
        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        lhs_dummy_tag = lhs_pos->getDummyTag();

        // unconditional write to first one to initialize it
        Join<B>::write_left(*dst_pos, *lhs_pos);
        Join<B>::write_right(*dst_pos, *rhs_pos);
        joined = *dst_pos;

        predicate_eval = Join<B>::predicate_.callBoolExpression(joined);
        dst_dummy_tag = (!predicate_eval) | lhs_dummy_tag | rhs_dummy_tag;


        dst_pos->setDummyTag(dst_dummy_tag);

        while(lhs_pos != lhs_table->end()) {
            lhs_dummy_tag =  lhs_pos->getDummyTag();
            Join<B>::write_left(joined, *lhs_pos);

            predicate_eval = Join<B>::predicate_.callBoolExpression(joined);

            dst_dummy_tag =  (!predicate_eval) | lhs_dummy_tag | rhs_dummy_tag;

            Join<B>::write_left(!dst_dummy_tag, *dst_pos, *lhs_pos);
            Join<B>::update_dummy_tag(*dst_pos, predicate_eval, dst_dummy_tag);
            ++lhs_pos;

        }
        ++dst_pos;

    }


    return Join<B>::output_;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;


