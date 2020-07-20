//
// Created by madhav on 12/31/19.
//

#ifndef PQ_DATA_PROVIDER_H
#define PQ_DATA_PROVIDER_H

#include "DataProvider.h"
#include "pqxx_compat.h"
#include "proto_converter.h"

class PQDataProvider : DataProvider {

public:
  std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                            std::string query_string);
    std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                              std::string query_string, bool hasDummmyFlag);

};

#endif // PQ_DATA_PROVIDER_H
