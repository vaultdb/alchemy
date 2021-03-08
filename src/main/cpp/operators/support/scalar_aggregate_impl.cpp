#include <util/type_utilities.h>
#include "scalar_aggregate_impl.h"
#include "query_table/field/bool_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/float_field.h"


using namespace vaultdb;

void vaultdb::ScalarCount::initialize(const vaultdb::QueryTuple &tuple) {

    assert(!tuple.isEncrypted());
    runningCount = TypeUtilities::getBool(*tuple.getDummyTag()) ? 0 : 1;
    initialized = true;
}


void vaultdb::ScalarCount::accumulate(const vaultdb::QueryTuple &tuple) {
    assert(!tuple.isEncrypted());
    assert(initialized);

    runningCount += TypeUtilities::getBool(*tuple.getDummyTag()) ? 0 : 1;


}


const Field * vaultdb::ScalarCount::getResult() const {
    return new LongField(runningCount);
}

FieldType ScalarCount::getType() {
    return FieldType::INT64;
}



void vaultdb::ScalarSum::initialize(const vaultdb::QueryTuple &tuple) {
    assert(!tuple.isEncrypted());
    const Field * tupleVal = tuple.getField(aggregateOrdinal);
    FieldType sumType = tupleVal->getType();

    // initialize member variable for use in accumulate()
    zero = FieldFactory::getZero(sumType);
    if (TypeUtilities::getBool(*tuple.getDummyTag())) {
        *runningSum = *zero;
    } else {
        *runningSum = *tupleVal;
    }
    initialized = true;
}

void vaultdb::ScalarSum::accumulate(const vaultdb::QueryTuple &tuple) {
    assert(!tuple.isEncrypted());
    assert(initialized);

    const Field * tupleVal = tuple.getField(aggregateOrdinal);
    const Field * toAdd = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : tupleVal;
    runningSum =  &(*runningSum + *toAdd);

}

const Field * vaultdb::ScalarSum::getResult() const {
    return runningSum;
}

FieldType ScalarSum::getType() {
    assert(initialized);
    return runningSum->getType();
}


ScalarMin::ScalarMin(const uint32_t & ordinal, const FieldType & aggType) :  ScalarAggregateImpl(ordinal, aggType) {
  resetRunningMin();
}

void vaultdb::ScalarMin::initialize(const vaultdb::QueryTuple &tuple) {

  assert(!tuple.isEncrypted());
  resetRunningMin();
  const Field * tupleVal = tuple.getField(aggregateOrdinal);;
  zero = FieldFactory::getZero(aggregateType);
  initialized = true;
}


void vaultdb::ScalarMin::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(!tuple.isEncrypted());
  assert(initialized);

  const Field * tupleVal = tuple.getField(aggregateOrdinal);
  bool newMin = TypeUtilities::getBool(*tupleVal < *runningMin);

  const Field * currMin = newMin ? tupleVal : runningMin;
  bool isDummy =  TypeUtilities::getBool(*tuple.getDummyTag());
  *runningMin = isDummy ? *runningMin : *currMin;


}

const Field * vaultdb::ScalarMin::getResult() const {
  return runningMin;
}

FieldType ScalarMin::getType() {
  return runningMin->getType();
}

void vaultdb::ScalarMin::resetRunningMin()  {
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
  }
}

ScalarMax::ScalarMax(const uint32_t & ordinal, const FieldType & aggType) :  ScalarAggregateImpl(ordinal, aggType) {
  resetRunningMax();
}

void vaultdb::ScalarMax::initialize(const vaultdb::QueryTuple &tuple) {

  assert(!tuple.isEncrypted());
  resetRunningMax();
  const Field * tupleVal = tuple.getField(aggregateOrdinal);;
  zero = FieldFactory::getZero(aggregateType);
  *runningMax = TypeUtilities::getBool(*tuple.getDummyTag()) ? *zero : *tupleVal;
  initialized = true;
}


void vaultdb::ScalarMax::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(!tuple.isEncrypted());
  assert(initialized);

  const Field * tupleVal = tuple.getField(aggregateOrdinal);
  bool newMax = TypeUtilities::getBool(*tupleVal > *runningMax);
  const Field * currMax = newMax ? tupleVal : runningMax;
    if (TypeUtilities::getBool(*tuple.getDummyTag())) {
        runningMax = runningMax;
    } else {
        *runningMax = *currMax;
    }

}

const Field * vaultdb::ScalarMax::getResult() const {
  return runningMax;
}

FieldType ScalarMax::getType() {
  return runningMax->getType();
}

void vaultdb::ScalarMax::resetRunningMax()  {
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

void ScalarAverage::initialize(const QueryTuple &tuple) {

    assert(!tuple.isEncrypted());
    const Field * tupleVal = tuple.getField(aggregateOrdinal);;
    FieldType sumType = tupleVal->getType();

    // initialize member variable for use in accumulate()
    zero =
    one = FieldFactory::getOne(sumType);
    bool isDummy = TypeUtilities::getBool(*tuple.getDummyTag());
    if(isDummy) {
        runningSum = FieldFactory::getZero(sumType);
        runningCount = FieldFactory::getZero(sumType);
    }
    else {
        runningSum = FieldFactory::copyField(tupleVal);
        runningCount = FieldFactory::getOne(sumType);
        }
    initialized = true;

}

void ScalarAverage::accumulate(const QueryTuple &tuple) {

    assert(!tuple.isEncrypted());
    assert(initialized);

    const Field * tupleVal = tuple.getField(aggregateOrdinal);;
    const Field * toAdd = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : tupleVal;
    Field * tupleCount = TypeUtilities::getBool(*tuple.getDummyTag()) ? zero : one;

    *runningSum =  *runningSum + *toAdd;
    *runningCount = *runningCount + *tupleCount;
}

const Field * ScalarAverage::getResult() const {

    assert(initialized);

    if(TypeUtilities::getBool(*runningCount == *zero)) { return zero; }

    return &(*runningSum / *runningCount);
}

FieldType ScalarAverage::getType() {
    assert(initialized);
    return runningCount->getType();
}