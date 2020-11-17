#include "secure_group_by_aggregate_impl.h"

using namespace  vaultdb;
types::Value SecureGroupByAggregateImpl::getDummyTag(const vaultdb::types::Value &isLastEntry,
                                                                       const vaultdb::types::Value &nonDummyBinFlag) {

    assert(isLastEntry.getType() == types::TypeId::ENCRYPTED_BOOLEAN &&
                   nonDummyBinFlag.getType() == types::TypeId::ENCRYPTED_BOOLEAN);

    emp::Bit output = emp::If(isLastEntry.getEmpBit(), !nonDummyBinFlag.getEmpBit(), emp::Bit(true, emp::PUBLIC));
    return types::Value(output);

}

void SecureGroupByAggregateImpl::updateGroupByBinBoundary(const types::Value &isNewBin, types::Value &nonDummyBinFlag) {
    assert(isNewBin.getType() == types::TypeId::ENCRYPTED_BOOLEAN &&
           nonDummyBinFlag.getType() == types::TypeId::ENCRYPTED_BOOLEAN);

    // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
    emp::Bit updatedFlag = emp::If(isNewBin.getEmpBit(), emp::Bit(false, emp::PUBLIC), nonDummyBinFlag.getEmpBit());
    nonDummyBinFlag.setValue(updatedFlag);

}

void SecureGroupByCountImpl::initialize(const QueryTuple &tuple, const types::Value &isDummy) {
    types::Value resetValue = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
    runningCount = types::Value::obliviousIf(isDummy.getEmpBit(), runningCount, resetValue);

}

void SecureGroupByCountImpl::accumulate(const QueryTuple &tuple, const types::Value &isDummy) {

    emp::Bit toUpdate = !(isDummy.getEmpBit()) & !(tuple.getDummyTag().getEmpBit());
    types::Value incremented = runningCount + one;
    runningCount = types::Value::obliviousIf(toUpdate, incremented, runningCount);

}

types::Value SecureGroupByCountImpl::getResult() {
    return runningCount;
}


void SecureGroupBySumImpl::initialize(const QueryTuple &tuple, const types::Value &isDummy) {
    types::Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    types::Value resetValue = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, aggInput);
    runningSum = types::Value::obliviousIf(isDummy.getEmpBit(), runningSum, resetValue);
}

void SecureGroupBySumImpl::accumulate(const QueryTuple &tuple, const types::Value &isDummy) {
    types::Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    emp::Bit toUpdate = !(isDummy.getEmpBit()) & !(tuple.getDummyTag().getEmpBit());
    types::Value incremented = runningSum + aggInput;
    runningSum = types::Value::obliviousIf(toUpdate, incremented, runningSum);
}

types::Value SecureGroupBySumImpl::getResult() {
    return runningSum;
}
