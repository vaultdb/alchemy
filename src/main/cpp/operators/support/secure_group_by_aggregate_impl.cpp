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

    emp::Bit toUpdate = !(isDummy.getEmpBit()) & !(tuple.getDummyTag().getEmpBit());
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

    Value resetValue = Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, aggInput);
    runningSum = Value::obliviousIf(isDummy.getEmpBit(), runningSum, resetValue);
}

void SecureGroupBySumImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    Value toAdd = tuple.getField(aggregateOrdinal).getValue();

    // re-cast sum as INT64_T in keeping with postgres convention
    if(toAdd.getType() == TypeId::ENCRYPTED_INTEGER32) {
        emp::Integer empInt = toAdd.getEmpInt();
        empInt.resize(64, true);
        toAdd = Value(TypeId::ENCRYPTED_INTEGER64, empInt);
    }


    emp::Bit toUpdate = !(isDummy.getEmpBit()) & !(tuple.getDummyTag().getEmpBit());
    Value incremented = runningSum + toAdd;
    runningSum = Value::obliviousIf(toUpdate, incremented, runningSum);
}

Value SecureGroupBySumImpl::getResult() {
    return runningSum;
}
