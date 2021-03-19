#include "group_by_aggregate_impl.h"



using namespace vaultdb;


/*const Field * PlainGroupByAggregateImpl::getDummyTag(const Field *isLastEntry, const Field *nonDummyBin) {

    assert(isLastEntry->getType() == FieldType::BOOL && nonDummyBin->getType() == FieldType::BOOL);
    // if last entry, then return true nonDummyBin status, otherwise mark as dummy
    // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
    if (TypeUtilities::getBool(*isLastEntry)) {
        return &(!*nonDummyBin);
    } else {
        return new BoolField(true);
    }
}

void PlainGroupByAggregateImpl::updateGroupByBinBoundary(const Field *isNewBin,  Field *nonDummyBinFlag) {
    assert(isNewBin->getType() == nonDummyBinFlag->getType() && nonDummyBinFlag->getType() == FieldType::BOOL);

    bool updatedFlag = TypeUtilities::getBool(*isNewBin) ? false : TypeUtilities::getBool(*nonDummyBinFlag);
    *nonDummyBinFlag = BoolField(updatedFlag);

}*/

template<typename T, typename B>
void GroupByCountImpl<T,B>::initialize(const QueryTuple &tuple, const B & isDummy) {

    B tupleDummyTag = static_cast<B &>(*tuple.getDummyTag());
    runningCount = T::If(isDummy | tupleDummyTag, zero, one);

}



template<typename T, typename B>
void GroupByCountImpl<T,B>::accumulate(const QueryTuple &tuple, const B & isDummy) {

    assert(isDummy->getType() == FieldType::BOOL || isDummy->getType() == FieldType::SECURE_BOOL);


    T selection = zero.select(isDummy, one);
    runningCount = runningCount + selection;

}



template<typename T, typename B>
 Field GroupByCountImpl<T,B>::getResult() const {
    return runningCount;
}

}


