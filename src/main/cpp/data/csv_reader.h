#include <vaultdb.h>

#ifndef CSV_READER_H
#define CSV_READER_H


namespace  vaultdb {
    class CsvReader {
    public:
        static std::unique_ptr<PlainTable > readCsv(const std::string &filename, const QuerySchema &schema);

        static void
        parseTuple(const std::string &csvLine, const QuerySchema &src_schema, std::unique_ptr<PlainTable> &dst,
                   const size_t &tupleIdx);

    private:

        static vector<string> split(const string &tupleEntry);
    };
}

#endif //CSV_DATA_PROVIDER_H
