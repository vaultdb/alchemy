#include "scalar_aggregate_impl.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h> // leave this in to avoid templating issues

using namespace vaultdb;

template<typename B>
ScalarCountImpl<B>::ScalarCountImpl(const uint32_t &ordinal, const QueryFieldDesc & def) : ScalarAggregateImpl<B>(ordinal, def) {
    // initialize as long for count regardless of input ordinal

    ScalarAggregateImpl<B>::type_ =
            (TypeUtilities::isEncrypted(def.getType())) ? FieldType::SECURE_LONG : FieldType::LONG;

    ScalarAggregateImpl<B>::zero_ = FieldFactory<B>::getZero(ScalarAggregateImpl<B>::type_);
    ScalarAggregateImpl<B>::one_ = FieldFactory<B>::getOne(ScalarAggregateImpl<B>::type_);
    running_count_ = ScalarAggregateImpl<B>::zero_;


}

template<typename B>
void ScalarCountImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    running_count_ = Field<B>::If(tuple.getDummyTag(), running_count_, running_count_ + ScalarAggregateImpl<B>::one_);
}

template<typename B>
 Field<B> ScalarCountImpl<B>::getResult() const {
    return running_count_;
}



template<typename B>
void ScalarSumImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
    running_sum_ = Field<B>::If(tuple.getDummyTag(), running_sum_, running_sum_ + agg_input);
}

template<typename B>
 Field<B> ScalarSumImpl<B>::getResult() const {
    // extend this to a LONG to keep with PostgreSQL convention
   // if(running_sum_.getType() == FieldType::INT || running_sum_.getType() == FieldType::SECURE_INT)
    //    return FieldFactory<B>::toLong(running_sum_);

    return running_sum_;
}

template<typename B>
FieldType ScalarSumImpl<B>::getType() const {
    return ScalarAggregateImpl<B>::type_;

}


template<typename B>
ScalarMinImpl<B>::ScalarMinImpl(const uint32_t &ordinal, const QueryFieldDesc & def):ScalarAggregateImpl<B>(ordinal, def) {
    if(def.size() != TypeUtilities::getTypeSize(def.getType())) { // if we are dealing with bit packing, has to be int or long
            emp::Integer t(def.size(), 0, PUBLIC);
            emp::Bit *b = t.bits.data();
            emp::Bit tr(1, PUBLIC);
            for(int i = 0; i < def.size(); ++i) {
                   *b = tr; // set to max
            }
        running_min_ = Field<B>(def.getType(), t, 0);
    }
    else
        running_min_ = FieldFactory<B>::getMax(def.getType());

}

template<typename B>
void ScalarMinImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
    running_min_ = Field<B>::If((agg_input >= running_min_) | tuple.getDummyTag(), running_min_, agg_input);
}

template<typename B>
 Field<B> ScalarMinImpl<B>::getResult() const {
    return running_min_;
}


template<typename B>
ScalarMaxImpl<B>::ScalarMaxImpl(const uint32_t &ordinal, const QueryFieldDesc & def):ScalarAggregateImpl<B>(ordinal, def) {

    if(def.size() != TypeUtilities::getTypeSize(def.getType())) { // if we are dealing with bit packing, has to be int or long
        emp::Integer t(def.size(), 0, PUBLIC);
        running_max_ = Field<B>(def.getType(), t, 0);
    }
    else
        running_max_ = FieldFactory<B>::getMax(def.getType());
}

template<typename B>
void ScalarMaxImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
    running_max_ = Field<B>::If((agg_input <= running_max_) | tuple.getDummyTag(), running_max_, agg_input);

}

template<typename B>
 Field<B> ScalarMaxImpl<B>::getResult() const {
    return running_max_;
}


template<typename B>
ScalarAvgImpl<B>::ScalarAvgImpl(const uint32_t &ordinal, const QueryFieldDesc & def) :  ScalarAggregateImpl<B>(ordinal, def) {
    running_sum_ = ScalarAggregateImpl<B>::zero_;
    running_count_ = ScalarAggregateImpl<B>::zero_;

}

template<typename B>
void ScalarAvgImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> agg_input = tuple.getField(ScalarAggregateImpl<B>::ordinal_);
    running_sum_ = Field<B>::If(tuple.getDummyTag(), running_sum_, running_sum_ + agg_input);
    running_count_ = Field<B>::If(tuple.getDummyTag(), running_count_, running_count_ + ScalarAggregateImpl<B>::one_);

}

template<typename B>
 Field<B> ScalarAvgImpl<B>::getResult() const {
    return running_sum_ / running_count_;

}



template class vaultdb::ScalarCountImpl<bool>;
template class vaultdb::ScalarCountImpl<emp::Bit>;

template class vaultdb::ScalarSumImpl<bool>;
template class vaultdb::ScalarSumImpl<emp::Bit>;

template class vaultdb::ScalarMinImpl<bool>;
template class vaultdb::ScalarMinImpl<emp::Bit>;

template class vaultdb::ScalarMaxImpl<bool>;
template class vaultdb::ScalarMaxImpl<emp::Bit>;

template class vaultdb::ScalarAvgImpl<bool>;
template class vaultdb::ScalarAvgImpl<emp::Bit>;