/*

template<typename T, typename B>
T GroupBySumImpl<T, B>::readVal(const QueryTuple & tuple) const {
    const Field *aggInput = tuple.getField(GroupByAggregateImpl<T,B>::aggregateOrdinal);
    T myInput;

    // need to invoke constructor to cover INT32 --> INT64 case
    if(aggInput->getType() == FieldType::INT) {
        int64_t input = static_cast<IntField>(*aggInput).getPayload();
        myInput = T(input);
    }
    else if(aggInput->getType() == FieldType::SECURE_INT) {
        emp::Integer input = static_cast<SecureIntField>(*aggInput).getPayload();
        input.resize(64); // make sure it is the right size
        myInput = T(input);

    }
    else {
        myInput = static_cast<T>(*aggInput);
    }

    return myInput;

}


template<typename T, typename B>
void GroupBySumImpl<T,B>::initialize(const QueryTuple &tuple, const Field *isDummy) {

    assert(isDummy->getType() == FieldType::BOOL || isDummy->getType() == FieldType::SECURE_BOOL);

    B dummyTag = static_cast<B &>(*tuple.getDummyTag());
    T myInput = readVal(tuple);
    runningSum = myInput.select(dummyTag, GroupByAggregateImpl<T,B>::zero);
}


template<typename T, typename B>
void GroupBySumImpl<T,B>::accumulate(const QueryTuple &tuple, const Field *isDummy) {

    assert(isDummy->getType() == FieldType::BOOL || isDummy->getType() == FieldType::SECURE_BOOL);

    B dummyTag = static_cast<B &>(*tuple.getDummyTag());
    T myInput = readVal(tuple);
    runningSum = runningSum + myInput.select(dummyTag, GroupByAggregateImpl<T,B>::zero);

}

template<typename T, typename B>
 Field  GroupBySumImpl<T,B>::getResult() const {
    return runningSum;
}



template<typename T, typename B>
GroupByAvgImpl<T,B>::GroupByAvgImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<T,B>(
        ordinal, aggType)  {
    // this always becomes a float for computing AVG, using the precedent in psql
    GroupByAggregateImpl<T,B>::aggregateType = FieldType::FLOAT;
    GroupByAggregateImpl<T,B>::zero = FieldFactory::getZero(GroupByAggregateImpl<T,B>::aggregateType);
    GroupByAggregateImpl<T,B>::one = FieldFactory::getOne(GroupByAggregateImpl<T,B>::aggregateType);
    runningSum = GroupByAggregateImpl<T,B>::zero;
    runningCount = GroupByAggregateImpl<T,B>::zero;
}

template<typename T, typename B>
void GroupByAvgImpl<T,B>::initialize(const QueryTuple &tuple, const Field *isDummy) {
    assert(!tuple.isEncrypted());
    assert(isDummy->getType() == FieldType::BOOL || isDummy->getType() == FieldType::SECURE_BOOL);


    // re-cast avg as float in keeping with postgres convention
    // is a FLOAT or SECURE_FLOAT
    Field *aggInput = FieldFactory::toFloat(tuple.getField(GroupByAggregateImpl<T,B>::aggregateOrdinal));
    B dummyTag = static_cast<B>(*isDummy);

    T myInput = static_cast<T>(*aggInput);
    runningSum = GroupByAggregateImpl<T,B>::zero.select(dummyTag, myInput);
    runningCount = GroupByAggregateImpl<T,B>::zero.select(dummyTag, GroupByAvgImpl<T,B>::one);
}

template<typename T, typename B>
void GroupByAvgImpl<T,B>::accumulate(const QueryTuple &tuple, const Field *isDummy) {


    Field *aggInput = FieldFactory::toFloat(tuple.getField(GroupByAggregateImpl<T,B>::aggregateOrdinal));
    B dummyTag = static_cast<B>(*isDummy);

    T myInput = static_cast<T>(*aggInput);
    runningSum = runningSum + GroupByAggregateImpl<T,B>::zero.select(dummyTag, myInput);
    runningCount = runningCount + GroupByAggregateImpl<T,B>::zero.select(dummyTag, GroupByAvgImpl<T,B>::one);

}

template<typename T, typename B>
Field GroupByAvgImpl<T,B>::getResult() const {
    return runningSum / runningCount;
}



template<typename T, typename B>
void GroupByMinImpl<T,B>::resetRunningMin()  {

    switch(GroupByAggregateImpl<T,B>::aggregateType) {
        case FieldType::INT:
            runningMin =  IntField(INT_MAX);
            break;
        case FieldType::LONG:
            runningMin =  LongField(LONG_MAX);
            break;
        case FieldType::BOOL:
            runningMin =  BoolField(true);
            break;
        case FieldType::FLOAT:
            runningMin =  FloatField(FLT_MAX);
            break;
        case FieldType::SECURE_INT:
            runningMin = SecureIntField(Integer(32, INT_MAX, PUBLIC));
        case FieldType::SECURE_LONG:
            runningMin =   SecureLongField(Integer(64, LONG_MAX, PUBLIC));
        case FieldType::SECURE_BOOL:
            runningMin =   SecureBoolField(Bit(false, PUBLIC));
        case FieldType::SECURE_FLOAT:
            runningMin =   SecureFloatField(Float(FLT_MAX, PUBLIC));

        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(GroupByAggregateImpl<T,B>::aggregateType) + " not supported by MIN()");
    }}


template<typename T, typename B>
GroupByMinImpl<T,B>::GroupByMinImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<T,B>(
        ordinal, aggType) {
    resetRunningMin();

}


template<typename T, typename B>
void GroupByMinImpl<T,B>::initialize(const QueryTuple &tuple, const Field *isDummy) {
    T aggInput = static_cast<const T>(*tuple.getField(GroupByMinImpl<T,B>::aggregateOrdinal));
    B isDummyImpl = static_cast<B>(*isDummy);
    B tupleDummyTag = static_cast<B>(*tuple.getDummyTag());
    runningMin = T::If(tupleDummyTag | isDummyImpl, runningMin, aggInput);

}

template<typename T, typename B>
void GroupByMinImpl<T,B>::accumulate(const QueryTuple &tuple, const Field *isDummy) {
    T aggInput = static_cast<T>(tuple.getField(GroupByAggregateImpl<T,B>::aggregateOrdinal));
    B isDummyImpl = static_cast<B>(*isDummy);
    B tupleDummyTag = static_cast<B>(*tuple.getDummyTag());
    B newMin = aggInput < runningMin;

    B toUpdate = (!isDummyImpl) & (!tupleDummyTag) & newMin;
    runningMin = T::If(toUpdate, aggInput, runningMin);


}

// if not initialized the value will get discarded later because the whole group-by bin will have had dummies
template<typename T, typename B>
 Field GroupByMinImpl<T,B>::getResult() const {
    return runningMin;
}



template<typename T, typename B>
GroupByMaxImpl<T,B>::GroupByMaxImpl(const int32_t &ordinal, const FieldType &aggType) : GroupByAggregateImpl<T,B>(ordinal, aggType) {
    resetRunningMax();

}


template<typename T, typename B>
void GroupByMaxImpl<T,B>::initialize(const QueryTuple &tuple, const Field *isDummy) {
    T aggInput = static_cast<const T>(*tuple.getField(GroupByMinImpl<T,B>::aggregateOrdinal));
    B isDummyImpl = static_cast<B>(*isDummy);
    B tDummyTag = static_cast<B>(*tuple.getDummyTag());
    runningMax = T::If(tDummyTag | isDummyImpl, runningMax, aggInput);

}

template<typename T, typename B>
void GroupByMaxImpl<T,B>::accumulate(const QueryTuple &tuple, const Field *isDummy) {

    T aggInput = static_cast<T>(tuple.getField(GroupByAggregateImpl<T,B>::aggregateOrdinal));
    B isDummyImpl = static_cast<B>(*isDummy);
    B tupleDummyTag = static_cast<B>(*tuple.getDummyTag());
    B newMin = aggInput > runningMax;

    B toUpdate = (!isDummyImpl) & (!tupleDummyTag) & newMin;
    runningMax = T::If(toUpdate, aggInput, runningMax);


//   if(!TypeUtilities::getBool(*isDummy) && !TypeUtilities::getBool(*tuple.getDummyTag())) {
//        const Field *aggInput = tuple.getField(aggregateOrdinal);
//        if (TypeUtilities::getBool(*aggInput > *runningMax)) {
//            *runningMax = *aggInput;
//        }
//    }

}

template<typename T, typename B>
 Field GroupByMaxImpl<T,B>::getResult() const {

    return runningMax;
}

template<typename T, typename B>
void GroupByMaxImpl<T,B>::resetRunningMax() {
    delete runningMax;
    switch(GroupByAggregateImpl<T,B>::aggregateType) {
        case FieldType::INT:
            runningMax =  IntField(INT_MIN);
            break;
        case FieldType::LONG:
            runningMax =  LongField(LONG_MIN);
            break;
        case FieldType::BOOL:
            runningMax =  BoolField(false);
            break;
        case FieldType::FLOAT:
            runningMax =  FloatField(FLT_MIN);
            break;
        case FieldType::SECURE_INT:
            runningMax = SecureIntField(Integer(32, INT_MIN, PUBLIC));
        case FieldType::SECURE_LONG:
            runningMax =   SecureLongField(Integer(64, LONG_MIN, PUBLIC));
        case FieldType::SECURE_BOOL:
            runningMax =   SecureBoolField(Bit(false, PUBLIC));
        case FieldType::SECURE_FLOAT:
            runningMax =   SecureFloatField(Float(FLT_MIN, PUBLIC));

        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(GroupByAggregateImpl<T,B>::aggregateType) + " not supported by MIN()");
    }
}

template class vaultdb::GroupByAggregator<BoolField>;
template class vaultdb::GroupByAggregator<SecureBoolField>;

/*
// forward declaration
template class vaultdb::GroupByCountImpl<LongField,BoolField>;
template class vaultdb::GroupByCountImpl<SecureLongField,SecureBoolField>;

template class vaultdb::GroupBySumImpl<IntField,BoolField>;
template class vaultdb::GroupBySumImpl<SecureIntField,SecureBoolField>;
template class vaultdb::GroupBySumImpl<LongField,BoolField>;
template class vaultdb::GroupBySumImpl<SecureLongField,SecureBoolField>;
template class vaultdb::GroupBySumImpl<FloatField,BoolField>;
template class vaultdb::GroupBySumImpl<SecureFloatField,SecureBoolField>;

template class vaultdb::GroupByAvgImpl<FloatField,BoolField>;
template class vaultdb::GroupByAvgImpl<SecureFloatField,SecureBoolField>;

template class vaultdb::GroupByMinImpl<BoolField,BoolField>;
template class vaultdb::GroupByMinImpl<SecureBoolField,SecureBoolField>;
template class vaultdb::GroupByMinImpl<IntField,BoolField>;
template class vaultdb::GroupByMinImpl<SecureIntField,SecureBoolField>;
template class vaultdb::GroupByMinImpl<LongField,BoolField>;
template class vaultdb::GroupByMinImpl<SecureLongField,SecureBoolField>;
template class vaultdb::GroupByMinImpl<FloatField,BoolField>;
template class vaultdb::GroupByMinImpl<SecureFloatField,SecureBoolField>;


template class vaultdb::GroupByMaxImpl<BoolField,BoolField>;
template class vaultdb::GroupByMaxImpl<SecureBoolField,SecureBoolField>;
template class vaultdb::GroupByMaxImpl<IntField,BoolField>;
template class vaultdb::GroupByMaxImpl<SecureIntField,SecureBoolField>;
template class vaultdb::GroupByMaxImpl<LongField,BoolField>;
template class vaultdb::GroupByMaxImpl<SecureLongField,SecureBoolField>;
template class vaultdb::GroupByMaxImpl<FloatField,BoolField>;
template class vaultdb::GroupByMaxImpl<SecureFloatField,SecureBoolField>;
*/