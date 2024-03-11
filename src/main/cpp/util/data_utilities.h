#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H

// For memory instrumentation
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
#include "util/system_configuration.h"

namespace vaultdb {

    template<typename B> class QueryTable;
    typedef QueryTable<bool> PlainTable;
    typedef QueryTable<Bit> SecureTable;

    class DataUtilities {




    public:

        static PlainTable *
        getUnionedResults(const std::string &alice_db, const std::string &bob_db, const std::string &sql,
                          const bool &has_dummy_tag, const size_t &limit = 0);

        static PlainTable *getQueryResults(const std::string &db_name, const std::string &sql,  const bool &has_dummy_tag = false);

        static PlainTable *getExpectedResults(const string &db_name, const string &sql, const bool &hasDummyTag,
                                              const int &sortColCount);

        static void runQueryNoOutput(const std::string &db_name, const std::string &sql);

        static std::string queryDatetime(const std::string &colName); // transform a column into an int64 for our expected output

        // filenames have full path, otherwise in standard testing framework filenames are relative to src/main/cpp/bin
        static void locallySecretShareTable(const std::unique_ptr<PlainTable> &table, const std::string &aliceFile,
                                            const std::string &bobFile);

        static void writeFile(const string &fileName, vector<int8_t> contents);

        static void writeFile(const string  &filename,const string & contents);
        // sort all columns one after another
        // default setting for many tests
        static SortDefinition getDefaultSortDefinition(const uint32_t &colCount);

        // create a copy of the table without its dummy tuples
        static void removeDummies( PlainTable *input);

        static std::string printSortDefinition(const SortDefinition  & sortDefinition);
        static std::string printSortDirection(const SortDirection & sort_dir){
            return (sort_dir == SortDirection::ASCENDING) ? "ASC" : (sort_dir == SortDirection::DESCENDING) ? "DESC" : "INVALID";
        }

        static vector<int8_t> readFile(const string &fileName);
        // if the file were divided into batches of length *batch_tuple_cnt*, read in the *batch_no* partition
        // zero_-indexed
        static vector<string> readTextFileBatch(const string &filename, const size_t & batch_tuple_cnt, const size_t & batch_no);

        static string printFirstBytes(vector<int8_t> &bytes, const int &byteCount);

        static string revealAndPrintFirstBytes(vector<Bit> &bits, const int &byteCount);

        static size_t getTupleCount(const string &db_name, const string &sql, bool has_dummy_tag);
        static string printByteArray(const int8_t *bytes, const size_t & byte_cnt);
        static string printBitArray(const int8_t *bits, const size_t & byte_cnt);
//        static emp::Integer toEmpInteger(const vector<int8_t> & src_bytes);
        static string printBitArray(const bool* bits, const size_t & bit_cnt) {
            std::stringstream ss;
            for(size_t i = 0; i < bit_cnt; ++i) {
                if(i % 8 == 0) ss << " ";
                ss << ((bits[i]) ? "1" : "0");
            }
            return ss.str();
        }

       static vector<string> readTextFile(const string & filename);
       static string readTextFileToString(const string & filename);


        static bool isOrdinal(const string &s);

        static bool verifyCollation(PlainTable *sorted);
        static bool verifyCollation(SecureTable *sorted);

        // reverse the order of bits in a byte
        static inline unsigned char reverse(unsigned char b) {
            b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
            b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
            b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
            return b;
        }

        static inline void reverseBitstring(int8_t *src_bytes, int8_t *dst_bytes, const size_t & byte_cnt) {
            int8_t *dst_cursor = dst_bytes + byte_cnt - 1;

            for(size_t i = 0; i < byte_cnt; ++i) {
                *dst_cursor = reverse(src_bytes[i]);
                --dst_cursor;
            }
        }

    static inline void reverseBytes(Bit *src, Bit *dst, const int & byte_cnt) {
            Bit *src_ptr = src;
            Bit *dst_ptr = dst + (byte_cnt - 1)*8;
            for(int i = 0; i < byte_cnt; ++i) {
                memcpy(dst_ptr, src_ptr, 8 * sizeof(emp::Bit));
                src_ptr += 8;
                dst_ptr -= 8;
            }
    }

    static string printTable(SecureTable *table, int tuple_limit = -1, bool show_dummies = false);
    static string printTable(const PlainTable *table, int tuple_limit = -1, bool show_dummies = false);

    };
}

#endif // DATA_UTILITIES_H
