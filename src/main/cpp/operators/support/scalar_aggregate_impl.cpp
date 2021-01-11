#include <util/type_utilities.h>
#include "scalar_aggregate_impl.h"

using namespace vaultdb;

void vaultdb::ScalarCount::initialize(const vaultdb::QueryTuple &tuple) {

    assert(!tuple.isEncrypted());
    runningCount = tuple.getDummyTag().getBool() ? 0 : 1;
    initialized = true;
}


void vaultdb::ScalarCount::accumulate(const vaultdb::QueryTuple &tuple) {
    assert(!tuple.isEncrypted());
    assert(initialized);

    runningCount += tuple.getDummyTag().getBool() ? 0 : 1;


}


vaultdb::types::Value vaultdb::ScalarCount::getResult() {
    return vaultdb::types::Value(runningCount);
}

types::TypeId ScalarCount::getType() {
    return types::TypeId::INTEGER64;
}



void vaultdb::ScalarSum::initialize(const vaultdb::QueryTuple &tuple) {
    assert(!tuple.isEncrypted());
    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::TypeId sumType = tupleVal.getType();

    // initialize member variable for use in accumulate()
    zero = TypeUtilities::getZero(sumType);
    runningSum = tuple.getDummyTag().getBool() ? zero : tupleVal;
    initialized = true;
}

void vaultdb::ScalarSum::accumulate(const vaultdb::QueryTuple &tuple) {
    assert(!tuple.isEncrypted());
    assert(initialized);

    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::Value toAdd = tuple.getDummyTag().getBool() ? zero : tupleVal;
    runningSum =  runningSum + toAdd;

}

vaultdb::types::Value vaultdb::ScalarSum::getResult() {
    return runningSum;
}

types::TypeId ScalarSum::getType() {
    assert(initialized);
    return runningSum.getType();
}


ScalarMin::ScalarMin(const uint32_t & ordinal, const types::TypeId & aggType) :  ScalarAggregateImpl(ordinal, aggType) {
  resetRunningMin();
}

void vaultdb::ScalarMin::initialize(const vaultdb::QueryTuple &tuple) {

  assert(!tuple.isEncrypted());
  resetRunningMin();
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  zero = TypeUtilities::getZero(aggregateType);
  initialized = true;
}


void vaultdb::ScalarMin::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(!tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value currMin = (tupleVal < runningMin).getBool() ? tupleVal : runningMin;
  runningMin = tuple.getDummyTag().getBool() ? runningMin : currMin;

}

vaultdb::types::Value vaultdb::ScalarMin::getResult() {
  return vaultdb::types::Value(runningMin);
}

types::TypeId ScalarMin::getType() {
  return runningMin.getType();
}

void vaultdb::ScalarMin::resetRunningMin()  {

  switch(aggregateType) {
  case types::TypeId::INTEGER32:
    runningMin = types::Value(INT_MAX);
    break;
  case types::TypeId::INTEGER64:
    runningMin = types::Value(aggregateType, LONG_MAX);
    break;
  case types::TypeId::BOOLEAN:
    runningMin = types::Value(true);
    break;
  case types::TypeId::FLOAT32:
    runningMin = types::Value(FLT_MAX);
    break;
  default:
    throw std::invalid_argument("Type " + TypeUtilities::getTypeIdString(aggregateType) + " not supported by MIN()");
  }
}

ScalarMax::ScalarMax(const uint32_t & ordinal, const types::TypeId & aggType) :  ScalarAggregateImpl(ordinal, aggType) {
  resetRunningMax();
}

void vaultdb::ScalarMax::initialize(const vaultdb::QueryTuple &tuple) {

  assert(!tuple.isEncrypted());
  resetRunningMax();
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  zero = TypeUtilities::getZero(aggregateType);
  runningMax = tuple.getDummyTag().getBool() ? zero : tupleVal;
  initialized = true;
}


void vaultdb::ScalarMax::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(!tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value currMax = (tupleVal > runningMax).getBool() ? tupleVal : runningMax;
  runningMax = tuple.getDummyTag().getBool() ? runningMax : currMax;

}

vaultdb::types::Value vaultdb::ScalarMax::getResult() {
  return vaultdb::types::Value(runningMax);
}

types::TypeId ScalarMax::getType() {
  return runningMax.getType();
}

void vaultdb::ScalarMax::resetRunningMax()  {

  switch(runningMax.getType()) {
  case types::TypeId::INTEGER32:
    runningMax = types::Value(INT_MIN);
    break;
  case types::TypeId::INTEGER64:
    runningMax = types::Value(maxType, LONG_MIN);
    break;
  case types::TypeId::BOOLEAN:
    runningMax = types::Value(true);
    break;
  case types::TypeId::FLOAT32:
    runningMax = types::Value(FLT_MIN);
    break;
  default:
    throw std::invalid_argument("Type " + TypeUtilities::getTypeIdString(maxType) + " not supported by MAX()");
  }
}

void ScalarAverage::initialize(const QueryTuple &tuple) {

    assert(!tuple.isEncrypted());
    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::TypeId sumType = tupleVal.getType();

    // initialize member variable for use in accumulate()
    zero = TypeUtilities::getZero(sumType);
    one = TypeUtilities::getOne(sumType);
    runningSum = tuple.getDummyTag().getBool() ? zero : tupleVal;
    runningCount = tuple.getDummyTag().getBool() ? zero : one;
    initialized = true;

}

void ScalarAverage::accumulate(const QueryTuple &tuple) {

    assert(!tuple.isEncrypted());
    assert(initialized);

    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::Value toAdd = tuple.getDummyTag().getBool() ? zero : tupleVal;
    types::Value tupleCount = tuple.getDummyTag().getBool() ? zero : one;

    runningSum =  runningSum + toAdd;
    runningCount = runningCount + tupleCount;
}

types::Value ScalarAverage::getResult() {

    assert(initialized);

    if((runningCount == zero).getBool()) { return zero; }

    return types::Value(runningSum/runningCount);
}

types::TypeId ScalarAverage::getType() {
    assert(initialized);
    return runningCount.getType();
}