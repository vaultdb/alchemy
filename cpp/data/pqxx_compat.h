//
// Created by madhav on 12/27/19.
//

#ifndef TESTING_PQXX_COMPAT_H
#define TESTING_PQXX_COMPAT_H

#include "dbquery.pb.h"
#include <pqxx/pqxx>
#include <string>

const dbquery::Schema GetSchemaFromQuery(pqxx::result res);
pqxx::result query(std::string dbname, std::string query_string);
dbquery::Table GetPqTable(std::string dbname, std::string query_string);
#endif // TESTING_PQXX_COMPAT_H
