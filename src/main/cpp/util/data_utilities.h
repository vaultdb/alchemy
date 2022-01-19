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
#include "utilities.h"
#include <query_table/query_table.h>

namespace vaultdb {
    class DataUtilities {


        // reverse the order of bits in a byte
        static unsigned char reverse(unsigned char b);


    public:

        static std::shared_ptr<PlainTable>
        getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                          const bool &hasDummyTag, const size_t & limit = 0);

        static std::shared_ptr<PlainTable >
        getQueryResults(const string &dbName, const string &sql, const bool &hasDummyTag);

        static std::shared_ptr<PlainTable > getExpectedResults(const string &dbName, const string &sql, const bool &hasDummyTag, const int & sortColCount);


        static std::string
        queryDatetime(const std::string &colName); // transform a column into an int64 for our expected output

        // filenames have full path, otherwise in standard testing framework filenames are relative to src/main/cpp/bin
        static void locallySecretShareTable(const std::unique_ptr<PlainTable> &table, const std::string &aliceFile,
                                            const std::string &bobFile);

        static void writeFile(const string &fileName, vector<int8_t> contents);

        static void writeFile(const string  &filename,const string & contents);
        // sort all columns one after another
        // default setting for many tests
        static SortDefinition getDefaultSortDefinition(const uint32_t &colCount);

        // create a copy of the table without its dummy tuples
        static std::shared_ptr<PlainTable> removeDummies(const std::shared_ptr<PlainTable > & input);

        static std::string printSortDefinition(const SortDefinition  & sortDefinition);

        static vector<int8_t> readFile(const string &fileName);
        // if the file were divided into batches of length *batch_tuple_cnt*, read in the *batch_no* partition
        // zero-indexed
        static vector<string> readTextFileBatch(const string &filename, const size_t & batch_tuple_cnt, const size_t & batch_no);

        static string printFirstBytes(vector<int8_t> &bytes, const int &byteCount);

        static string revealAndPrintFirstBytes(vector<Bit> &bits, const int &byteCount);

        static size_t get_tuple_cnt(const std::string & db_name, const std::string &  sql, bool has_dummy_tag);

//        static emp::Integer toEmpInteger(const vector<int8_t> & src_bytes);

       static vector<string> readTextFile(const string & filename);
       static string readTextFileToString(const string & filename);


        static bool isOrdinal(const string &s);


    };
}

#endif // DATA_UTILITIES_H
