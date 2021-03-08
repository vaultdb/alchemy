#include "secure_replace_tuple.h"

#include <utility>

using namespace vaultdb;

SecureReplaceTuple::SecureReplaceTuple(std::shared_ptr<QueryTable> table) : ReplaceTuple(std::move(table)) {}

void SecureReplaceTuple::conditionalWrite(const uint32_t &writeIdx, const QueryTuple &srcTuple,
                                          const Field &toWrite) {

    assert(dstTable->isEncrypted());
    QueryTuple toCopy(srcTuple);
    QueryTuple *dstTuple = dstTable->getTuplePtr(writeIdx);

    QueryTuple::compareAndSwap(TypeUtilities::getSecureBool(toWrite), dstTuple, &toCopy);

}
