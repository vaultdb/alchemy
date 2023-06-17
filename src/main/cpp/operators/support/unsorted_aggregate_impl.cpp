#include "unsorted_aggregate_impl.h"
#include <util/field_utilities.h>

using namespace vaultdb;

template<typename B>
UnsortedAggregateImpl<B>::UnsortedAggregateImpl(const AggregateId &id, const FieldType &type,
                                                const int32_t &input_ordinal, const int32_t &output_ordinal)
        : agg_type_(id), field_type_(type), input_ordinal_(input_ordinal), output_ordinal_(output_ordinal) {


}

template<typename B>
UnsortedStatelessAggregateImpl<B>::UnsortedStatelessAggregateImpl(const AggregateId &id, const FieldType &type,
                                                                  const int32_t &input_ordinal,
                                                                  const int32_t &output_ordinal)
        : UnsortedAggregateImpl<B>(id, type, input_ordinal, output_ordinal) {

}


template<typename B>
void UnsortedStatelessAggregateImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match) {

    // input is NOT a dummy AND (output IS a dummy AND !matched)
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field;
    if(UnsortedAggregateImpl<B>::input_ordinal_ >= 0)
        input_field = src->getField(src_row, UnsortedAggregateImpl<B>::input_ordinal_);
    Field<B> output_field = dst->getField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_);

    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;

    if(std::is_same_v<B, bool>) {
        bool check = FieldUtilities::extract_bool(!(to_initialize & to_accumulate));
        assert(check); // can't both be true at the same time
    }

    Field<B> accumulated;
    Field<B> one = FieldFactory<B>::getOne(UnsortedAggregateImpl<B>::field_type_);

    switch(UnsortedAggregateImpl<B>::agg_type_) {
        case AggregateId::AVG:
            throw; // should use specialized UnsortedAvgImpl for this
        case AggregateId::COUNT:
            throw;
//            accumulated = Field<B>::If(to_initialize, one, output_field);
//            accumulated = Field<B>::If(to_accumulate, accumulated + one, accumulated);
            break;
        case AggregateId::MIN:
            throw;
        case AggregateId::MAX:
            throw;
        case AggregateId::SUM:
            accumulated = Field<B>::If(to_initialize, input_field, output_field);
            accumulated = Field<B>::If(to_accumulate, accumulated + input_field, accumulated);
    }
    dst->setField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_, accumulated);

}

template<typename B>
UnsortedAvgImpl<B>::UnsortedAvgImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal)
    : UnsortedAggregateImpl<B>(id, type, input_ordinal, output_ordinal){

        tuple_count_ = FieldFactory<B>::getZero(UnsortedAggregateImpl<B>::field_type_);
        running_sum_ = tuple_count_;
}

template<typename B>
void UnsortedAvgImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match){
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field = src->getField(src_row,UnsortedAggregateImpl<B>::input_ordinal_);

    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;
    B to_write = to_accumulate | to_initialize;
    Field<B> one = FieldFactory<B>::getOne(UnsortedAggregateImpl<B>::field_type_);


    // if initialize or accumulate, add 1 to count and add input_field to running sum
    tuple_count_ = Field<B>::If(to_write, tuple_count_ + one, tuple_count_);
    running_sum_ = Field<B>::If(to_write, running_sum_ + input_field, running_sum_);

    Field<B> output_field = running_sum_ / tuple_count_;

    dst->setField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_, output_field);


}

template<typename B>
UnsortedCountImpl<B>::UnsortedCountImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal)
        : UnsortedAggregateImpl<B>(id, type, input_ordinal, output_ordinal){

}

template<typename B>
void UnsortedCountImpl<B>::initialize(const QueryFieldDesc &input_schema){
    if(input_schema.bitPacked()) {
        // generate max
        assert(this->field_type_ == FieldType::SECURE_LONG || this->field_type_ == FieldType::SECURE_INT);
        Integer count = emp::Integer(input_schema.size() + 1, 0);
        Bit one(true);
        Bit *write_cursor = count.bits.data();
        for(int i = 0; i < input_schema.size(); ++i) {
            *write_cursor = one;
            ++write_cursor;
        }
        running_count_ = Field<B>(this->field_type_, count, 0);
        packed_fields_ = true;
    }
}

