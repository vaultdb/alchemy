#include "group_by_aggregate_impl.h"

using namespace vaultdb;



//template<typename B>
//GroupByCountImpl<B>::GroupByCountImpl(const int32_t &ordinal, const FieldType &aggType)

//
//template<typename B>
//void GroupByCountImpl<B>::initialize(const QueryTuple<B> &tuple, const B &group_by_match) {
//    Field<B> reset_value = Field<B>::If(tuple.getDummyTag(), GroupByAggregateImpl<B>::zero_, GroupByAggregateImpl<B>::one_);
//    running_count_ = Field<B>::If(group_by_match, running_count_, reset_value);
//
//}



//template<typename B>
//void GroupByCountImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &group_by_match) {
//
//    // if it's a group-by match and not a dummy - increment
//    // if it is a group-by match and a dummy - leave unchanged
//    // what is the relationship to the final output?  Need to sketch this out - can likely make it faster if we integrate this more tightly
//    Field<B> reset_value = Field<B>::If(tuple.getDummyTag(), GroupByAggregateImpl<B>::zero_, GroupByAggregateImpl<B>::one_);
//    running_count_ = Field<B>::If(group_by_match, running_count_, reset_value);
//
//
//    B to_update = (!group_by_match) & !(tuple.getDummyTag());
//    running_count_ = running_count_ + Field<B>::If(to_update, GroupByCountImpl<B>::one_, GroupByCountImpl<B>::zero_);
//}

//template<typename B>
//Field<B> GroupByCountImpl<B>::getResult() {
//    return running_count_;
//}

//template<typename B>
//FieldType GroupByCountImpl<B>::getType() const {
//
//}


//template<typename B>
//GroupBySumImpl<B>::GroupBySumImpl(const int32_t &ordinal, const FieldType &aggType)

//
//template<typename B>
//void GroupBySumImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//
//   const Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//   Field<B> resetValue = Field<B>::If(tuple.getDummyTag(), GroupByAggregateImpl<B>::zero_, aggInput);
//    running_sum_ = Field<B>::If(isGroupByMatch, running_sum_, resetValue);
//
//}


//template<typename B>
//void GroupBySumImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//    Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//    B toUpdate = (!isGroupByMatch) & !tuple.getDummyTag();
//    running_sum_ = running_sum_ + Field<B>::If(toUpdate, aggInput, GroupByAggregateImpl<B>::zero_);
//}

//template<typename B>
//Field<B> GroupBySumImpl<B>::getResult() {
//    // extend this to a LONG to keep with PostgreSQL convention
//   //if(running_sum_.getType() == FieldType::INT || running_sum_.getType() == FieldType::SECURE_INT)
//    //    return FieldFactory<B>::toLong(running_sum_);
//
//    return running_sum_;
//}
//
//template<typename B>
//FieldType GroupBySumImpl<B>::getType() const {
//   // if(running_sum_.getType() == FieldType::INT) return FieldType::LONG;
//   // if(running_sum_.getType() == FieldType::SECURE_INT) return FieldType::SECURE_LONG;
//
//    return GroupByAggregateImpl<B>::aggregate_type_;
//}



//
//template<typename B>
//void GroupByAvgImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//    Field<B> resetValue = Field<B>::If(tuple.getDummyTag(), GroupByAggregateImpl<B>::zero_, GroupByAggregateImpl<B>::one_);
//    running_count_ = Field<B>::If(isGroupByMatch, running_count_, resetValue);
//
//
//    Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//    resetValue = Field<B>::If(tuple.getDummyTag(), GroupByAggregateImpl<B>::zero_, aggInput);
//    running_sum_ = Field<B>::If(isGroupByMatch, running_sum_, resetValue);
//
//}

//template<typename B>
//void GroupByAvgImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//
//    Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//    B toUpdate = (!isGroupByMatch) & !(tuple.getDummyTag());
//    running_count_ = running_count_ + Field<B>::If(toUpdate, GroupByAggregateImpl<B>::one_, GroupByAggregateImpl<B>::zero_);
//    running_sum_ = running_sum_ + Field<B>::If(toUpdate, aggInput, GroupByAggregateImpl<B>::zero_);
//}

//template<typename B>
//Field<B> GroupByAvgImpl<B>::getResult() {
//    return running_sum_ / running_count_;
//}



//template<typename B>
//void GroupByMinImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//
//    Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//    Field<B> newValue = Field<B>::If(tuple.getDummyTag(), reset_min_, aggInput);
//    running_min_ = Field<B>::If(isGroupByMatch, running_min_, newValue);
//}
//
//template<typename B>
//void GroupByMinImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//    Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//    B toUpdate = (!isGroupByMatch) & !(tuple.getDummyTag()) & (aggInput < running_min_);
//    running_min_ = Field<B>::If(toUpdate, aggInput, running_min_);
//
//}

// if not initialized the value will get discarded later because the whole group-by bin will have had dummies
//template<typename B>
//Field<B> GroupByMinImpl<B>::getResult() {
//    return running_min_;
//}


//
//template<typename B>
//GroupByMaxImpl<B>::GroupByMaxImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<B>(ordinal, aggType) {
//    runningMax = FieldFactory<B>::getMin(aggType);
//    resetMax = runningMax;
//}
//
//
//template<typename B>
//void GroupByMaxImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//
//    Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//    Field<B> newValue = Field<B>::If(tuple.getDummyTag(), resetMax, aggInput);
//    runningMax = Field<B>::If(isGroupByMatch, runningMax, newValue);
//
//}
//
//template<typename B>
//void GroupByMaxImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
//    Field<B> aggInput = tuple.getField(GroupByAggregateImpl<B>::aggregate_ordinal_);
//    B toUpdate = (!isGroupByMatch) & !(tuple.getDummyTag()) & (aggInput > runningMax);
//    runningMax = Field<B>::If(toUpdate, aggInput, runningMax);
//}
//
//
//template<typename B>
//Field<B> GroupByMaxImpl<B>::getResult() {
//
//    return runningMax;
//}

template class vaultdb::GroupByAggregateImpl<bool>;
template class vaultdb::GroupByAggregateImpl<emp::Bit>;

template class vaultdb::GroupBySumImpl<bool>;
template class vaultdb::GroupBySumImpl<emp::Bit>;

template class vaultdb::GroupByMinImpl<bool>;
template class vaultdb::GroupByMinImpl<emp::Bit>;

template class vaultdb::GroupByMaxImpl<bool>;
template class vaultdb::GroupByMaxImpl<emp::Bit>;

template class vaultdb::GroupByAvgImpl<bool>;
template class vaultdb::GroupByAvgImpl<emp::Bit>;

