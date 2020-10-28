#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include <query_table/query_table.h>

class DataProvider {
public:
  std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                            std::string query_string);

    std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                              std::string query_string,
                                              bool hasDummyFlag);

};

#endif // DATA_PROVIDER_H
