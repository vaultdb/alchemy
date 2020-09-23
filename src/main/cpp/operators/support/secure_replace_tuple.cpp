//
// Created by Jennie Rogers on 9/21/20.
//

#include "secure_replace_tuple.h"

SecureReplaceTuple::SecureReplaceTuple(std::shared_ptr<QueryTable> table) : ReplaceTuple(table) {}

void SecureReplaceTuple::conditionalWrite(const uint32_t &writeIdx, const QueryTuple &srcTuple,
                                          const types::Value &toWrite) {

    assert(dstTable->isEncrypted());
    QueryTuple *dstTuple = dstTable->getTuplePtr(writeIdx);
    assert(srcTuple.getFieldCount() == dstTuple->getFieldCount());
    emp::Bit writeCondition = toWrite.getEmpBit();

    for(uint32_t i = 0; i < dstTuple->getFieldCount();  ++i) {
        types::Value dstVal = dstTuple->getField(i).getValue();
        types::Value srcVal = srcTuple.getField(i).getValue();
        // only write if it is a match
        dstVal = types::Value::obliviousIf(writeCondition, srcVal, dstVal);
        QueryField dstField(i, dstVal);
        dstTuple->putField(dstField);
    }



}
