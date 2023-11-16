#include "filter.h"
#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include "util/field_utilities.h"
#include "query_table/query_table.h"

using namespace vaultdb;


template<typename B>
Filter<B>::Filter(Operator<B> *child, Expression<B> *predicate) :
     Operator<B>(child, child->getSortOrder()), predicate_(predicate) {
         this->output_cardinality_ = child->getOutputCardinality();
     }

template<typename B>
Filter<B>::Filter(QueryTable<B> *child, Expression<B> *predicate) :
     Operator<B>(child, child->order_by_), predicate_(predicate) {
         this->output_cardinality_ = child->tuple_cnt_;
     }

template<typename B>
QueryTable<B> *Filter<B>::runSelf() {
    QueryTable<B> *input = this->getChild()->getOutput();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    // deep copy new output, then just modify the dummy tag
    this->output_ = input->clone();
    int tuple_cnt = input->tuple_cnt_;

    for(int i = 0; i < tuple_cnt; ++i) {
        B selected = predicate_->call(this->output_, i).template getValue<B>();
        B dummy_tag =  (!selected | this->output_->getDummyTag(i)); // (!) because dummyTag is false if our selection criteria is satisfied
        this->output_->setDummyTag(i, dummy_tag);
    }

    this->output_->order_by_ = input->order_by_;
    return this->output_;

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
