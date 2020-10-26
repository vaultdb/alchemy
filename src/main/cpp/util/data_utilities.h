//
// Created by Jennie Rogers on 8/5/20.
//

#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H


#include <cstdint>
#include <vaultdb.h>

class DataUtilities {

    // convert 8 bits to a byte
    static int8_t boolsToByte(const bool *src);

    // reverse the order of bits in a byte
    static unsigned char reverse(unsigned char b);


public:
    static bool *bytesToBool(int8_t* bytes, int byteCount);
    static int8_t *boolsToBytes(const bool *src, const uint32_t &bitCount);



    static std::unique_ptr<QueryTable>
    getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                      const bool &hasDummyTag);

    static std::shared_ptr<QueryTable> getQueryResults(const string &dbName, const string &sql, const bool &hasDummyTag);
    static std::string queryDatetime(const std::string & colName); // transform a column into an int64 for our expected output
};


#endif // DATA_UTILITIES_H
