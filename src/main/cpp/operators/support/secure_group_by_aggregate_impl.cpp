#include "secure_group_by_aggregate_impl.h"

using namespace  vaultdb;
types::Value SecureGroupByAggregateImpl::getDummyTag(const vaultdb::types::Value &isLastEntry,
                                                                       const vaultdb::types::Value &nonDummyBinFlag) {

    assert(isLastEntry.getType() == types::TypeId::ENCRYPTED_BOOLEAN &&
                   nonDummyBinFlag.getType() == types::TypeId::ENCRYPTED_BOOLEAN);

    emp::Bit output = emp::If(isLastEntry.getEmpBit(), nonDummyBinFlag.getEmpBit(), emp::Bit(true, emp::PUBLIC));
    return types::Value(output);

}

void SecureGroupByAggregateImpl::updateGroupByBinBoundary(const types::Value &isNewBin, types::Value &nonDummyBinFlag) {
    assert(isNewBin.getType() == types::TypeId::ENCRYPTED_BOOLEAN &&
           nonDummyBinFlag.getType() == types::TypeId::ENCRYPTED_BOOLEAN);

    // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
    emp::Bit updatedFlag = emp::If(isNewBin.getEmpBit(), !nonDummyBinFlag.getEmpBit(), emp::Bit(true, emp::PUBLIC));
    nonDummyBinFlag.setValue(updatedFlag);

}

void SecureGroupByCountImpl::initialize(const QueryTuple &tuple, const types::Value &isDummy) {
    runningCount = emp::If(!isDummy.getEmpBit() & !tuple.getDummyTag().getEmpBit(), one, zero);

}

void SecureGroupByCountImpl::accumulate(const QueryTuple &tuple, const types::Value &isDummy) {
    runningCount = emp::If(!isDummy.getEmpBit() & !tuple.getDummyTag().getEmpBit(), runningCount + one, runningCount);

}

types::Value SecureGroupByCountImpl::getResult() {
    return types::Value(types::TypeId::ENCRYPTED_INTEGER64, runningCount);
}

types::TypeId SecureGroupByCountImpl::getType() {
    return types::TypeId::ENCRYPTED_INTEGER64;
}
