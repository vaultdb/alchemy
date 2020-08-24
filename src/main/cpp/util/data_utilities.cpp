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

int8_t *DataUtilities::boolsToBytes(bool *src, int bitCount) {
    int byteCount = bitCount / 8;
    assert(bitCount % 8 == 0); // no partial bytes supported

    int8_t *result = new int8_t[byteCount];

    bool *cursor = src;

    for(int i = 0; i < byteCount; ++i) {
        result[i] = boolsToByte(cursor);
        cursor += 8;
    }

    return result;

}

std::unique_ptr<QueryTable>
DataUtilities::getExpectedResults(const std::string &sql, const std::string &orderBy, const int &party,
                                  const std::string &aliceDb,
                                  const std::string &bobDb) {

    PsqlDataProvider dataProvider;


    std::unique_ptr<QueryTable> alice = dataProvider.getQueryTable("tpch_alice", sql, false); // dummyTag true not yet implemented
    std::unique_ptr<QueryTable> bob = dataProvider.getQueryTable("tpch_bob", sql, false);

    std::string leadDb =  party == 1 ? aliceDb : bobDb;

    pqxx::connection c("dbname=" + leadDb);
    pqxx::work w(c);

    w.exec("DROP TABLE IF EXISTS tmp");

    w.exec("SELECT * INTO TABLE tmp FROM (" + sql + ") AS foo");


    QueryTable *otherQueryTable = (party == 1) ? bob.get() : alice.get();

    assert(otherQueryTable->getTupleCount() >= 1);  // will fail otherwise

    std::string colNames = "(" + otherQueryTable->getSchema().getField(0).getName();
    for(uint32_t i = 1; i < otherQueryTable->getSchema().getFieldCount(); ++i) {
        colNames += ", " + otherQueryTable->getSchema().getField(i).getName();
    }

    colNames += ")";

    std::string insertString = "INSERT INTO tmp " + colNames + " VALUES " + otherQueryTable->getTuple(0).toString();
    for(int i = 1; i < otherQueryTable->getTupleCount();  ++i) {
        insertString += ", " + otherQueryTable->getTuple(i).toString();
    }

    w.exec(insertString);
    w.commit();


    std::string sortStr = "SELECT * FROM tmp";

    if(!orderBy.empty()) {
        sortStr += " ORDER BY " + orderBy;
    }

    std::unique_ptr<QueryTable> unioned = dataProvider.getQueryTable(leadDb, sortStr, false);
    unioned->setSchema(alice->getSchema());


    return unioned;
    // TODO: clean up / drop tmp table in test tearDown, see SecureSortTest as an example

}

std::unique_ptr<QueryTable>
DataUtilities::getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql) {

    PsqlDataProvider dataProvider;

    std::unique_ptr<QueryTable> alice = dataProvider.getQueryTable("tpch_alice", sql, false); // dummyTag true not yet implemented
    std::unique_ptr<QueryTable> bob = dataProvider.getQueryTable("tpch_bob", sql, false);


    uint32_t tupleCount = alice->getTupleCount() + bob->getTupleCount();

    std::unique_ptr<QueryTable> unioned(new QueryTable(tupleCount, alice->getSchema().getFieldCount(), false));
    unioned->setSchema(alice->getSchema());

    for(int i = 0; i < alice->getTupleCount(); ++i) {
        unioned->putTuple(i, alice->getTuple(i));
    }

    int offset = alice->getTupleCount();

    for(int i = 0; i < bob->getTupleCount(); ++i) {
        unioned->putTuple(i + offset, bob->getTuple(i));
    }

    unioned->setSchema(alice->getSchema());
    return unioned;
}

