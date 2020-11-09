#include <util/type_utilities.h>
#include "scalar_aggregate_impl.h"

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
