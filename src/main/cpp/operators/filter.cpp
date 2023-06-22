#include "filter.h"
#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>
#include "util/field_utilities.h"

using namespace vaultdb;


template<typename B>
Filter<B>::Filter(Operator<B> *child, Expression<B> *predicate) :
     Operator<B>(child, child->getSortOrder()), predicate_(predicate) {
     }

template<typename B>
Filter<B>::Filter(QueryTable<B> *child, Expression<B> *predicate) :
     Operator<B>(child, child->getSortOrder()), predicate_(predicate) {
     }

template<typename B>
QueryTable<B> *Filter<B>::runSelf() {
    QueryTable<B> *input = Operator<B>::getChild()->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    // deep copy new output, then just modify the dummy tag
    this-> output_ = input->clone();
    int tuple_cnt = input->getTupleCount();

    for(int i = 0; i < tuple_cnt; ++i) {
        Field<B> selected = predicate_->call(Operator<B>::output_, i);
        B dummy_tag =  ((!(selected.template getValue<B>())) | Operator<B>::output_->getDummyTag(i)); // (!) because dummyTag is false if our selection criteria is satisfied
        Operator<B>::output_->setDummyTag(i, dummy_tag);
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
    if(predicate_->exprClass() == ExpressionClass::GENERIC) {
        return ((GenericExpression<B> *) predicate_)->root_->toString();
    }
    return predicate_->toString();
}

template class vaultdb::Filter<bool>;
template class vaultdb::Filter<emp::Bit>;
