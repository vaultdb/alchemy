#include "secure_scalar_aggregate_impl.h"
#include <util/emp_manager.h>
#include <util/type_utilities.h>

using namespace vaultdb;


void vaultdb::SecureScalarAverage::initialize(const vaultdb::QueryTuple &tuple) {
    assert(tuple.isEncrypted());
    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::TypeId avgType = tupleVal.getType();

    // initialize member variable for use in accumulate()

    runningSum =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
    runningCount = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zeroFloat , oneFloat);
    initialized = true;

}

void vaultdb::SecureScalarAverage::accumulate(const vaultdb::QueryTuple &tuple) {
    assert(tuple.isEncrypted());
    assert(initialized);

    types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
    types::Value toAdd = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
    runningSum =  runningSum + toAdd;
    runningCount = runningCount +
            types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zeroFloat , oneFloat);
}

vaultdb::types::Value vaultdb::SecureScalarAverage::getResult() {

  emp::Float runningSumFloat = (runningSum.getType() == types::TypeId::ENCRYPTED_FLOAT32) ? runningSum.getEmpFloat32() : TypeUtilities::getZero(types::TypeId::ENCRYPTED_FLOAT32).getEmpFloat32();

  if(runningSum.getType() == types::TypeId::ENCRYPTED_INTEGER64 || runningSum.getType() == types::TypeId::ENCRYPTED_INTEGER32) {
    emp::Integer empInt = runningSum.getEmpInt();
    if(empInt.size() != 32) empInt.resize(32);
    runningSumFloat = EmpManager::castIntToFloat(empInt);

  }

  return runningSumFloat/runningCount.getEmpFloat32();
}

SecureScalarAverage::SecureScalarAverage(const uint32_t &ordinal,
                                         const types::TypeId &aggType)
    : ScalarAggregateImpl(ordinal, aggType) {

  runningSum = zero;
  aggregateType = types::TypeId::ENCRYPTED_FLOAT32;
  zeroFloat = TypeUtilities::getZero(aggregateType);
  oneFloat = TypeUtilities::getOne(aggregateType);
  runningCount = zeroFloat;
}

void vaultdb::SecureScalarCount::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId countType = tupleVal.getType();

  // initialize member variable for use in accumulate()
  //zero = TypeUtilities::getZero(countType);
  //one = TypeUtilities::getOne(countType);

  //types::Value addValue = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
  runningCount = runningCount + types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero, one);
  initialized = true;

}

void vaultdb::SecureScalarCount::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  runningCount = runningCount +
                 types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , one);

//  types::Value toUpdate = !isDummy & !tuple.getDummyTag();
  types::Value incremented = runningCount + one;
  runningCount = types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), incremented, runningCount);
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
  //zero = TypeUtilities::getZero(sumType);

    // re-cast sum as INT64_T in keeping with postgres convention
    if(tupleVal.getType() == types::TypeId::ENCRYPTED_INTEGER32) {
        emp::Integer empInt = tupleVal.getEmpInt();
        empInt.resize(64, true);
        tupleVal = types::Value(types::TypeId::ENCRYPTED_INTEGER64, empInt);
    }

  runningSum =  types::Value::obliviousIf(tuple.getDummyTag(), zero , tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarSum::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();

  // re-cast sum as INT64_T in keeping with postgres convention
  if(tupleVal.getType() == types::TypeId::ENCRYPTED_INTEGER32) {
        emp::Integer empInt = tupleVal.getEmpInt();
        empInt.resize(64, true);
        tupleVal = types::Value(types::TypeId::ENCRYPTED_INTEGER64, empInt);
  }

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

  types::Value getMaxValue();
  // initialize member variable for use in accumulate()
  //zero = TypeUtilities::getZero(minType);

  runningMin =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), zero , tupleVal);
  initialized = true;

}

void vaultdb::SecureScalarMin::accumulate(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  assert(initialized);

  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::Value currMin = types::Value::obliviousIf((tupleVal < currMin).getEmpBit(), tupleVal, runningMin);
  runningMin =  types::Value::obliviousIf(tuple.getDummyTag().getEmpBit(), runningMin , currMin);

}

vaultdb::types::Value vaultdb::SecureScalarMin::getResult() {
  return runningMin;
}

vaultdb::types::TypeId vaultdb::SecureScalarMin::getType() {
  assert(initialized);
  return runningMin.getType();
}

//vaultdb::types::Value SecureScalarMaxImpl::getMaxValue() const  {
//  switch(aggregateType) {
//  case TypeId::ENCRYPTED_INTEGER32:
//    return Value(TypeId::ENCRYPTED_INTEGER32, Integer(32, INT_MIN, PUBLIC));
//  case TypeId::INTEGER64:
//    return Value(TypeId::ENCRYPTED_INTEGER64, Integer(64, LONG_MIN, PUBLIC));
//  case TypeId::BOOLEAN:
//    return Value(Bit(true, PUBLIC));
//  case TypeId::FLOAT32:
//    return Value(Float(FLT_MAX, PUBLIC));
//  default:
//    throw std::invalid_argument("Type " + TypeUtilities::getTypeIdString(aggregateType) + " not supported by MIN()");
//  }
//}

void vaultdb::SecureScalarMax::initialize(const vaultdb::QueryTuple &tuple) {
  assert(tuple.isEncrypted());
  types::Value tupleVal = tuple.getFieldPtr(aggregateOrdinal)->getValue();
  types::TypeId maxType = tupleVal.getType();

  // initialize member variable for use in accumulate()
  //zero = TypeUtilities::getZero(maxType);

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


//Value SecureScalarMaxImpl::getMinValue() const {
//  switch(aggregateType) {
//  case TypeId::ENCRYPTED_INTEGER32:
//    return Value(TypeId::ENCRYPTED_INTEGER32, Integer(32, INT_MIN, PUBLIC));
//  case TypeId::INTEGER64:
//    return Value(TypeId::ENCRYPTED_INTEGER64, Integer(64, LONG_MIN, PUBLIC));
//  case TypeId::BOOLEAN:
//    return Value(Bit(false, PUBLIC));
//  case TypeId::FLOAT32:
//    return Value(Float(FLT_MIN, PUBLIC));
//  default:
//    throw std::invalid_argument("Type " + TypeUtilities::getTypeIdString(aggregateType) + " not supported by MAX()");
//  }
//}