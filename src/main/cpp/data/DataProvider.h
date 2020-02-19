//
// Created by madhav on 12/30/19.
//

#ifndef TESTING_DATAPROVIDER_H
#define TESTING_DATAPROVIDER_H

#include "dbquery.pb.h"
#include <querytable/query_table.h>

class DataProvider {
public:
  std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                            std::string query_string);
};

#endif // TESTING_DATAPROVIDER_H
