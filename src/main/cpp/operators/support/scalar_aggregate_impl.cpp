#include "scalar_aggregate_impl.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h> // leave this in to avoid templating issues

using namespace vaultdb;

template<typename B>
ScalarAggregateImpl<B>::ScalarAggregateImpl(const AggregateId &id, const FieldType &type,
                                            const int32_t &input_ordinal, const int32_t &output_ordinal)
        : agg_type_(id), field_type_(type), input_ordinal_(input_ordinal), output_ordinal_(output_ordinal) {

}

template<typename B>
ScalarStatelessAggregateImpl<B>::ScalarStatelessAggregateImpl(const AggregateId &id, const FieldType &type,
                                                              const int32_t &input_ordinal, const int32_t &output_ordinal)
        : ScalarAggregateImpl<B>(id, type, input_ordinal, output_ordinal) {

}


template<typename B>
void ScalarStatelessAggregateImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst) {

    // input is NOT a dummy AND (output IS a dummy AND !matched)
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field;

    if(this->agg_type_ == AggregateId::SUM || this->agg_type_ == AggregateId::AVG) {
        input_field = src->getField(src_row, this->input_ordinal_);
    }
    else {
        input_field = src->getField(src_row, this->input_ordinal_);
    }

    Field<B> output_field = dst->getField(0, this->output_ordinal_);

    Field<B> one;
    Field<B> accumulated;

    B to_accumulate = (!input_dummy) & (!not_initialized);
    B to_initialize = (!input_dummy) & not_initialized;

    switch(ScalarAggregateImpl<B>::agg_type_) {
        case AggregateId::AVG:
            throw; // should use specialized UnsortedAvgImpl for this
        case AggregateId::COUNT:
            one = (TypeUtilities::isEncrypted(this->field_type_)) ? Field<B>(this->field_type_, Integer(this->bit_packed_size_, 1)) : Field<B>(this->field_type_, (int64_t) 1);
            accumulated = Field<B>::If(to_initialize, one, output_field);
            accumulated = Field<B>::If(to_accumulate, accumulated + one, accumulated);
            break;
        case AggregateId::MIN:
            accumulated = Field<B>::If(to_initialize, input_field, output_field);
            accumulated = Field<B>::If(to_accumulate & (input_field < output_field), input_field, accumulated);
            break;
        case AggregateId::MAX:
            accumulated = Field<B>::If(to_initialize, input_field, output_field);
            accumulated = Field<B>::If(to_accumulate & (input_field > output_field), input_field, accumulated);
            break;
        case AggregateId::SUM:
            accumulated = Field<B>::If(to_initialize, input_field, output_field);
            accumulated = Field<B>::If(to_accumulate, accumulated + input_field, accumulated);

    }
    not_initialized = (to_initialize & !not_initialized) | (to_accumulate & not_initialized);
    dst->setField(0, this->output_ordinal_, accumulated);
    Field<B> output_field_checking = dst->getField(0, this->output_ordinal_);
}

template<typename B>
ScalarAvgImpl<B>::ScalarAvgImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal)
        : ScalarAggregateImpl<B>(id, type, input_ordinal, output_ordinal){

    tuple_count_ = FieldFactory<B>::getZero(ScalarAggregateImpl<B>::field_type_);
    running_sum_ = tuple_count_;
}

template<typename B>
void ScalarAvgImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst){
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field = src->getField(src_row, this->input_ordinal_);

    B to_accumulate = (!input_dummy) & (!not_initialized);
    B to_initialize = (!input_dummy) & not_initialized;

    B to_write = to_accumulate | to_initialize;
    Field<B> one = FieldFactory<B>::getOne(ScalarAggregateImpl<B>::field_type_);

    // if initialize or accumulate, add 1 to count and add input_field to running sum
    tuple_count_ = Field<B>::If(to_write, tuple_count_ + one, tuple_count_);
    running_sum_ = Field<B>::If(to_write, running_sum_ + input_field, running_sum_);

    Field<B> output_field = running_sum_ / tuple_count_;
    not_initialized = (to_initialize & !not_initialized) | (to_accumulate & not_initialized);
    dst->setField(0, ScalarAggregateImpl<B>::output_ordinal_, output_field);
}

template class vaultdb::ScalarAggregateImpl<bool>;
template class vaultdb::ScalarAggregateImpl<emp::Bit>;
template class vaultdb::ScalarStatelessAggregateImpl<bool>;
template class vaultdb::ScalarStatelessAggregateImpl<emp::Bit>;
template class vaultdb::ScalarAvgImpl<bool>;
template class vaultdb::ScalarAvgImpl<emp::Bit>;
