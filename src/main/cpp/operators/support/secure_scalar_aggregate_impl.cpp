#include "secure_scalar_aggregate_impl.h"
#include <util/emp_manager.h>
#include <util/type_utilities.h>

using namespace vaultdb;


void vaultdb::SecureScalarAverage::initialize(const vaultdb::QueryTuple &tuple) {
    assert(tuple.isEncrypted());
    Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();

    // initialize member variable for use in accumulate()

    runningSum =  Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
    runningCount = Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zeroFloat , oneFloat);
    initialized = true;

}

void vaultdb::SecureScalarAverage::accumulate(const vaultdb::QueryTuple &tuple) {
    assert(tuple.isEncrypted());
    assert(initialized);

    Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    Field toAdd = Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
    runningSum =  runningSum + toAdd;
    runningCount = runningCount +
            Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zeroFloat , oneFloat);
}

const Field * vaultdb::SecureScalarAverage::getResult() {

  emp::Float runningSumFloat = (runningSum.getType() == FieldType::SECURE_FLOAT) ? runningSum.getEmpFloat32() : TypeUtilities::getZero(FieldType::SECURE_FLOAT).getEmpFloat32();

  if(runningSum.getType() == FieldType::SECURE_LONG || runningSum.getType() == FieldType::SECURE_INT) {
    emp::Integer empInt = runningSum.getEmpInt();
    if(empInt.size() != 32) empInt.resize(32);
    runningSumFloat = EmpManager::castIntToFloat(empInt);

  }

  return runningSumFloat/runningCount.getEmpFloat32();
}

SecureScalarAverage::SecureScalarAverage(const uint32_t &ordinal,
                                         const TypeId &aggType)
    : ScalarAggregateImpl(ordinal, aggType) {

  runningSum = zero;
  aggregateType = FieldType::SECURE_FLOAT;
  zeroFloat = TypeUtilities::getZero(aggregateType);
  oneFloat = TypeUtilities::getOne(aggregateType);
  runningCount = zeroFloat;
}

void vaultdb::SecureScalarCount::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());

  //Field addField = Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
  runningCount = runningCount + Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
  initialized = true;

}

void vaultdb::SecureScalarCount::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  runningCount = runningCount + Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
}

const Field * vaultdb::SecureScalarCount::getResult() {
  return runningCount;
}

vaultdb::TypeId vaultdb::SecureScalarCount::getType() {
  assert(initialized);
  return runningCount.getType();
}


void vaultdb::SecureScalarSum::initialize(const QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();

    // re-cast sum as INT64_T in keeping with postgres convention
    if(tupleVal.getType() == FieldType::SECURE_INT) {
        emp::Integer empInt = tupleVal.getEmpInt();
        empInt.resize(64, true);
        tupleVal = Value(FieldType::SECURE_LONG, empInt);
    }

  //
  runningSum = Value::obliviousIf(tuple.getDummyTag(), zero, tupleVal);
  // Field resetField =  Value::obliviousIf(tuple.getDummyTag(), zero , tupleVal);
  //runningSum = runningSum + Value::obliviousIf(tuple.getDummyTag(), zero, tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarSum::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();

  // re-cast sum as INT64_T in keeping with postgres convention
  if(tupleVal.getType() == FieldType::SECURE_INT) {
        emp::Integer empInt = tupleVal.getEmpInt();
        empInt.resize(64, true);
        tupleVal = Value(FieldType::SECURE_LONG, empInt);
  }

  Field toAdd = Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, tupleVal);
  runningSum = runningSum + toAdd;

}

const Field * vaultdb::SecureScalarSum::getResult() {
  return runningSum;
}


void vaultdb::SecureScalarMin::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();

  runningMin =  Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarMin::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  Field currMin = Value::obliviousIf((tupleVal < runningMin).getEmpBit(), tupleVal, runningMin);
  runningMin =  Value::obliviousIf(tuple.getDummyTag().getEmpBit(), runningMin , currMin);

}

const Field * vaultdb::SecureScalarMin::getResult() {
  return runningMin;
}

vaultdb::TypeId vaultdb::SecureScalarMin::getType() {
  assert(initialized);
  return runningMin.getType();
}




void vaultdb::SecureScalarMax::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();

  runningMax =  Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarMax::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  Field tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  Field currMax = Value::obliviousIf((tupleVal > runningMax).getEmpBit(), tupleVal, runningMax);
  runningMax =  Value::obliviousIf(tuple.getDummyTag().getEmpBit(), runningMax , currMax);

}

const Field * vaultdb::SecureScalarMax::getResult() {
  return runningMax;
}

vaultdb::TypeId vaultdb::SecureScalarMax::getType() {
  assert(initialized);
  return runningMax.getType();
}


//Field SecureScalarMaxImpl::getMinValue() const {
//  switch(aggregateType) {
//  case FieldType::SECURE_INT:
//    return Value(FieldType::SECURE_INT, Integer(32, INT_MIN, PUBLIC));
//  case FieldType::LONG:
//    return Value(FieldType::SECURE_LONG, Integer(64, LONG_MIN, PUBLIC));
//  case FieldType::BOOL:
//    return Value(Bit(false, PUBLIC));
//  case FieldType::FLOAT:
//    return Value(Float(FLT_MIN, PUBLIC));
//  default:
//    throw std::invalid_argument("Type " + TypeUtilities::getTypeString(aggregateType) + " not supported by MAX()");
//  }
//}