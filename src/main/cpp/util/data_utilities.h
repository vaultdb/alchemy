#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H

// For EMP memory instrumentation
#if defined(__linux__)
#include <sys/time.h>
#include <sys/resource.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>
#endif


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

        static vector<int8_t> boolsToBytes(const bool *const src, const uint32_t &bitCount);
        static vector<int8_t> boolsToBytes( string & src); // for the output of Integer::reveal<string>()

        static std::unique_ptr<QueryTable>
        getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                          const bool &hasDummyTag);

        static std::shared_ptr<QueryTable>
        getQueryResults(const string &dbName, const string &sql, const bool &hasDummyTag);

        static std::shared_ptr<QueryTable> getExpectedResults(const string &dbName, const string &sql, const bool &hasDummyTag, const int & sortColCount);


        static std::string
        queryDatetime(const std::string &colName); // transform a column into an int64 for our expected output

        // filenames have full path, otherwise in standard testing framework filenames are relative to src/main/cpp/bin
        static void locallySecretShareTable(const std::unique_ptr<QueryTable> &table, const std::string &aliceFile,
                                            const std::string &bobFile);

        static void writeFile(std::string fileName, vector<int8_t> contents);

        // sort all columns one after another
        // default setting for many tests
        static SortDefinition getDefaultSortDefinition(const uint32_t &colCount);

        // create a copy of the table without its dummy tuples
        static std::shared_ptr<QueryTable> removeDummies(const std::shared_ptr<QueryTable> & input);

        static std::string getCurrentWorkingDirectory();
        static std::string printSortDefinition(const SortDefinition  & sortDefinition);

        static vector<int8_t> readFile(const string &fileName);

        static string printFirstBytes(vector<int8_t> &bytes, const int &byteCount);

        static string revealAndPrintFirstBytes(vector<Bit> &bits, const int &byteCount);

        static void checkMemoryUtilization(string msg);

        static void checkMemoryUtilization();

    };
}

#endif // DATA_UTILITIES_H
