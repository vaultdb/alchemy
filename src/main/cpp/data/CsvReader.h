#include <vaultdb.h>

#ifndef CSV_READER_H
#define CSV_READER_H



class CsvReader   {
public:
    static std::unique_ptr<QueryTable> readCsv(const std::string & filename, const QuerySchema & schema);
    static QueryTuple parseTuple(const std::string & csvLine, const QuerySchema & schema);

private:
    static std::vector<std::string> readFile(const std::string & filename);
    static vector<string> split(const string &tupleEntry);
};


#endif //CSV_DATA_PROVIDER_H
