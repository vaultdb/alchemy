#include "replace_tuple.h"

using namespace vaultdb;

ReplaceTuple::ReplaceTuple(std::shared_ptr<QueryTable> table) {
    dstTable = table;

}

void
ReplaceTuple::conditionalWrite(const uint32_t &writeIdx, const QueryTuple &inputTuple, const Field &toWrite) {
    assert(!dstTable->isEncrypted()); // this instance only supports plaintext case

    QueryTuple *dstTuple = dstTable->getTuplePtr(writeIdx);

    // if it is a dummy
    if(TypeUtilities::getBool(toWrite)) {
        //std::cout << "Writing tuple " << writeIdx << " as " <<  inputTuple.toString(true) << std::endl;
        *dstTuple = inputTuple;
    }

}
