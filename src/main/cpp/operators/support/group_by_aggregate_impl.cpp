#include "group_by_aggregate_impl.h"


using namespace vaultdb;




void GroupByCountImpl::initialize(const QueryTuple &tuple, const types::Value &isDummy) {
    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == types::TypeId::BOOLEAN);

    if(!isDummy.getBool()) {
        runningCount = tuple.getDummyTag().getBool() ? 0 : 1;

    }

}



void GroupByCountImpl::accumulate(const QueryTuple &tuple, const types::Value &isDummy) {

    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == types::TypeId::BOOLEAN);


    if(!isDummy.getBool()) {
        runningCount += tuple.getDummyTag().getBool() ? 0 : 1;
    }
}

types::Value GroupByCountImpl::getResult() {
    return types::Value(runningCount);
}

types::TypeId GroupByCountImpl::getType() {
    return types::TypeId::INTEGER64;
}

types::Value PlainGroupByAggregateImpl::getDummyTag(const types::Value &isLastEntry, const types::Value &nonDummyBin) {

    assert(isLastEntry.getType() == types::TypeId::BOOLEAN && nonDummyBin.getType() == types::TypeId::BOOLEAN);
        // if last entry, then return true nonDummyBin status, otherwise mark as dummy
        // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
        return isLastEntry.getBool() ? !nonDummyBin : types::Value(true);
}

void PlainGroupByAggregateImpl::updateGroupByBinBoundary(const types::Value &isNewBin, types::Value &nonDummyBinFlag) {
    assert(isNewBin.getType() == nonDummyBinFlag.getType() && nonDummyBinFlag.getType() == types::TypeId::BOOLEAN);

    bool updatedFlag = isNewBin.getBool() ? false : nonDummyBinFlag.getBool();
    nonDummyBinFlag.setValue(updatedFlag);

}
