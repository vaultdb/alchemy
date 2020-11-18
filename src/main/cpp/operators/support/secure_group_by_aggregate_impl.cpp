#include "secure_group_by_aggregate_impl.h"

using namespace vaultdb;
using namespace vaultdb::types;

Value SecureGroupByAggregateImpl::getDummyTag(const Value &isLastEntry,
                                                                       const Value &nonDummyBinFlag) {

    assert(isLastEntry.getType() == TypeId::ENCRYPTED_BOOLEAN &&
                   nonDummyBinFlag.getType() == TypeId::ENCRYPTED_BOOLEAN);

    emp::Bit output = emp::If(isLastEntry.getEmpBit(), !nonDummyBinFlag.getEmpBit(), emp::Bit(true, emp::PUBLIC));
    return Value(output);

}

void SecureGroupByAggregateImpl::updateGroupByBinBoundary(const Value &isNewBin, Value &nonDummyBinFlag) {
    assert(isNewBin.getType() == TypeId::ENCRYPTED_BOOLEAN &&
           nonDummyBinFlag.getType() == TypeId::ENCRYPTED_BOOLEAN);

    // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
    emp::Bit updatedFlag = emp::If(isNewBin.getEmpBit(), emp::Bit(false, emp::PUBLIC), nonDummyBinFlag.getEmpBit());
    nonDummyBinFlag.setValue(updatedFlag);

}

void SecureGroupByCountImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value resetValue = Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
    runningCount = Value::obliviousIf(isDummy.getEmpBit(), runningCount, resetValue);

}

void SecureGroupByCountImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {

    Value toUpdate = !isDummy & !tuple.getDummyTag();
    Value incremented = runningCount + one;
    runningCount = Value::obliviousIf(toUpdate, incremented, runningCount);

}

Value SecureGroupByCountImpl::getResult() {
    return runningCount;
}


void SecureGroupBySumImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    // re-cast sum as INT64_T in keeping with postgres convention
    if(aggInput.getType() == TypeId::ENCRYPTED_INTEGER32) {
        emp::Integer empInt = aggInput.getEmpInt();
        empInt.resize(64, true);
        aggInput = Value(TypeId::ENCRYPTED_INTEGER64, empInt);
    }

    Value resetValue = Value::obliviousIf(tuple.getDummyTag(), zero, aggInput);
    runningSum = Value::obliviousIf(isDummy, runningSum, resetValue);
}

void SecureGroupBySumImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    Value toAdd = tuple.getField(aggregateOrdinal).getValue();

    // re-cast sum as INT64_T in keeping with postgres convention
    if(toAdd.getType() == TypeId::ENCRYPTED_INTEGER32) {
        emp::Integer empInt = toAdd.getEmpInt();
        empInt.resize(64, true);
        toAdd = Value(TypeId::ENCRYPTED_INTEGER64, empInt);
    }


    Value toUpdate = !isDummy & !(tuple.getDummyTag());
    Value incremented = runningSum + toAdd;
    runningSum = Value::obliviousIf(toUpdate, incremented, runningSum);
}

Value SecureGroupBySumImpl::getResult() {
    return runningSum;
}

void SecureGroupByAvgImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    Value resetSum = Value::obliviousIf(tuple.getDummyTag(), zero, aggInput);
    Value resetCount = Value::obliviousIf(tuple.getDummyTag(), zero, one);

    runningSum = Value::obliviousIf(isDummy, runningSum, resetSum);
    runningCount = Value::obliviousIf(isDummy, runningCount, resetCount);

}

void SecureGroupByAvgImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {

    Value toAdd = tuple.getField(aggregateOrdinal).getValue();


    Value toUpdate = !isDummy & !(tuple.getDummyTag());
    Value incremented = runningSum + toAdd;
    Value incrementedCount = runningCount + one;

    runningSum = Value::obliviousIf(toUpdate, incremented, runningSum);
    runningCount = Value::obliviousIf(toUpdate, incrementedCount, runningCount);
}

Value SecureGroupByAvgImpl::getResult() {
    return runningSum/runningCount;
}


void SecureGroupByMinImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    // if the tuple is a dummy...
    Value resetValue = Value::obliviousIf(tuple.getDummyTag(), runningMin, aggInput);

    Value localInitialized = !isDummy & !(tuple.getDummyTag());

    // use secret initialized variable for debugging
    initialized = Value::obliviousIf(isDummy, initialized, localInitialized);

    runningMin = Value::obliviousIf(isDummy, runningMin, resetValue);

}

void SecureGroupByMinImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();
    Value toUpdate = !isDummy & !(tuple.getDummyTag()) & (aggInput < runningMin);
    runningMin = Value::obliviousIf(toUpdate, aggInput, runningMin);
}

Value SecureGroupByMinImpl::getResult() {
    return runningMin;
}


void SecureGroupByMaxImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    // if the tuple is a dummy, don't update it
    Value resetValue = Value::obliviousIf(tuple.getDummyTag(), runningMax, aggInput);

    Value localInitialized = !isDummy & !(tuple.getDummyTag());

    // use secret initialized variable for debugging
    initialized = Value::obliviousIf(isDummy, initialized, localInitialized);
    runningMax = Value::obliviousIf(isDummy, runningMax, resetValue);
}

void SecureGroupByMaxImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();
    Value toUpdate = !isDummy & !(tuple.getDummyTag()) & (aggInput > runningMax);
    runningMax = Value::obliviousIf(toUpdate, aggInput, runningMax);

}

Value SecureGroupByMaxImpl::getResult() {
    //assert(initialized.getEmpBit().reveal());
    return runningMax;
}
