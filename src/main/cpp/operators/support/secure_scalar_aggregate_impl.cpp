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
