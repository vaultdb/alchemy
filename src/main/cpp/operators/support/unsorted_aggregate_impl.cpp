#include "operators/support/unsorted_aggregate_impl.h"
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
UnsortedAggregateImpl<B>::UnsortedAggregateImpl(const AggregateId &id, const FieldType &type,
                                                const int32_t &input_ordinal, const int32_t &output_ordinal, const int & max_value)
        : agg_type_(id), field_type_(type), input_ordinal_(input_ordinal), output_ordinal_(output_ordinal) {

    if(max_value > 0 && std::is_same_v<B, Bit>) {
        bit_packed_size_ =  ceil(log2(static_cast<float>(max_value))) + 1; // for sign bit
        if(this->field_type_ == FieldType::SECURE_INT || this->field_type_ == FieldType::SECURE_LONG)
            one_ = Field<B>(this->field_type_, Integer(bit_packed_size_, 1));
        else
            one_ = FieldFactory<B>::getOne(this->field_type_);
    } else  {
        bit_packed_size_ = TypeUtilities::getTypeSize(this->field_type_);
        one_ = FieldFactory<B>::getOne(this->field_type_);
    }

}

template<typename B>
UnsortedStatelessAggregateImpl<B>::UnsortedStatelessAggregateImpl(const AggregateId &id, const FieldType &type,
                                                                  const int32_t &input_ordinal,
                                                                  const int32_t &output_ordinal, const int & max_value)
        : UnsortedAggregateImpl<B>(id, type, input_ordinal, output_ordinal, max_value) {

}


template<typename B>
void UnsortedStatelessAggregateImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) {

    // input is NOT a dummy AND (output IS a dummy AND !matched)
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field, output_field;
    output_field = dst->getField(dst_row, this->output_ordinal_);

    if(this->agg_type_ == AggregateId::SUM) {
        input_field =  src->getField(src_row, this->input_ordinal_);
        input_field.unpack(src->getSchema().getField(this->input_ordinal_));
        output_field = dst->getField(dst_row, this->output_ordinal_);
    } else if(this->agg_type_ != AggregateId::COUNT) { // no input field for cnt
        input_field =  src->getField(src_row, this->input_ordinal_);
    }

    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;


    Field<B> accumulated;

    switch(this->agg_type_) {
        case AggregateId::AVG:
            throw; // should use specialized UnsortedAvgImpl for this
        case AggregateId::COUNT:
            accumulated = Field<B>::If(to_initialize, this->one_, output_field);
            accumulated = Field<B>::If(to_accumulate, accumulated + this->one_, accumulated);
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

     dst->setField(dst_row, this->output_ordinal_, accumulated);
}

template<typename B>
UnsortedAvgImpl<B>::UnsortedAvgImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal)
        : UnsortedAggregateImpl<B>(id, type, input_ordinal, output_ordinal){

    tuple_count_ = FieldFactory<B>::getZero(this->field_type_);
    running_sum_ = tuple_count_;
}

template<typename B>
void UnsortedAvgImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match){
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field = src->getField(src_row, this->input_ordinal_);
    input_field.unpack(src->getSchema().getField(this->input_ordinal_));

    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;
    B to_write = to_accumulate | to_initialize;

    // if initialize or accumulate, add 1 to count and add input_field to running sum
    tuple_count_ = Field<B>::If(to_write, tuple_count_ + this->one_, tuple_count_);
    running_sum_ = Field<B>::If(to_write, running_sum_ + input_field, running_sum_);

    Field<B> output_field = running_sum_ / tuple_count_;
    dst->setField(dst_row, this->output_ordinal_, output_field);

}



template class vaultdb::UnsortedAggregateImpl<bool>;
template class vaultdb::UnsortedAggregateImpl<emp::Bit>;
template class vaultdb::UnsortedStatelessAggregateImpl<bool>;
template class vaultdb::UnsortedStatelessAggregateImpl<emp::Bit>;
template class vaultdb::UnsortedAvgImpl<bool>;
template class vaultdb::UnsortedAvgImpl<emp::Bit>;