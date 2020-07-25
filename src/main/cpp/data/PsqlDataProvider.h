//
// Created by Jennie Rogers on 7/18/20.
//

#ifndef PSQL_DATA_PROVIDER_H
#define PSQL_DATA_PROVIDER_H

#include "DataProvider.h"
#include "query_table.h"
#include "query_tuple.h"
#include "query_field.h"

#include <pqxx/pqxx>

using namespace pqxx;
using namespace vaultdb;



class PsqlDataProvider  { // :  DataProvider
public:
    std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                              std::string query_string, std::string tableName);

    std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                              std::string query_string, std::string tableName, bool hasDummyTag);

private:
    void getTuple(pqxx::row row, QueryTuple *dstTuple);
    std::unique_ptr<QueryField>  getField(pqxx::field src);
    std::unique_ptr<QuerySchema> getSchema(pqxx::result result);

     std::string srcTable;
     std::string dbName;

    size_t getVarCharLength(string table, string column) const;
};


#endif //VAULTDB_EMP_PSQLDATAPROVIDER_H
