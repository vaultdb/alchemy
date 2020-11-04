#include <vaultdb.h>

#ifndef CSV_READER_H
#define CSV_READER_H



class CsvReader   {
public:
    static std::unique_ptr<QueryTable> readCsv(const std::string & filename, const QuerySchema & schema);
};


#endif //CSV_DATA_PROVIDER_H
