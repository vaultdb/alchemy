#ifndef PSQL_DATA_PROVIDER_H
#define PSQL_DATA_PROVIDER_H

#include "DataProvider.h"
#include "query_table/query_table.h"
#include "query_table/query_tuple.h"
#include "query_table/query_field.h"

#include <pqxx/pqxx>

using namespace pqxx;
using namespace vaultdb;



class  PsqlDataProvider  { // :  DataProvider
public:
    std::unique_ptr<QueryTable> getQueryTable(std::string dbname, std::string query_string);

    std::unique_ptr<QueryTable> getQueryTable(std::string dbname, std::string query_string, bool hasDummyTag);

private:
    QueryTuple getTuple(pqxx::row row, bool hasDummyTag);
    QueryField getField(pqxx::field src);
    std::unique_ptr<QuerySchema> getSchema(pqxx::result input, bool hasDummyTag);

     std::string srcTable;
     std::string dbName;
    std::unique_ptr<QuerySchema> tableSchema;

    size_t getVarCharLength(string table, string column) const;

    string getTableName(int oid);
    pqxx::result query(const std::string &  dbname, const std::string  & query_string) const;
};


#endif //PSQL_DATA_PROVIDER_H
