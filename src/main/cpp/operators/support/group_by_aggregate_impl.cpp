#include "group_by_aggregate_impl.h"


using namespace vaultdb;



template<typename B>
GroupByCountImpl<B>::GroupByCountImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<B>(ordinal, aggType)
{
    GroupByAggregateImpl<B>::aggregateType =
            (TypeUtilities::isEncrypted(aggType)) ? FieldType::SECURE_LONG : FieldType::LONG;

    GroupByAggregateImpl<B>::zero = FieldFactory<B>::getZero(GroupByAggregateImpl<B>::aggregateType);
    GroupByAggregateImpl<B>::one = FieldFactory<B>::getOne(GroupByAggregateImpl<B>::aggregateType);
    runningCount = GroupByAggregateImpl<B>::zero;

}



template<typename B>
void GroupByCountImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
    Field<B> resetValue = Field<B>::If(*tuple.getDummyTag(), GroupByAggregateImpl<B>::zero, GroupByAggregateImpl<B>::one);
    runningCount = Field<B>::If(isGroupByMatch, runningCount, resetValue);

}



template<typename B>
void GroupByCountImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {

    B toUpdate = (!isGroupByMatch) & !(*tuple.getDummyTag());
    runningCount = runningCount + Field<B>::If(toUpdate, GroupByCountImpl<B>::one, GroupByCountImpl<B>::zero);

}

template<typename B>
Field<B> GroupByCountImpl<B>::getResult() {
    return runningCount;
}




template<typename B>
void GroupBySumImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {

   Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
   Field<B> resetValue = Field<B>::If(*tuple.getDummyTag(), GroupByAggregateImpl<B>::zero, aggInput);
   runningSum = Field<B>::If(isGroupByMatch, runningSum, resetValue);

}


template<typename B>
void GroupBySumImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
    Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
    B toUpdate = (!isGroupByMatch) & !(*tuple.getDummyTag());
    runningSum = runningSum + Field<B>::If(toUpdate,aggInput, GroupByCountImpl<B>::zero);
}

template<typename B>
Field<B> GroupBySumImpl<B>::getResult() {
    // extend this to a LONG to keep with PostgreSQL convention
   if(runningSum.getType() == FieldType::INT || runningSum.getType() == FieldType::SECURE_INT)
        return FieldFactory<B>::toLong(runningSum);

    return runningSum;
}


template<typename B>
GroupByAvgImpl<B>::GroupByAvgImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<B>(ordinal, aggType)  {
    runningSum = GroupByAggregateImpl<B>::zero;
    runningCount = GroupByAggregateImpl<B>::zero;
}


template<typename B>
void GroupByAvgImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
    Field<B> resetValue = Field<B>::If(*tuple.getDummyTag(), GroupByAggregateImpl<B>::zero, GroupByAggregateImpl<B>::one);
    runningCount = Field<B>::If(isGroupByMatch, runningCount, resetValue);


    Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
    resetValue = Field<B>::If(*tuple.getDummyTag(), GroupByAggregateImpl<B>::zero, aggInput);
    runningSum = Field<B>::If(isGroupByMatch, runningSum, resetValue);

}

template<typename B>
void GroupByAvgImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {

    Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
    B toUpdate = (!isGroupByMatch) & !(*tuple.getDummyTag());
    runningCount = runningCount + Field<B>::If(toUpdate, GroupByCountImpl<B>::one, GroupByCountImpl<B>::zero);
    runningSum = runningSum +  Field<B>::If(toUpdate, aggInput, GroupByCountImpl<B>::zero);
}

template<typename B>
Field<B> GroupByAvgImpl<B>::getResult() {
    // this always becomes a float for computing AVG, using the precedent in psql
    Field<B> sumFloat = FieldFactory<B>::toFloat(runningSum);
    Field<B> cntFloat = FieldFactory<B>::toFloat(runningCount);
    return sumFloat / cntFloat;
}


template<typename B>
GroupByMinImpl<B>::GroupByMinImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<B>(ordinal, aggType) {
    runningMin = FieldFactory<B>::getMax(aggType);
}


template<typename B>
void GroupByMinImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {

    Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
    B sameMin = aggInput >= runningMin;
    B noWrite = isGroupByMatch | *tuple.getDummyTag() | sameMin;

    runningMin = Field<B>::If(noWrite, runningMin, aggInput);
}

template<typename B>
void GroupByMinImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {

    Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
    B sameMin = aggInput >= runningMin;
    B noWrite = isGroupByMatch | *tuple.getDummyTag() | sameMin;

    runningMin = Field<B>::If(noWrite, runningMin, aggInput);

}

// if not initialized the value will get discarded later because the whole group-by bin will have had dummies
template<typename B>
Field<B> GroupByMinImpl<B>::getResult() {
    return runningMin;
}



template<typename B>
GroupByMaxImpl<B>::GroupByMaxImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<B>(ordinal, aggType) {
    runningMax = FieldFactory<B>::getMin(aggType);
}


template<typename B>
void GroupByMaxImpl<B>::initialize(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
    Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
    B sameMax = aggInput <= runningMax;
    B noWrite = isGroupByMatch | *tuple.getDummyTag() | sameMax;

    runningMax = Field<B>::If(noWrite, runningMax, aggInput);


}

template<typename B>
void GroupByMaxImpl<B>::accumulate(const QueryTuple<B> &tuple, const B &isGroupByMatch) {
    Field<B> aggInput = *tuple.getField(GroupByAggregateImpl<B>::aggregateOrdinal);
    B sameMax = aggInput <= runningMax;
    B noWrite = isGroupByMatch | *tuple.getDummyTag() | sameMax;

    runningMax = Field<B>::If(noWrite, runningMax, aggInput);
}

template<typename B>
Field<B> GroupByMaxImpl<B>::getResult() {

    return runningMax;
}

template class vaultdb::GroupByAggregateImpl<BoolField>;
template class vaultdb::GroupByAggregateImpl<SecureBoolField>;

template class vaultdb::GroupByCountImpl<BoolField>;
template class vaultdb::GroupByCountImpl<SecureBoolField>;

template class vaultdb::GroupBySumImpl<BoolField>;
template class vaultdb::GroupBySumImpl<SecureBoolField>;

template class vaultdb::GroupByMinImpl<BoolField>;
template class vaultdb::GroupByMinImpl<SecureBoolField>;

template class vaultdb::GroupByMaxImpl<BoolField>;
template class vaultdb::GroupByMaxImpl<SecureBoolField>;

template class vaultdb::GroupByAvgImpl<BoolField>;
template class vaultdb::GroupByAvgImpl<SecureBoolField>;

