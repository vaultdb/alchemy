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


void vaultdb::ScalarMin::initialize(const vaultdb::QueryTuple &tuple) {

  assert(!tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId sumType = tupleVal.getType();
  zero = TypeUtilities::getZero(sumType);

  runningMin = tuple.getDummyTag().getBool() ? zero : tupleVal;
  initialized = true;
}


void vaultdb::ScalarMin::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(!tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value currMin = (tupleVal < currMin).getBool() ? tupleVal : currMin;
  runningMin = tuple.getDummyTag().getBool() ? runningMin : currMin;

}

vaultdb::types::Value vaultdb::ScalarMin::getResult() {
  return vaultdb::types::Value(runningMin);
}

types::TypeId ScalarMin::getType() {
  return runningMin.getType();
}


void vaultdb::ScalarMax::initialize(const vaultdb::QueryTuple &tuple) {

  assert(!tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId sumType = tupleVal.getType();
  zero = TypeUtilities::getZero(sumType);

  runningMax = tuple.getDummyTag().getBool() ? zero : tupleVal;
  initialized = true;
}


void vaultdb::ScalarMax::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(!tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value currMax = (tupleVal > currMax).getBool() ? tupleVal : currMax;
  runningMax = tuple.getDummyTag().getBool() ? runningMax : currMax;

}

vaultdb::types::Value vaultdb::ScalarMax::getResult() {
  return vaultdb::types::Value(runningMax);
}

types::TypeId ScalarMax::getType() {
  return runningMax.getType();
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
    runningCount = (tuple.getDummyTag().getBool()) ? runningCount : tupleCount;
}

types::Value ScalarAverage::getResult() {

    average = (runningCount!=zero).getBool() ? zero : runningSum/runningCount;
    return types::Value(average);
}

types::TypeId ScalarAverage::getType() {
    assert(initialized);
    return average.getType();
}