#ifndef PSQL_DATA_PROVIDER_H
#define PSQL_DATA_PROVIDER_H

#include "query_table/query_table.h"
#include "query_table/query_tuple.h"

#include <pqxx/pqxx>

using namespace pqxx;
using namespace vaultdb;



class  PsqlDataProvider  {
public:

    PlainTable *getQueryTable(std::string db_name, std::string sql, bool has_dummy_tag = false);

private:
    void getTuple(pqxx::row row, bool has_dummy_tag, PlainTable &dst_table, const size_t &idx);
     PlainField getField(pqxx::field src);
    QuerySchema getSchema(pqxx::result input, bool has_dummy_tag);

     std::string src_table_;
     std::string db_name_;
     QuerySchema schema_;
     StorageModel storage_model_;

    size_t getVarCharLength(string table_name, string col_name) const;

    string getTableName(int oid);
    pqxx::result query(const std::string &  db_name, const std::string  & sql) const;
};


#endif //PSQL_DATA_PROVIDER_H
