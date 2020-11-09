#include "secure_replace_tuple.h"

SecureReplaceTuple::SecureReplaceTuple(std::shared_ptr<QueryTable> table) : ReplaceTuple(table) {}

void SecureReplaceTuple::conditionalWrite(const uint32_t &writeIdx, const QueryTuple &srcTuple,
                                          const types::Value &toWrite) {

    assert(dstTable->isEncrypted());

    QueryTuple srcTupleCopy(srcTuple);
    QueryTuple *dstTuple = dstTable->getTuplePtr(writeIdx);
    emp::Bit writeCondition = toWrite.getEmpBit();

    for(int i = 0; i < dstTuple->getFieldCount(); ++i) {
        types::Value originalValue = dstTuple->getField(i).getValue();
        types::Value newValue = srcTuple.getField(i).getValue();

        types::Value output = types::Value::obliviousIf(writeCondition, newValue, originalValue);
        dstTuple->putField(QueryField(i, output));
    }
    types::Value originalDummyTag = dstTuple->getDummyTag();
    types::Value newDummyTag = srcTuple.getDummyTag();

    types::Value outputDummyTag = types::Value::obliviousIf(writeCondition, newDummyTag, originalDummyTag);
    dstTuple->setDummyTag(outputDummyTag);

}
