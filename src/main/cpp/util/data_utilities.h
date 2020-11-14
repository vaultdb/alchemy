#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H


#include <cstdint>
#include <vaultdb.h>

namespace vaultdb {
    class DataUtilities {

        // convert 8 bits to a byte
        static int8_t boolsToByte(const bool *src);

        // reverse the order of bits in a byte
        static unsigned char reverse(unsigned char b);


    public:
        static bool *bytesToBool(int8_t *bytes, int byteCount);

        static int8_t *boolsToBytes(const bool *src, const uint32_t &bitCount);


        static std::unique_ptr<QueryTable>
        getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                          const bool &hasDummyTag);

        static std::shared_ptr<QueryTable>
        getQueryResults(const string &dbName, const string &sql, const bool &hasDummyTag);

        static std::string
        queryDatetime(const std::string &colName); // transform a column into an int64 for our expected output

        // filenames have full path, otherwise in standard testing framework filenames are relative to src/main/cpp/bin
        static void locallySecretShareTable(const std::unique_ptr<QueryTable> &table, const std::string &aliceFile,
                                            const std::string &bobFile);

        static void writeFile(std::string fileName, const char *contents);

        // sort all columns one after another
        // default setting for many tests
        static SortDefinition getDefaultSortDefinition(const uint32_t &colCount);

        // create a copy of the table without its dummy tuples
        static std::shared_ptr<QueryTable> removeDummies(const std::shared_ptr<QueryTable> & input);
    };
}

#endif // DATA_UTILITIES_H
