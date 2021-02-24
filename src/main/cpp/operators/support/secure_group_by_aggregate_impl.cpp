#include <util/emp_manager.h>
#include "secure_group_by_aggregate_impl.h"

using namespace vaultdb;
using namespace vaultdb::types;
using namespace emp;

Value SecureGroupByAggregateImpl::getDummyTag(const Value &isLastEntry,
                                                                       const Value &nonDummyBinFlag) {

    assert(isLastEntry.getType() == TypeId::ENCRYPTED_BOOLEAN &&
                   nonDummyBinFlag.getType() == TypeId::ENCRYPTED_BOOLEAN);

    // if it is the last bin, then we output if it's a non-dummy bin
    // else set to dummy
    Bit output = If(isLastEntry.getEmpBit(), !nonDummyBinFlag.getEmpBit(), Bit(true, PUBLIC));
    std::string outputStr = output.reveal() ? "true" : "false";

    return Value(output);

}

void SecureGroupByAggregateImpl::updateGroupByBinBoundary(const Value &isNewBin, Value &nonDummyBinFlag) {
    assert(isNewBin.getType() == TypeId::ENCRYPTED_BOOLEAN &&
           nonDummyBinFlag.getType() == TypeId::ENCRYPTED_BOOLEAN);

    // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
    Bit updatedFlag = If(isNewBin.getEmpBit(), Bit(false, PUBLIC), nonDummyBinFlag.getEmpBit());
    nonDummyBinFlag.setValue(updatedFlag);

}

void SecureGroupByCountImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value resetValue = Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
    runningCount = Value::obliviousIf(isDummy.getEmpBit(), runningCount, resetValue);

}

void SecureGroupByCountImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {

    Value toUpdate = (!isDummy) & !tuple.getDummyTag();
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
        Integer empInt = aggInput.getEmpInt();
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
        Integer empInt = toAdd.getEmpInt();
        empInt.resize(64, true);
        toAdd = Value(TypeId::ENCRYPTED_INTEGER64, empInt);
    }


    Value toUpdate = (!isDummy) & !(tuple.getDummyTag());
    Value incremented = runningSum + toAdd;
    runningSum = Value::obliviousIf(toUpdate, incremented, runningSum);
}

Value SecureGroupBySumImpl::getResult() {
    return runningSum;
}

SecureGroupByAvgImpl::SecureGroupByAvgImpl(const int32_t &ordinal, const TypeId &aggType) : SecureGroupByAggregateImpl(ordinal, aggType)  {
    runningSum = zero;
    aggregateType = TypeId::ENCRYPTED_FLOAT32;
    zeroFloat = TypeUtilities::getZero(aggregateType);
    oneFloat = TypeUtilities::getOne(aggregateType);
    runningCount = zeroFloat;


}


void SecureGroupByAvgImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {

    Value aggInput = tuple.getField(aggregateOrdinal).getValue();
    Value resetSum = Value::obliviousIf(tuple.getDummyTag(), zero, aggInput);
    Value resetCount = Value::obliviousIf(tuple.getDummyTag(), zeroFloat, oneFloat);

    runningSum = Value::obliviousIf(isDummy, runningSum, resetSum);
    runningCount = Value::obliviousIf(isDummy, runningCount, resetCount);

}

void SecureGroupByAvgImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    Value toAdd = tuple.getField(aggregateOrdinal).getValue();


    Value toUpdate = (!isDummy) & !(tuple.getDummyTag());
    Value incremented = runningSum + toAdd;
    Value incrementedCount = runningCount + oneFloat;

    runningSum = Value::obliviousIf(toUpdate, incremented, runningSum);
    runningCount = Value::obliviousIf(toUpdate, incrementedCount, runningCount);

}


// average is implemented as float
// have secure cast from int to float for this
Value SecureGroupByAvgImpl::getResult() {

    Float runningSumFloat = (runningSum.getType() == TypeId::ENCRYPTED_FLOAT32) ? runningSum.getEmpFloat32() : zeroFloat.getEmpFloat32();

    if(runningSum.getType() == TypeId::ENCRYPTED_INTEGER64 || runningSum.getType() == TypeId::ENCRYPTED_INTEGER32) {
        Integer empInt = runningSum.getEmpInt();
        if(empInt.size() != 32) empInt.resize(32);
        runningSumFloat = EmpManager::castIntToFloat(empInt);

    }

    return runningSumFloat/runningCount.getEmpFloat32();
}



void SecureGroupByMinImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    // if the tuple is a dummy...
    Value maxValue = getMaxValue();
    Value resetValue = Value::obliviousIf(tuple.getDummyTag(), maxValue, aggInput);

    // use secret initialized variable for debugging
    runningMin = Value::obliviousIf(isDummy, runningMin, resetValue);

}

void SecureGroupByMinImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();
    Value toUpdate = (!isDummy) & !(tuple.getDummyTag()) & (aggInput < runningMin);
    runningMin = Value::obliviousIf(toUpdate, aggInput, runningMin);
}

Value SecureGroupByMinImpl::getResult() {
    return runningMin;
}


Value SecureGroupByMinImpl::getMaxValue() const  {
    switch(aggregateType) {
        case TypeId::ENCRYPTED_INTEGER32:
            return Value(TypeId::ENCRYPTED_INTEGER32, Integer(32, INT_MAX, PUBLIC));
        case TypeId::INTEGER64:
            return Value(TypeId::ENCRYPTED_INTEGER64, Integer(64, LONG_MAX, PUBLIC));
        case TypeId::BOOLEAN:
            return Value(Bit(true, PUBLIC));
        case TypeId::FLOAT32:
            return Value(Float(FLT_MAX, PUBLIC));
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeIdString(aggregateType) + " not supported by MIN()");
    }
}



void SecureGroupByMaxImpl::initialize(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();

    Value minValue = getMinValue();
    Value resetValue = Value::obliviousIf(tuple.getDummyTag(), minValue, aggInput);
    runningMax = Value::obliviousIf(isDummy, runningMax, resetValue);
}

void SecureGroupByMaxImpl::accumulate(const QueryTuple &tuple, const Value &isDummy) {
    Value aggInput = tuple.getField(aggregateOrdinal).getValue();
    Value toUpdate = (!isDummy) & !(tuple.getDummyTag()) & (aggInput > runningMax);
    runningMax = Value::obliviousIf(toUpdate, aggInput, runningMax);

}

Value SecureGroupByMaxImpl::getResult() {
    return runningMax;
}

Value SecureGroupByMaxImpl::getMinValue() const {
    switch(aggregateType) {
        case TypeId::ENCRYPTED_INTEGER32:
            return Value(TypeId::ENCRYPTED_INTEGER32, Integer(32, INT_MIN, PUBLIC));
        case TypeId::INTEGER64:
            return Value(TypeId::ENCRYPTED_INTEGER64, Integer(64, LONG_MIN, PUBLIC));
        case TypeId::BOOLEAN:
            return Value(Bit(false, PUBLIC));
        case TypeId::FLOAT32:
            return Value(Float(FLT_MIN, PUBLIC));
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeIdString(aggregateType) + " not supported by MAX()");
    }
}
