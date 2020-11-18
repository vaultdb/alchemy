#include "group_by_aggregate_impl.h"


using namespace vaultdb;
using namespace vaultdb::types;


Value PlainGroupByAggregateImpl::getDummyTag(const Value &isLastEntry, const Value &nonDummyBin) {

    assert(isLastEntry.getType() == TypeId::BOOLEAN && nonDummyBin.getType() == TypeId::BOOLEAN);
    // if last entry, then return true nonDummyBin status, otherwise mark as dummy
    // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
    return isLastEntry.getBool() ? !nonDummyBin : Value(true);
}

void PlainGroupByAggregateImpl::updateGroupByBinBoundary(const Value &isNewBin, Value &nonDummyBinFlag) {
    assert(isNewBin.getType() == nonDummyBinFlag.getType() && nonDummyBinFlag.getType() == TypeId::BOOLEAN);

    bool updatedFlag = isNewBin.getBool() ? false : nonDummyBinFlag.getBool();
    nonDummyBinFlag.setValue(updatedFlag);

}

void GroupByCountImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == TypeId::BOOLEAN);

    if(!isDummy.getBool()) {
        runningCount = tuple.getDummyTag().getBool() ? zero : one;

    }

}



void GroupByCountImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {

    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == TypeId::BOOLEAN);


    if(!isDummy.getBool()) {
        runningCount = runningCount + (tuple.getDummyTag().getBool() ? zero : one);
    }
}

Value GroupByCountImpl::getResult() {
    return runningCount;
}




void GroupBySumImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {

    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == TypeId::BOOLEAN);

    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    // re-cast sum as INT64_T in keeping with postgres convention
    if(aggInput.getType() == TypeId::INTEGER32) {
        aggInput = Value(TypeId::INTEGER64, (int64_t) aggInput.getInt32());
    }

    if(!isDummy.getBool()) {
        runningSum = tuple.getDummyTag().getBool() ? zero : aggInput;

    }
}


void GroupBySumImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {

    if(!isDummy.getBool()) {
        Value toAdd = tuple.getDummyTag().getBool() ? zero :  tuple.getField(aggregateOrdinal).getValue();

        // re-cast sum as INT64_T in keeping with postgres convention
        if(toAdd.getType() == TypeId::INTEGER32) {
            toAdd = Value(TypeId::INTEGER64, (int64_t) toAdd.getInt32());
        }

        runningSum = runningSum + toAdd;
    }
}

Value GroupBySumImpl::getResult() {
    return runningSum;
}



void GroupByAvgImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    assert(!tuple.isEncrypted());
    assert(isDummy.getType() == TypeId::BOOLEAN);

    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    // re-cast sum as INT64_T in keeping with postgres convention
    if(aggInput.getType() == TypeId::INTEGER32) {
        aggInput = Value(TypeId::INTEGER64, (int64_t) aggInput.getInt32());
    }

    if(!isDummy.getBool()) {
        runningSum = tuple.getDummyTag().getBool() ? zero : aggInput;
        runningCount = tuple.getDummyTag().getBool() ? zero : one;
    }
}

void GroupByAvgImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {


    if(!isDummy.getBool()) {
        Value toAdd = tuple.getDummyTag().getBool() ? zero :  tuple.getField(aggregateOrdinal).getValue();
        Value toIncr = tuple.getDummyTag().getBool() ? zero : one;

        // re-cast sum as INT64_T in keeping with postgres convention
        if(toAdd.getType() == TypeId::INTEGER32) {
            toAdd = Value(TypeId::INTEGER64, (int64_t) toAdd.getInt32());
        }

        runningSum = runningSum + toAdd;
        runningCount = runningCount + toIncr;
    }

}

Value GroupByAvgImpl::getResult() {
    return runningSum/runningCount;
}


void GroupByMinImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    if(!isDummy.getBool() && !tuple.getDummyTag().getBool()) {
        initialized = true;
        runningMin = tuple.getField(aggregateOrdinal).getValue();
    }
}

void GroupByMinImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
   assert(initialized);

    if(!isDummy.getBool() && !tuple.getDummyTag().getBool()) {
        Value aggInput = tuple.getField(aggregateOrdinal).getValue();
        runningMin = (aggInput < runningMin).getBool() ? aggInput : runningMin;
    }
}

Value GroupByMinImpl::getResult() {
    assert(initialized);
    return runningMin;
}


void GroupByMaxImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    if(!isDummy.getBool() && !tuple.getDummyTag().getBool()) {
        initialized = true;
        runningMax = tuple.getField(aggregateOrdinal).getValue();
    }
}

void GroupByMaxImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    assert(initialized);

    if(!isDummy.getBool() && !tuple.getDummyTag().getBool()) {
        Value aggInput = tuple.getField(aggregateOrdinal).getValue();
        runningMax = (aggInput > runningMax).getBool() ? aggInput : runningMax;
    }

}

Value GroupByMaxImpl::getResult() {
    assert(initialized);
    return runningMax;
}