#include "filter.h"
#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>
#include "util/field_utilities.h"

using namespace vaultdb;


template<typename B>
Filter<B>::Filter(Operator<B> *child, BoolExpression<B> & predicate) :
     Operator<B>(child, child->getSortOrder()), predicate_(predicate) {
     }

template<typename B>
Filter<B>::Filter(shared_ptr<QueryTable<B> > child, BoolExpression<B> & predicate) :
     Operator<B>(child, child->getSortOrder()), predicate_(predicate) {
     }

template<typename B>
std::shared_ptr<QueryTable<B> > Filter<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();

    // deep copy new output, then just modify the dummy tag
    Operator<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));
    QueryTableIterator<B> pos = Operator<B>::output_->begin();
    QueryTableIterator<B> stop = Operator<B>::output_->end();


    while(pos != stop){
        QueryTuple<B> &tuple = *pos;

        B dummy_tag = tuple.getDummyTag();
        B predicate_out = predicate_.callBoolExpression(tuple);

        dummy_tag =  ((!predicate_out) | dummy_tag); // (!) because dummyTag is false if our selection criteria is satisfied
        tuple.setDummyTag(dummy_tag);
        ++pos;
    }


    Operator<B>::output_->setSortOrder(input->getSortOrder());
    return Operator<B>::output_;

}

template<typename B>
string Filter<B>::getOperatorType() const {
    return "Filter";
}

template<typename B>
string Filter<B>::getParameters() const {

    return predicate_.root_->toString();
}

template class vaultdb::Filter<bool>;
template class vaultdb::Filter<emp::Bit>;
