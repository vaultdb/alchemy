#include "replace_tuple.h"

ReplaceTuple::ReplaceTuple(std::shared_ptr<QueryTable> table) {
    dstTable = table;

}

void
ReplaceTuple::conditionalWrite(const uint32_t &writeIdx, const QueryTuple &inputTuple, const types::Value &toWrite) {
    assert(!dstTable->isEncrypted()); // this instance only supports plaintext case

    QueryTuple *dstTuple = dstTable->getTuplePtr(writeIdx);

    // if it is a dummy
    if(toWrite.getBool()) {
        //std::cout << "Writing tuple " << writeIdx << " as " <<  inputTuple.toString(true) << std::endl;
        *dstTuple = inputTuple;
    }

}
