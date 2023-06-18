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

    bool bit_packed = dst->getSchema().getField(this->output_ordinal_).bitPacked();
    // input is NOT a dummy AND (output IS a dummy AND !matched)
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field;
    if(UnsortedAggregateImpl<B>::input_ordinal_ >= 0) {
        input_field = bit_packed ?
                      src->getPackedField(src_row, UnsortedAggregateImpl<B>::input_ordinal_) :
                      src->getField(src_row, UnsortedAggregateImpl<B>::input_ordinal_);
    }

    // TODO: refactor this to get rid of this branching
    Field<B> output_field = bit_packed ?
            dst->getPackedField(dst_row, this->output_ordinal_) :
            dst->getField(dst_row, this->output_ordinal_);



    B to_accumulate = (!input_dummy) & group_by_match;
    B to_initialize = (!input_dummy) & dst->getDummyTag(dst_row)  & !match_found;


    Field<B> accumulated;

    Field<B> one;

    switch(UnsortedAggregateImpl<B>::agg_type_) {
        case AggregateId::AVG:
            throw; // should use specialized UnsortedAvgImpl for this
        case AggregateId::COUNT:
          one =  (TypeUtilities::isEncrypted(this->field_type_))
                 ?  Field<B>(this->field_type_, Integer(this->bit_packed_size_, 1)) :   Field<B>(this->field_type_, (int64_t) 1);
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

     dst->setField(dst_row, UnsortedAggregateImpl<B>::output_ordinal_, accumulated, bit_packed);

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



template class vaultdb::UnsortedAggregateImpl<bool>;
template class vaultdb::UnsortedAggregateImpl<emp::Bit>;
template class vaultdb::UnsortedStatelessAggregateImpl<bool>;
template class vaultdb::UnsortedStatelessAggregateImpl<emp::Bit>;
template class vaultdb::UnsortedAvgImpl<bool>;
template class vaultdb::UnsortedAvgImpl<emp::Bit>;