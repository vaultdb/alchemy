#include "keyed_join.h"
#include "secure_tuple.h"
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
    std::shared_ptr<QueryTable<B> > lhs_table = Join<B>::children_[0]->getOutput(); // foreign key
    std::shared_ptr<QueryTable<B> > rhs_table = Join<B>::children_[1]->getOutput(); // primary key
    QueryTuple<B> lhs_tuple(*lhs_table->getSchema()), rhs_tuple(*rhs_table->getSchema());

    uint32_t output_tuple_cnt = lhs_table->getTupleCount(); // foreignKeyTable = foreign key
    QuerySchema lhs_schema = *lhs_table->getSchema();
    QuerySchema rhs_schema = *rhs_table->getSchema();
    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);

    std::cout << "Lhs input has " << lhs_table->getTupleCount() << " tuples, " << lhs_table->getTrueTupleCount()  << " of which are real." << std::endl;
    std::cout << "Rhs input has " << rhs_table->getTupleCount() << " tuples, " << rhs_table->getTrueTupleCount()  << " of which are real." << std::endl;

    Join<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(output_tuple_cnt, output_schema, lhs_table->getSortOrder()));

    // each foreignKeyTable tuple can have at most one match from primaryKeyTable relation
    for(uint32_t i = 0; i < lhs_table->getTupleCount(); ++i) {
        lhs_tuple = (*lhs_table)[i];

        QueryTuple<B> dst_tuple = Join<B>::output_->getTuple(i);

        // for first tuple comparison, initialize output tuple -- just in case there are no matches
        rhs_tuple = (*rhs_table)[0];

        B predicateEval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);
        B dst_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicateEval);

        // unconditional write to first one to initialize it
        Join<B>::write_left(true, dst_tuple, lhs_tuple);
        Join<B>::write_right(true, dst_tuple, rhs_tuple);
        dst_tuple.setDummyTag(dst_dummy_tag);

        for(uint32_t j = 1; j < rhs_table->getTupleCount(); ++j) {
            rhs_tuple = (*rhs_table)[j];
            predicateEval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);
            B this_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicateEval);

            Join<B>::write_right(!this_dummy_tag, dst_tuple, rhs_tuple);
            if(!FieldUtilities::extract_bool(this_dummy_tag)) {
                std::cout << "Match found on lhs idx=" << i << ", rhs idx=" << j << ": " << lhs_tuple.toString(true) << ", " << rhs_tuple.toString(true) <<  std::endl;
            }

        }



    }
    //***** temp debug *****/
    std::cout << "True tuple count post-join: " << Join<B>::output_->getTrueTupleCount() << " observed output card: " <<  Join<B>::output_->getTupleCount() <<   std::endl;

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
    QuerySchema output_schema = Join<B>::concatenateSchemas(lhs_schema, rhs_schema, false);

    std::cout << "Lhs input has " << lhs_table->getTupleCount() << " tuples, " << lhs_table->getTrueTupleCount()  << " of which are real." << std::endl;
    std::cout << "Rhs input has " << rhs_table->getTupleCount() << " tuples, " << rhs_table->getTrueTupleCount()  << " of which are real." << std::endl;

    // do fkey --> primary key loops as usual
    // *** does not preserve expected output order of (lhs_sort_key) ***
    // *** offers rhs_sort_key instead
    // output size, colCount, is_encrypted
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
        Join<B>::write_left(true, dst_tuple, lhs_tuple);
        Join<B>::write_right(true, dst_tuple, rhs_tuple);
        dst_tuple.setDummyTag(dst_dummy_tag);

        if(i == 168)
            std::cout << "*** Found suspect at idx " << i << " with value " <<rhs_tuple.toString(true)
            << " prelim dst tuple: " << dst_tuple.toString(true) <<  std::endl;


        for(uint32_t j = 1; j < lhs_table->getTupleCount(); ++j) {
            lhs_tuple = (*lhs_table)[j];


            predicate_eval = Join<B>::predicate_.call(lhs_tuple, rhs_tuple, output_schema);
            B this_dummy_tag = Join<B>::get_dummy_tag(lhs_tuple, rhs_tuple, predicate_eval);



            Join<B>::write_left(!this_dummy_tag, dst_tuple, lhs_tuple);
            Field<B> write_dummy_tag_field = Field<B>::If(!this_dummy_tag, this_dummy_tag, dst_tuple.getDummyTag());
            B write_dummy_tag = FieldUtilities::getBoolPrimitive(write_dummy_tag_field);
            dst_tuple.setDummyTag(write_dummy_tag);

            bool tmp_plain_dummy_tag = FieldUtilities::extract_bool(this_dummy_tag);


            if((j == 168 && !FieldUtilities::extract_bool(rhs_tuple.getDummyTag())) || !FieldUtilities::extract_bool(this_dummy_tag)){
                //           if(!FieldUtilities::extract_bool(lhs_tuple.getDummyTag()) && !FieldUtilities::extract_bool(rhs_tuple.getDummyTag()) ) {
                if(lhs_schema.getFieldCount() >= 3 && rhs_schema.getFieldCount() >= 1)
                    std::cout << "Suspect: " << lhs_tuple.getField(2).toString() << " vs " << rhs_tuple.getField(0).toString()
                              << " eq? " << (lhs_tuple.getField(2) == rhs_tuple.getField(0)) <<  std::endl;

                std::cout << "Comparing on lhs idx=" << i << ", rhs idx=" << j << ": " << lhs_tuple.toString(true) << ", " << rhs_tuple.toString(true)
                          << " predicate eval=" << FieldUtilities::extract_bool(predicate_eval) << ", " << " dt=" <<  FieldUtilities::extract_bool(this_dummy_tag) <<  std::endl;
                std::cout << "   dst tuple[" << i << "] : " << dst_tuple.toString(true) << std::endl;
            }

            QueryTuple<B> tmp = Join<B>::output_->getTuple(168);
            Field<B> tmp_field = tmp.getField(2);
            Field<B> t(FieldType::INT, 4141668);
            B cmp = (t != tmp_field);
            if(lhs_schema.getFieldCount() < 3 && i > 168 && FieldUtilities::extract_bool(cmp))
                std::cout << "Writing to suspect at rhs idx " << j << std::endl;

        }

    // it's already wrong after the 168 round.  Why?
        if(lhs_schema.getFieldCount() < 3 && i == 168)
            std::cout << "***Output tuple: " << dst_tuple.toString(true) << std::endl;



    }

    //***** temp debug *****/
    std::cout << "True tuple count post-join: " << Join<B>::output_->getTrueTupleCount() << " observed output card: " <<  Join<B>::output_->getTupleCount() <<   std::endl;
    std::cout << "Suspect value post-join: " <<  Join<B>::output_->getTuple(168).toString(true) << std::endl;

    return Join<B>::output_;

}


template class vaultdb::KeyedJoin<bool>;
template class vaultdb::KeyedJoin<emp::Bit>;


