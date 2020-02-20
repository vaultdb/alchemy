//
// Created by madhav on 12/31/19.
//

#include "PQDataProvider.h"

std::unique_ptr<QueryTable>
PQDataProvider::GetQueryTable(std::string dbname, std::string query_string) {

  auto proto_table = GetPqTable(dbname, query_string);
  auto t = ProtoToQuerytable(proto_table);
  return t;
}
