//
// Created by Jennie Rogers on 8/5/20.
//

#include <cstddef>
#include <assert.h>
#include <data/PsqlDataProvider.h>
#include "data_utilities.h"

unsigned char DataUtilities::reverse(unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}


signed char DataUtilities::boolsToByte(bool *src) {
    signed char dst = 0;

    for(int i = 0; i < 8; ++i) {
        dst |= (src[i] << i);
    }

    return dst;
}

bool *DataUtilities::bytesToBool(int8_t *bytes, int byteCount) {
    bool *ret = new bool[byteCount * 8];


    bool *writePos = ret;

    for(int i = 0; i < byteCount; ++i) {
        uint8_t b = bytes[i];
        for(int j = 0; j < 8; ++j) {
            *writePos = ((b & (1<<j)) != 0);
            ++writePos;
        }
    }
    return ret;
}

int8_t *DataUtilities::boolsToBytes(const bool *const src, const uint32_t &bitCount) {
    int byteCount = bitCount / 8;
    assert(bitCount % 8 == 0); // no partial bytes supported

    int8_t *result = new int8_t[byteCount];

    bool *cursor = const_cast<bool*>(src);

    for(int i = 0; i < byteCount; ++i) {
        result[i] = boolsToByte(cursor);
        cursor += 8;
    }

    return result;

}



// in some cases, like with LIMIT, we can't just run over tpch_unioned
std::unique_ptr<QueryTable>
DataUtilities::getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                                 const bool &hasDummyTag) {

    PsqlDataProvider dataProvider;

    std::unique_ptr<QueryTable> alice = dataProvider.getQueryTable(aliceDb, sql, hasDummyTag); // dummyTag true not yet implemented
    std::unique_ptr<QueryTable> bob = dataProvider.getQueryTable(bobDb, sql, hasDummyTag);


    uint32_t tupleCount = alice->getTupleCount() + bob->getTupleCount();

    std::unique_ptr<QueryTable> unioned(new QueryTable(tupleCount, alice->getSchema().getFieldCount(), false));
    unioned->setSchema(alice->getSchema());

    for(int i = 0; i < alice->getTupleCount(); ++i) {
        unioned->putTuple(i, alice->getTuple(i));
    }

    int offset = alice->getTupleCount();

    // add bob's tuples from last to first
    int readIdx = bob->getTupleCount();
    for(int i = 0; i < bob->getTupleCount(); ++i) {
        --readIdx;
        unioned->putTuple(i + offset, bob->getTuple(readIdx));
    }

    QuerySchema schema = alice->getSchema();
    unioned->setSchema(schema);


    return unioned;
}


 std::shared_ptr<QueryTable> DataUtilities::getQueryResults(const std::string & dbName, const std::string & sql, const bool & hasDummyTag) {
    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable(dbName, sql, hasDummyTag);
}

std::string DataUtilities::queryDatetime(const string &colName) {
    return "CAST(EXTRACT(epoch FROM " + colName + ") AS BIGINT) " + colName; // last colName for aliasing
}


