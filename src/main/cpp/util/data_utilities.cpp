#include <cstddef>
#include <assert.h>
#include <data/PsqlDataProvider.h>
#include "data_utilities.h"

using namespace vaultdb;

unsigned char DataUtilities::reverse(unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}


signed char DataUtilities::boolsToByte(const bool *src) {
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


void DataUtilities::locallySecretShareTable(const std::unique_ptr<QueryTable> &table, const string &aliceFile,
                                           const string &bobFile) {
    std::pair<int8_t *, int8_t *> shares = table->generateSecretShares();
    writeFile(aliceFile, (char *) shares.first);
    writeFile(bobFile, (char *) shares.second);


}


void DataUtilities::writeFile(std::string fileName, const char *contents) {
    std::ofstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);
    if(!outFile.is_open()) {
        throw std::invalid_argument("Could not write output file " + fileName);
    }
    outFile.write(contents, 16);
    outFile.close();
}

SortDefinition DataUtilities::getDefaultSortDefinition(const uint32_t &colCount) {
        SortDefinition  sortDefinition;

        for(uint32_t i = 0; i < colCount; ++i) {
            sortDefinition.push_back(ColumnSort(i, SortDirection::ASCENDING));
        }

        return sortDefinition;

    }

std::shared_ptr<QueryTable> DataUtilities::removeDummies(const std::shared_ptr<QueryTable> &input) {
    // only works for plaintext tables
    assert(!input->isEncrypted());
    int outputTupleCount = input->getTrueTupleCount();

    int writeCursor = 0;
    std::shared_ptr<QueryTable> output(new QueryTable(outputTupleCount, input->getTupleCount(), false));
    output->setSchema(input->getSchema());
    output->setSortOrder(input->getSortOrder());

    for(int i = 0; i < input->getTupleCount(); ++i) {
        QueryTuple *tuple = input->getTuplePtr(i);
        if(!tuple->getDummyTag().getBool()) {
            output->putTuple(writeCursor, *tuple);
            ++writeCursor;
        }
    }

    return output;
}


