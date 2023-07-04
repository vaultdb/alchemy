#ifndef CSV_READER_H
#define CSV_READER_H

#include "query_table/query_table.h"

namespace  vaultdb {
    class CsvReader {
    public:
        static PlainTable *readCsv(const std::string &filename, const QuerySchema &schema);
        static PlainTable *readCsvFromBatch(const vector<string> &payload, const QuerySchema &schema);

        static void
        parseTuple(const std::string &csv_line, const QuerySchema &src_schema, PlainTable *dst,
                   const int &idx);

        static vector<string> split(const string &tuple_entry);
    private:

        static QuerySchema convertDatesToLong(const QuerySchema & input_schema);

    };
}

#endif //CSV_DATA_PROVIDER_H
