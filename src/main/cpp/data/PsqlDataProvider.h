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



class PsqlDataProvider :  DataProvider {
public:
    std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                              std::string query_string);

    std::unique_ptr<QueryTable> GetQueryTable(std::string dbname,
                                              std::string query_string, bool hasDummyTag);

private:
    pqxx::result readQuery(string query, string dbname);
    void getTuple(pqxx::row row, QueryTuple *dstTuple);
    std::unique_ptr<QueryField>  getField(pqxx::field src);
    std::unique_ptr<QuerySchema> getSchema(pqxx::result result);


};


#endif //VAULTDB_EMP_PSQLDATAPROVIDER_H
