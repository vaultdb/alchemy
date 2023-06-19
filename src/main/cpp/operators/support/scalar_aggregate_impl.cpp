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

    bool bit_packed = dst->getSchema().getField(this->output_ordinal_).bitPacked();
    // input is NOT a dummy AND (output IS a dummy AND !matched)
    B input_dummy = src->getDummyTag(src_row);
    Field<B> input_field;
    //if(ScalarAggregateImpl<B>::input_ordinal_ >= 0) {
        input_field = bit_packed ?
                      src->getPackedField(src_row, this->input_ordinal_) :
                      src->getField(src_row, this->input_ordinal_);
    //}

    Field<B> output_field = bit_packed ?
                            dst->getPackedField(0, this->output_ordinal_) :
                            dst->getField(0, this->output_ordinal_);

    Field<B> one;
    //Field<B> zero = (std::is_same_v<B, Bit>) ? Field<B>(this->field_type_, Integer(this->bit_packed_size_, 0)) : Field<B>(this->field_type_, 0L);
    Field<B> accumulated;

    B to_accumulate = (!input_dummy) & (!not_initialized);
    B to_initialize = (!input_dummy) & not_initialized;

    switch(ScalarAggregateImpl<B>::agg_type_) {
        case AggregateId::AVG:
            throw; // should use specialized UnsortedAvgImpl for this
        case AggregateId::COUNT:
            one = (std::is_same_v<B, Bit>) ? Field<B>(this->field_type_, Integer(this->bit_packed_size_, 1)) : Field<B>(this->field_type_, 1L);
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
    dst->setField(0, this->output_ordinal_, accumulated, bit_packed);
    Field<B> output_field_checking = dst->getPackedField(0, this->output_ordinal_);

    std::cout << "input : " << input_field.reveal() << " output : " << output_field_checking.reveal() << endl;

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

//template<typename B>
//ScalarCountImpl<B>::ScalarCountImpl(const uint32_t &ordinal, const QueryFieldDesc & def) : ScalarAggregateImpl<B>(ordinal, def) {
//    // initialize as long for count regardless of input ordinal
//
//    ScalarAggregateImpl<B>::type_ =
//            (TypeUtilities::isEncrypted(def.getType())) ? FieldType::SECURE_LONG : FieldType::LONG;
//
//    ScalarAggregateImpl<B>::zero_ = FieldFactory<B>::getZero(ScalarAggregateImpl<B>::type_);
//    ScalarAggregateImpl<B>::one_ = FieldFactory<B>::getOne(ScalarAggregateImpl<B>::type_);
//    running_count_ = ScalarAggregateImpl<B>::zero_;
//
//
//}
//
//template<typename B>
//void ScalarCountImpl<B>::accumulate(const QueryTuple<B> &tuple) {
//    running_count_ = Field<B>::If(tuple.getDummyTag(), running_count_, running_count_ + ScalarAggregateImpl<B>::one_);
//}
//
//template<typename B>
//void ScalarCountImpl<B>::accumulate(const QueryTable<B> *table, const int &row) {
//    running_count_ = Field<B>::If(table->getDummyTag(row), running_count_, running_count_ + ScalarAggregateImpl<B>::one_);
//
//}
//
//template<typename B>
// Field<B> ScalarCountImpl<B>::getResult() const {
//    return running_count_;
//}
//
//
//
//template<typename B>
//void ScalarSumImpl<B>::accumulate(const QueryTuple<B> &tuple) {
//    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
//    running_sum_ = Field<B>::If(tuple.getDummyTag(), running_sum_, running_sum_ + agg_input);
//
//}
//
//template<typename B>
//void ScalarSumImpl<B>::accumulate(const QueryTable<B> *table, const int &row) {
//    Field<B> agg_input = table->getField(row,ScalarAggregateImpl<B>::ordinal_);
//    running_sum_ = Field<B>::If(table->getDummyTag(row), running_sum_, running_sum_ + agg_input);
//
//}
//
//template<typename B>
// Field<B> ScalarSumImpl<B>::getResult() const {
//    // extend this to a LONG to keep with PostgreSQL convention
//   // if(running_sum_.getType() == FieldType::INT || running_sum_.getType() == FieldType::SECURE_INT)
//    //    return FieldFactory<B>::toLong(running_sum_);
//
//    return running_sum_;
//}
//
//template<typename B>
//FieldType ScalarSumImpl<B>::getType() const {
//    return ScalarAggregateImpl<B>::type_;
//
//}
//
//
//template<typename B>
//ScalarMinImpl<B>::ScalarMinImpl(const uint32_t &ordinal, const QueryFieldDesc & def):ScalarAggregateImpl<B>(ordinal, def) {
////    if(def.size() != TypeUtilities::getTypeSize(def.getType())) { // if we are dealing with bit packing, has to be int or long
////            emp::Integer t(def.size(), 0, PUBLIC);
////            emp::Bit *b = t.bits.data();
////            emp::Bit tr(1, PUBLIC);
////            for(int i = 0; i < def.size(); ++i) {
////                   *b = tr; // set to max
////                   ++b;
////            }
////        running_min_ = Field<B>(def.getType(), t, 0);
////    }
////    else
//        running_min_ = FieldFactory<B>::getMax(def.getType());
//
//}
//
//template<typename B>
//void ScalarMinImpl<B>::accumulate(const QueryTuple<B> &tuple) {
//    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
//    running_min_ = Field<B>::If((agg_input >= running_min_) | tuple.getDummyTag(), running_min_, agg_input);
//
//}
//
//template<typename B>
//void ScalarMinImpl<B>::accumulate(const QueryTable<B> *table, const int &row) {
//    Field<B> agg_input = table->getField(row, ScalarAggregateImpl<B>::ordinal_);
//    running_min_ = Field<B>::If((agg_input >= running_min_) | table->getDummyTag(row), running_min_, agg_input);
//}
//
//template<typename B>
// Field<B> ScalarMinImpl<B>::getResult() const {
//    return running_min_;
//}
//
//
//template<typename B>
//ScalarMaxImpl<B>::ScalarMaxImpl(const uint32_t &ordinal, const QueryFieldDesc & def):ScalarAggregateImpl<B>(ordinal, def) {
//
////    if(def.size() != TypeUtilities::getTypeSize(def.getType())) { // if we are dealing with bit packing, has to be int or long
////        emp::Integer t(def.size(), 0, PUBLIC);
////        running_max_ = Field<B>(def.getType(), t, 0);
////    }
////    else
//        running_max_ = FieldFactory<B>::getMin(def.getType());
//}
//
//template<typename B>
//void ScalarMaxImpl<B>::accumulate(const QueryTuple<B> &tuple) {
//    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
//    running_max_ = Field<B>::If((agg_input <= running_max_) | tuple.getDummyTag(), running_max_, agg_input);
//
//}
//
//template<typename B>
//void ScalarMaxImpl<B>::accumulate(const QueryTable<B> *table, const int &row) {
//    Field<B> agg_input = table->getField(row, ScalarAggregateImpl<B>::ordinal_);
//    running_max_ = Field<B>::If((agg_input <= running_max_) | table->getDummyTag(row), running_max_, agg_input);
//}
//
//
//template<typename B>
// Field<B> ScalarMaxImpl<B>::getResult() const {
//    return running_max_;
//}
//
//
//template<typename B>
//ScalarAvgImpl<B>::ScalarAvgImpl(const uint32_t &ordinal, const QueryFieldDesc & def) :  ScalarAggregateImpl<B>(ordinal, def) {
//    ScalarAggregateImpl<B>::column_def_.setSize(TypeUtilities::getTypeSize(ScalarAggregateImpl<B>::type_)); // reset bit packing for this
//    running_sum_ = ScalarAggregateImpl<B>::zero_;
//    running_count_ = ScalarAggregateImpl<B>::zero_;
//
//}
//
//template<typename B>
//void ScalarAvgImpl<B>::accumulate(const QueryTuple<B> &tuple) {
//    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
//    running_sum_ = Field<B>::If(tuple.getDummyTag(), running_sum_, running_sum_ + agg_input);
//    running_count_ = Field<B>::If(tuple.getDummyTag(), running_count_, running_count_ + ScalarAggregateImpl<B>::one_);
//
//}
//
//template<typename B>
//void ScalarAvgImpl<B>::accumulate(const QueryTable<B> *table, const int &row) {
//
//    Field<B> agg_input = table->getField(row,ScalarAggregateImpl<B>::ordinal_);
//    B dummy_tag = table->getDummyTag(row);
//    running_sum_ = Field<B>::If(dummy_tag, running_sum_, running_sum_ + agg_input);
//    running_count_ = Field<B>::If(dummy_tag, running_count_, running_count_ + ScalarAggregateImpl<B>::one_);
//
//}
//
//template<typename B>
// Field<B> ScalarAvgImpl<B>::getResult() const {
//    return running_sum_ / running_count_;
//
//}
//
//
//
//template class vaultdb::ScalarCountImpl<bool>;
//template class vaultdb::ScalarCountImpl<emp::Bit>;
//
//template class vaultdb::ScalarSumImpl<bool>;
//template class vaultdb::ScalarSumImpl<emp::Bit>;
//
//template class vaultdb::ScalarMinImpl<bool>;
//template class vaultdb::ScalarMinImpl<emp::Bit>;
//
//template class vaultdb::ScalarMaxImpl<bool>;
//template class vaultdb::ScalarMaxImpl<emp::Bit>;
//
//template class vaultdb::ScalarAvgImpl<bool>;
//template class vaultdb::ScalarAvgImpl<emp::Bit>;
