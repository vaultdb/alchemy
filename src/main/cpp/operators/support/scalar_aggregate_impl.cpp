#include "scalar_aggregate_impl.h"

using namespace vaultdb;

template<typename B>
ScalarCountImpl<B>::ScalarCountImpl(const uint32_t &ordinal, const FieldType &aggType) : ScalarAggregateImpl<B>(ordinal, aggType) {
    // initialize as long for count regardless of input ordinal

    // aggregateType(aggType),
    //                                                                                       zero(FieldFactory<B>::getZero(aggregateType)),
    //                                                                                       one(FieldFactory<B>::getOne(aggregateType))
    if(TypeUtilities::isEncrypted(aggType)) {
        ScalarAggregateImpl<B>::aggregateType = FieldType::SECURE_LONG;
    }
    else {
        ScalarAggregateImpl<B>::aggregateType = FieldType::LONG;
    }

    ScalarAggregateImpl<B>::zero = FieldFactory<B>::getZero(ScalarAggregateImpl<B>::aggregateType);
    ScalarAggregateImpl<B>::one = FieldFactory<B>::getOne(ScalarAggregateImpl<B>::aggregateType);
    runningCount = ScalarAggregateImpl<B>::zero;

}

template<typename B>
void ScalarCountImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    runningCount = Field<B>::If(*tuple.getDummyTag(), runningCount, runningCount + ScalarAggregateImpl<B>::one);
}

template<typename B>
 Field<B> ScalarCountImpl<B>::getResult() const {
    return runningCount;
}



template<typename B>
void ScalarSumImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> aggInput = *tuple.getField(ScalarAggregateImpl<B>::aggregateOrdinal);
    runningSum = Field<B>::If(*tuple.getDummyTag(), runningSum, runningSum + aggInput);
}

template<typename B>
 Field<B> ScalarSumImpl<B>::getResult() const {
    return runningSum;
}


template<typename B>
ScalarMinImpl<B>::ScalarMinImpl(const uint32_t &ordinal, const FieldType &aggType):ScalarAggregateImpl<B>(ordinal, aggType) {
    runningMin = FieldFactory<B>::getMax(aggType);

}

template<typename B>
void ScalarMinImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> aggInput = *tuple.getField(ScalarAggregateImpl<B>::aggregateOrdinal);
    B sameMin = aggInput >= runningMin;
    runningMin = Field<B>::If(sameMin | *tuple.getDummyTag(), runningMin, aggInput);
}

template<typename B>
 Field<B> ScalarMinImpl<B>::getResult() const {
    return runningMin;
}


template<typename B>
ScalarMaxImpl<B>::ScalarMaxImpl(const uint32_t &ordinal, const FieldType &aggType):ScalarAggregateImpl<B>(ordinal, aggType) {
    runningMax = FieldFactory<B>::getMin(aggType);

}

template<typename B>
void ScalarMaxImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> aggInput = *tuple.getField(ScalarAggregateImpl<B>::aggregateOrdinal);
    B sameMax = aggInput <= runningMax;
    runningMax = Field<B>::If(sameMax | *tuple.getDummyTag(), runningMax, aggInput);

}

template<typename B>
 Field<B> ScalarMaxImpl<B>::getResult() const {
    return runningMax;
}


template<typename B>
ScalarAvgImpl<B>::ScalarAvgImpl(const uint32_t &ordinal, const FieldType &aggType) :  ScalarAggregateImpl<B>(ordinal, aggType) {
    runningSum = ScalarAggregateImpl<B>::zero;
    runningCount = ScalarAggregateImpl<B>::zero;

}

template<typename B>
void ScalarAvgImpl<B>::accumulate(const QueryTuple<B> &tuple) {
    Field<B> aggInput = *tuple.getField(ScalarAggregateImpl<B>::aggregateOrdinal);
    runningSum = Field<B>::If(*tuple.getDummyTag(), runningSum, runningSum + aggInput);
    runningCount = Field<B>::If(*tuple.getDummyTag(), runningCount, runningCount + ScalarAggregateImpl<B>::one);

}

template<typename B>
 Field<B> ScalarAvgImpl<B>::getResult() const {
    Field<B> sumFloat = FieldFactory<B>::toFloat(runningSum);
    Field<B> cntFloat = FieldFactory<B>::toFloat(runningCount);
    return sumFloat / cntFloat;

}

template class vaultdb::ScalarCountImpl<BoolField>;
template class vaultdb::ScalarCountImpl<SecureBoolField>;

template class vaultdb::ScalarSumImpl<BoolField>;
template class vaultdb::ScalarSumImpl<SecureBoolField>;

template class vaultdb::ScalarMinImpl<BoolField>;
template class vaultdb::ScalarMinImpl<SecureBoolField>;

template class vaultdb::ScalarMaxImpl<BoolField>;
template class vaultdb::ScalarMaxImpl<SecureBoolField>;

template class vaultdb::ScalarAvgImpl<BoolField>;
template class vaultdb::ScalarAvgImpl<SecureBoolField>;
