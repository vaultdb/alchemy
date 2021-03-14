#include <util/emp_manager.h>
#include "secure_group_by_aggregate_impl.h"

using namespace vaultdb;
using namespace emp;

Field *SecureGroupByAggregateImpl::getDummyTag(const Field *isLastEntry,
                                                                       const Field *nonDummyBinFlag) {

    assert(isLastEntry->getType() == FieldType::SECURE_BOOL &&
                   nonDummyBinFlag->getType() == FieldType::SECURE_BOOL);

    // if it is the last bin, then we output if it's a non-dummy bin
    // else set to dummy
    Bit output = If(TypeUtilities::getSecureBool(*isLastEntry), !TypeUtilities::getSecureBool(*nonDummyBinFlag), Bit(true, PUBLIC));
    std::string outputStr = output.reveal() ? "true" : "false";

    return new SecureBoolField(output);

}

void SecureGroupByAggregateImpl::updateGroupByBinBoundary(const Field *isNewBin, Field *nonDummyBinFlag) {
    assert(isNewBin->getType() == FieldType::SECURE_BOOL &&
           nonDummyBinFlag->getType() == FieldType::SECURE_BOOL);

    // ! nonDummy bin b/c if it is a real bin, we want dummyTag = false
    Bit updatedFlag = If(TypeUtilities::getSecureBool(*isNewBin), Bit(false, PUBLIC), TypeUtilities::getSecureBool(*nonDummyBinFlag));
    *nonDummyBinFlag = SecureBoolField(updatedFlag);

}

void SecureGroupByCountImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {
    Bit dummyTag = TypeUtilities::getSecureBool(*tuple.getDummyTag());
    Integer resetField = emp::If(dummyTag, Integer(64, 0), emp::Integer(64, 1));
    runningCount = emp::If(TypeUtilities::getSecureBool(*isDummy), runningCount, resetValue);

}

void SecureGroupByCountImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {

    emp::Bit toUpdate = (!TypeUtilities::getSecureBool((*isDummy)) & (!(TypeUtilities::getSecureBool(*tuple.getDummyTag()))));
    Integer incremented = runningCount + TypeUtilities::getSecureInt(*one);
    runningCount = emp::If(toUpdate, incremented, runningCount);

}

const Field *SecureGroupByCountImpl::getResult() const {
    return (Field *) (new SecureLongField(runningCount));
}


void SecureGroupBySumImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {
     Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));

    // re-cast sum as INT64_T in keeping with postgres convention
    if(aggInput->getType() == FieldType::SECURE_INT32) {
        Integer empInt = TypeUtilities::getSecureInt(*aggInput);
        empInt.resize(64, true);
        *aggInput = SecureLongField(empInt);
    }

    Field *resetField = FieldFactory::obliviousIf(*tuple.getDummyTag(), *zero, *aggInput);
    runningSum = FieldFactory::obliviousIf(*isDummy, *runningSum, *resetValue);
}

void SecureGroupBySumImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {
     Field *toAdd =  FieldFactory::copyField(tuple.getField(aggregateOrdinal));

    // re-cast sum as INT64_T in keeping with postgres convention
    if(toAdd->getType() == FieldType::SECURE_INT32) {
        Integer empInt = TypeUtilities::getSecureInt(*toAdd);
        empInt.resize(64, true);
        *toAdd = SecureLongField(empInt);
    }


    const Field *toUpdate = &((!isDummy) & !(tuple.getDummyTag()));
    const Field *incremented = &(*runningSum + *toAdd);
    *runningSum = *(FieldFactory::obliviousIf(*toUpdate, *incremented, *runningSum));
}

const Field *SecureGroupBySumImpl::getResult() const {
    return runningSum;
}

SecureGroupByAvgImpl::SecureGroupByAvgImpl(const int32_t &ordinal, const FieldType &aggType) : SecureGroupByAggregateImpl(ordinal, aggType)  {
    runningSum = zero;
    aggregateType = FieldType::SECURE_FLOAT32;
    zeroFloat = FieldFactory::getZero(aggregateType);
    oneFloat = FieldFactory::getOne(aggregateType);
    runningCount = zeroFloat;


}


void SecureGroupByAvgImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {

    Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));
    Field *resetSum = FieldFactory::obliviousIf(*tuple.getDummyTag(), *zero, *aggInput);
    Field resetCount = FieldFactory::obliviousIf(*tuple.getDummyTag(), *zeroFloat, *oneFloat);

    runningSum = FieldFactory::obliviousIf(isDummy, runningSum, resetSum);
    runningCount = FieldFactory::obliviousIf(isDummy, runningCount, resetCount);

}

void SecureGroupByAvgImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {
    Field *toAdd = FieldFactory::copyField(tuple.getField(aggregateOrdinal));


    Field toUpdate = (!isDummy) & !(tuple.getDummyTag());
    Field incremented = runningSum + toAdd;
    Field incrementedCount = runningCount + oneFloat;

    runningSum = FieldFactory::obliviousIf(toUpdate, incremented, runningSum);
    runningCount = FieldFactory::obliviousIf(toUpdate, incrementedCount, runningCount);

}


// average is implemented as float
// have secure cast from int to float for this
const Field *SecureGroupByAvgImpl::getResult() const {

    Float runningSumFloat = (runningSum->getType() == FieldType::ENCRYPTED_FLOAT32) ? runningSum.getEmpFloat32() : zeroFloat.getEmpFloat32();

    if(runningSum->getType() == FieldType::SECURE_INT64 || runningSum->getType() == FieldType::SECURE_INT32) {
        Integer empInt = runningSum.getEmpInt();
        if(empInt.size() != 32) empInt.resize(32);
        runningSumFloat = EmpManager::castIntToFloat(empInt);

    }

    return runningSumFloat/runningCount.getEmpFloat32();
}



void SecureGroupByMinImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {
    Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));

    // if the tuple is a dummy...
    Field maxField = getMaxValue();
    Field resetField = FieldFactory::obliviousIf(tuple.getDummyTag(), maxValue, aggInput);

    // use secret initialized variable for debugging
    runningMin = FieldFactory::obliviousIf(isDummy, runningMin, resetValue);

}

void SecureGroupByMinImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {
    Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));
    Field toUpdate = (!isDummy) & !(tuple.getDummyTag()) & (aggInput < runningMin);
    runningMin = FieldFactory::obliviousIf(toUpdate, aggInput, runningMin);
}

const Field * SecureGroupByMinImpl::getResult() const {
    return runningMin;
}


Field SecureGroupByMinImpl::getMaxValue() const  {
    switch(aggregateType) {
        case FieldType::SECURE_INT32:
            return Value(FieldType::SECURE_INT32, Integer(32, INT_MAX, PUBLIC));
        case FieldType::INTEGER64:
            return Value(FieldType::SECURE_INT64, Integer(64, LONG_MAX, PUBLIC));
        case FieldType::BOOLEAN:
            return Value(Bit(true, PUBLIC));
        case FieldType::FLOAT32:
            return Value(Float(FLT_MAX, PUBLIC));
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(aggregateType) + " not supported by MIN()");
    }
}



void SecureGroupByMaxImpl::initialize(const QueryTuple &tuple, const Field *isDummy) {
    Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));

    Field minField = getMinValue();
    Field resetField = FieldFactory::obliviousIf(tuple.getDummyTag(), minValue, aggInput);
    runningMax = FieldFactory::obliviousIf(isDummy, runningMax, resetValue);
}

void SecureGroupByMaxImpl::accumulate(const QueryTuple &tuple, const Field *isDummy) {
    Field *aggInput = FieldFactory::copyField(tuple.getField(aggregateOrdinal));
    Field toUpdate = (!isDummy) & !(tuple.getDummyTag()) & (aggInput > runningMax);
    runningMax = FieldFactory::obliviousIf(toUpdate, aggInput, runningMax);

}

const Field * SecureGroupByMaxImpl::getResult() const {
    return runningMax;
}

Field *SecureGroupByMaxImpl::getMinValue() const {
    switch(aggregateType) {
        case FieldType::SECURE_INT32:
            return new SecureIntField(Integer(32, INT_MIN, PUBLIC));
        case FieldType::SECURE_INT64:
            return new SecureLongField(Integer(64, LONG_MIN, PUBLIC));
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(Bit(false, PUBLIC));
        case FieldType::SECURE_FLOAT32:
            return new SecureFloatField(Float(FLT_MIN, PUBLIC));
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(aggregateType) + " not supported by MAX()");
    }
}
