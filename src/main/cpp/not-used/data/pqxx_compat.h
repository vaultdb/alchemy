#ifndef _PQXX_COMPAT_H
#define _PQXX_COMPAT_H

#include "dbquery.pb.h"
#include <pqxx/pqxx>
#include <string>

const dbquery::Schema GetSchemaFromQuery(pqxx::result res);
pqxx::result query(std::string dbname, std::string query_string);
dbquery::Table GetPqTable(std::string dbname, std::string query_string);
#endif // _PQXX_COMPAT_H
