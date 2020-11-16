#include <util/type_utilities.h>
#include "secure_scalar_aggregate_impl.h"

void vaultdb::SecureScalarAverage::initialize(const vaultdb::QueryTuple &tuple) {
    assert(tuple.isEncrypted());
    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::TypeId avgType = tupleVal.getType();

    // initialize member variable for use in accumulate()
    zero = TypeUtilities::getZero(avgType);
    one = TypeUtilities::getOne(avgType);

    runningSum =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
    runningCount = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , one);
    initialized = true;

}

void vaultdb::SecureScalarAverage::accumulate(const vaultdb::QueryTuple &tuple) {
    assert(tuple.isEncrypted());
    assert(initialized);

    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::Value toAdd = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
    runningSum =  runningSum + toAdd;
    runningCount = runningCount +
            types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , one);
}

vaultdb::types::Value vaultdb::SecureScalarAverage::getResult() {
    return runningSum / runningCount;
}

vaultdb::types::TypeId vaultdb::SecureScalarAverage::getType() {
    assert(initialized);
    return runningSum.getType();
}


void vaultdb::SecureScalarCount::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId countType = tupleVal.getType();

  // initialize member variable for use in accumulate()
  zero = TypeUtilities::getZero(countType);
  one = TypeUtilities::getOne(countType);

  runningCount = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , one);
  initialized = true;

}

void vaultdb::SecureScalarCount::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  runningCount = runningCount +
                 types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , one);
}

vaultdb::types::Value vaultdb::SecureScalarCount::getResult() {
  return runningCount;
}

vaultdb::types::TypeId vaultdb::SecureScalarCount::getType() {
  assert(initialized);
  return runningCount.getType();
}


void vaultdb::SecureScalarSum::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId sumType = tupleVal.getType();

  // initialize member variable for use in accumulate()
  zero = TypeUtilities::getZero(sumType);

  runningSum =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarSum::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value toAdd = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
  runningSum =  runningSum + toAdd;
}

vaultdb::types::Value vaultdb::SecureScalarSum::getResult() {
  return runningSum;
}

vaultdb::types::TypeId vaultdb::SecureScalarSum::getType() {
  assert(initialized);
  return runningSum.getType();
}


void vaultdb::SecureScalarMin::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId minType = tupleVal.getType();

  // initialize member variable for use in accumulate()
  zero = TypeUtilities::getZero(minType);

  runningMin =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarMin::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value currMin = types::Value::obliviousIf((tupleVal < currMin).getEmpBit(), tupleVal, currMin);
  runningMin =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), runningMin , currMin);

}

vaultdb::types::Value vaultdb::SecureScalarMin::getResult() {
  return runningMin;
}

vaultdb::types::TypeId vaultdb::SecureScalarMin::getType() {
  assert(initialized);
  return runningMin.getType();
}


void vaultdb::SecureScalarMax::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId maxType = tupleVal.getType();

  // initialize member variable for use in accumulate()
  zero = TypeUtilities::getZero(maxType);

  runningMax =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarMax::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value currMax = types::Value::obliviousIf((tupleVal > currMax).getEmpBit(), tupleVal, currMax);
  runningMax =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), runningMax , currMax);

}

vaultdb::types::Value vaultdb::SecureScalarMax::getResult() {
  return runningMax;
}

vaultdb::types::TypeId vaultdb::SecureScalarMax::getType() {
  assert(initialized);
  return runningMax.getType();
}