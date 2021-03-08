#include "group_by_aggregate_impl.h"



using namespace vaultdb;


const Field * PlainGroupByAggregateImpl::getDummyTag(const Field *isLastEntry, const Field *nonDummyBin) {

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

}

void GroupByCountImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {
    assert(!tuple.isEncrypted());
    assert(isDummy->getType() == FieldType::BOOL);


    if(!TypeUtilities::getBool(*isDummy)) {
        *runningCount = TypeUtilities::getBool(*tuple.getDummyTag()) ? *zero : *one;

    }

}



void GroupByCountImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {

    assert(!tuple.isEncrypted());
    assert(isDummy->getType() == FieldType::BOOL);


    if(!TypeUtilities::getBool(*isDummy)) {
        *runningCount = *runningCount + *(TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : one);
    }
}

const Field * GroupByCountImpl::getResult() const {
    return runningCount;
}




void GroupBySumImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {

    assert(!tuple.isEncrypted());
    assert(isDummy->getType() == FieldType::BOOL);

    Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));

    // re-cast sum as INT64_T in keeping with postgres convention
    if(aggInput->getType() == FieldType::INT32) {
        *aggInput =  LongField( (int64_t) TypeUtilities::getInt(*aggInput));
    }

    if(!TypeUtilities::getBool(*isDummy)) {
        runningSum = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : aggInput;

    }
}


void GroupBySumImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {

    if(!TypeUtilities::getBool(*isDummy)) {
        Field *toAdd = FieldFactory::copyField(TypeUtilities::getBool(*tuple.getDummyTag()) ? zero :  tuple.getField(aggregateOrdinal));

        // re-cast sum as INT64_T in keeping with postgres convention
        if(toAdd->getType() == FieldType::INT32) {
            *toAdd =  LongField( (int64_t) TypeUtilities::getInt(*toAdd));
        }

        *runningSum = *runningSum + *toAdd;
    }
}

const Field * GroupBySumImpl::getResult() const {
    return runningSum;
}


GroupByAvgImpl::GroupByAvgImpl(const int32_t &ordinal, const FieldType &aggType) : PlainGroupByAggregateImpl(ordinal, aggType)  {
    // this always becomes a float for computing AVG, using the precedent in psql
    aggregateType = FieldType::FLOAT32;
    zero = FieldFactory::getZero(aggregateType);
    one = FieldFactory::getOne(aggregateType);
    runningSum = zero;
    runningCount = zero;
}


void GroupByAvgImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {
    assert(!tuple.isEncrypted());
    assert(isDummy->getType() == FieldType::BOOL);

    Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));

    // re-cast avg as float in keeping with postgres convention
    aggInput = FieldFactory::toFloat(aggInput);

    if(!TypeUtilities::getBool(*isDummy)) {
        runningSum = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : aggInput;
        runningCount = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : one;
    }
}

void GroupByAvgImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {


    if(!TypeUtilities::getBool(*isDummy)) {
        Field *aggInput = FieldFactory::toFloat(tuple.getField(aggregateOrdinal));
        Field *toAdd = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero :  aggInput;
        Field *toIncr = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : one;

        // re-cast sum as INT64_T in keeping with postgres convention
        if(toAdd->getType() == FieldType::INT32) {
            *toAdd =  LongField( (int64_t) TypeUtilities::getInt(*toAdd));
        }

        *runningSum = *runningSum + *toAdd;
        *runningCount = *runningCount + *toIncr;
    }

}

const Field * GroupByAvgImpl::getResult() const {
    return &(*runningSum/ *runningCount);
}

void GroupByMinImpl::resetRunningMin()  {
    delete runningMin;

    switch(aggregateType) {
        case FieldType::INT32:
            runningMin = new IntField(INT_MAX);
            break;
        case FieldType::INT64:
            runningMin = new LongField(LONG_MAX);
            break;
        case FieldType::BOOL:
            runningMin = new BoolField(true);
            break;
        case FieldType::FLOAT32:
            runningMin = new FloatField(FLT_MAX);
            break;
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(aggregateType) + " not supported by MIN()");
    }}


GroupByMinImpl::GroupByMinImpl(const int32_t &ordinal, const FieldType &aggType) : PlainGroupByAggregateImpl(ordinal, aggType) {
    resetRunningMin();

}


void GroupByMinImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {

    if(!TypeUtilities::getBool(*isDummy)) {
        resetRunningMin();
        if(!TypeUtilities::getBool(*tuple.getDummyTag())) {
            *runningMin = *tuple.getField(aggregateOrdinal);
        }
    }
}

void GroupByMinImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {

    if(!TypeUtilities::getBool(*isDummy) && !TypeUtilities::getBool(*tuple.getDummyTag())) {
        const Field * tupleVal = tuple.getField(aggregateOrdinal);
        bool newMin = TypeUtilities::getBool(*tupleVal < *runningMin);

        *runningMin = newMin ? *tupleVal : *runningMin;
    }
}

// if not initialized the value will get discarded later because the whole group-by bin will have had dummies
const Field * GroupByMinImpl::getResult() const {
    return runningMin;
}



GroupByMaxImpl::GroupByMaxImpl(const int32_t &ordinal, const FieldType &aggType) : PlainGroupByAggregateImpl(ordinal, aggType) {
    resetRunningMax();
}


void GroupByMaxImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {
    if(!TypeUtilities::getBool(*isDummy)) {
        resetRunningMax();
        if(!TypeUtilities::getBool(*tuple.getDummyTag())) {
            runningMax = FieldFactory::copyField(tuple.getField(aggregateOrdinal));
        }
    }
}

void GroupByMaxImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {


    if(!TypeUtilities::getBool(*isDummy) && !TypeUtilities::getBool(*tuple.getDummyTag())) {
        const Field *aggInput = tuple.getField(aggregateOrdinal);
        if (TypeUtilities::getBool(*aggInput > *runningMax)) {
            *runningMax = *aggInput;
        }
    }

}

const Field * GroupByMaxImpl::getResult() const {

    return runningMax;
}

void GroupByMaxImpl::resetRunningMax() {
    delete runningMax;
    switch(aggregateType) {
        case FieldType::INT32:
            runningMax = new IntField(INT_MIN);
            break;
        case FieldType::INT64:
            runningMax = new LongField(LONG_MIN);
            break;
        case FieldType::BOOL:
            runningMax = new BoolField(false);
            break;
        case FieldType::FLOAT32:
            runningMax = new FloatField(FLT_MIN);
            break;
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(aggregateType) + " not supported by MIN()");
    }
}

