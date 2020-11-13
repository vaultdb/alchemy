#include "group_by_aggregate_impl.h"


using namespace vaultdb;


//the one code where we support both cleartext and mpc in-place
types::Value GroupByAggregateImpl::getDummyTag(const types::Value &isLastEntry,
                                                const types::Value &nonDummyBin) {

    if (isLastEntry.getType() == types::TypeId::BOOLEAN && nonDummyBin.getType() == types::TypeId::BOOLEAN) {
        // if last entry, then return true nonDummyBin status, otherwise mark as dummy
        return isLastEntry.getBool() ? nonDummyBin : types::Value(true);
    }

    assert(isLastEntry.getType() == types::TypeId::ENCRYPTED_BOOLEAN && nonDummyBin.getType() == types::TypeId::ENCRYPTED_BOOLEAN);

    emp::Bit output = emp::If(isLastEntry.getEmpBit(), nonDummyBin.getEmpBit(), emp::Bit(true, emp::PUBLIC));
    return types::Value(output);
}


void GroupByCountImpl::initialize(const QueryTuple &tuple, const types::Value &isDummy) {
    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == types::TypeId::BOOLEAN);

    if(!isDummy.getBool()) {
        runningCount = tuple.getDummyTag().getBool() ? 0 : 1;
        initialized = true;
    }

}

void GroupByCountImpl::accumulate(const QueryTuple &tuple, const types::Value &isDummy) {

    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == types::TypeId::BOOLEAN);
    assert(initialized.which() == 0 && boost::get<bool>(initialized)); // init'd to bool, bool is true


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