template<typename B>
void UnsortedCountImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match){
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field = src->getPackedField(src_row, UnsortedAggregateImpl<B>::input_ordinal_);
    Field<B> output_field = dst->getPackedField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_);

    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;

    if(std::is_same_v<B, bool>) {
        bool check = FieldUtilities::extract_bool(!(to_initialize & to_accumulate));
        assert(check); // can't both be true at the same time
    }

    Field<B> one = FieldFactory<B>::getOne(UnsortedAggregateImpl<B>::field_type_);

    std::cout << "input size : " << input_field.getSize() << " output size : " << output_field.getSize() << " running count size : " << running_count_.getSize() << endl;

    running_count_ = Field<B>::If(to_initialize, one, output_field);
    running_count_ = Field<B>::If(to_accumulate, running_count_ + one, running_count_);
    dst->setField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_, running_count_, packed_fields_);
}


template<typename B>
UnsortedMinImpl<B>::UnsortedMinImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal)
        : UnsortedAggregateImpl<B>(id, type, input_ordinal, output_ordinal){

}

template<typename B>
void UnsortedMinImpl<B>::initialize(const QueryFieldDesc &input_schema) {
    if(input_schema.bitPacked()) {
        // generate max
        assert(this->field_type_ == FieldType::SECURE_LONG || this->field_type_ == FieldType::SECURE_INT);
        Integer max = emp::Integer(input_schema.size() + 1, 0);
        Bit one(true);
        Bit *write_cursor = max.bits.data();
        for(int i = 0; i < input_schema.size(); ++i) {
            *write_cursor = one;
            ++write_cursor;
        }
        running_min_ = Field<B>(this->field_type_, max, 0);
        packed_fields_ = true;
    }
    else
        running_min_ = FieldFactory<B>::getMax(input_schema.getType());
}

template<typename B>
void UnsortedMinImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match){
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field = src->getPackedField(src_row, UnsortedAggregateImpl<B>::input_ordinal_);
    Field<B> output_field = dst->getPackedField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_);

    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;

    if(std::is_same_v<B, bool>) {
        bool check = FieldUtilities::extract_bool(!(to_initialize & to_accumulate));
        assert(check); // can't both be true at the same time
    }

    running_min_ = Field<B>::If(to_initialize, input_field, output_field);
    running_min_ = Field<B>::If(to_accumulate & (input_field < output_field), input_field, running_min_);
    dst->setField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_, running_min_, packed_fields_);
}

template<typename B>
UnsortedMaxImpl<B>::UnsortedMaxImpl(const AggregateId & id, const FieldType & type, const int32_t & input_ordinal, const int32_t & output_ordinal)
        : UnsortedAggregateImpl<B>(id, type, input_ordinal, output_ordinal){

}

template<typename B>
void UnsortedMaxImpl<B>::initialize(const QueryFieldDesc &input_schema) {
    if(input_schema.bitPacked()) {
        // generate max
        assert(this->field_type_ == FieldType::SECURE_LONG || this->field_type_ == FieldType::SECURE_INT);
        Integer min = emp::Integer(input_schema.size() + 1, input_schema.getFieldMin()); // +1 for 2's complement
        running_max_ = Field<B>(this->field_type_, min, 0);
        packed_fields_ = true;
        }
    else
        running_max_ = FieldFactory<B>::getMin(input_schema.getType());
}

template<typename B>
void UnsortedMaxImpl<B>::update(QueryTable<B> *src,  const int & src_row,  QueryTable<B> * dst, const int & dst_row, const B & match_found, const B & group_by_match){
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field = src->getPackedField(src_row, UnsortedAggregateImpl<B>::input_ordinal_);

    Field<B> output_field = dst->getPackedField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_);

    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;

    if(std::is_same_v<B, bool>) {
        bool check = FieldUtilities::extract_bool(!(to_initialize & to_accumulate));
        assert(check); // can't both be true at the same time
    }

    running_max_ = Field<B>::If(to_initialize, input_field, output_field);
    running_max_ = Field<B>::If(to_accumulate & (input_field > output_field), input_field, running_max_);

    dst->setField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_, running_max_, packed_fields_);
}



template class vaultdb::UnsortedAggregateImpl<bool>;
template class vaultdb::UnsortedAggregateImpl<emp::Bit>;
template class vaultdb::UnsortedStatelessAggregateImpl<bool>;
template class vaultdb::UnsortedStatelessAggregateImpl<emp::Bit>;
template class vaultdb::UnsortedAvgImpl<bool>;
template class vaultdb::UnsortedAvgImpl<emp::Bit>;
template class vaultdb::UnsortedCountImpl<bool>;
template class vaultdb::UnsortedCountImpl<emp::Bit>;
template class vaultdb::UnsortedMinImpl<bool>;
template class vaultdb::UnsortedMinImpl<emp::Bit>;
template class vaultdb::UnsortedMaxImpl<bool>;
template class vaultdb::UnsortedMaxImpl<emp::Bit>;
