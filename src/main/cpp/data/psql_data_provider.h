#ifndef PSQL_DATA_PROVIDER_H
#define PSQL_DATA_PROVIDER_H

#include "query_table/query_table.h"
#include "query_table/query_tuple.h"

#include <pqxx/pqxx>
#include <vector>
#include <string>


using namespace pqxx;
using namespace vaultdb;



class  PsqlDataProvider  {
public:

    PlainTable *getQueryTable(std::string db_name, std::string sql, bool has_dummy_tag = false) {
        return getQueryTable(db_name, sql, StorageModel::COLUMN_STORE, has_dummy_tag);
    }

    PlainTable *getQueryTable(std::string db_name, std::string sql, const StorageModel &model, bool has_dummy_tag = false);
    void runQuery(std::string db_name, std::string sql);

    static vector<ForeignKeyConstraint> getForeignKeys(const string & db_name);
    void createTable(const string & db_name, const string & table_name, const QuerySchema & schema);
    // // retrieve primary key name and type
    // static  vector<string> getPrimaryKeys(const string & db_name, const string & table_name)  {
    //     string query = "SELECT c.column_name::CHAR(20), c.data_type "
    //     "FROM information_schema.table_constraints tc "
    //     " JOIN information_schema.constraint_column_usage AS ccu USING (constraint_schema, constraint_name) "
    //     " JOIN information_schema.columns AS c ON c.table_schema = tc.constraint_schema "
    //      " AND tc.table_name = c.table_name AND ccu.column_name = c.column_name "
    //     " WHERE constraint_type = 'PRIMARY KEY' and tc.table_name = '" + table_name + "'";
    //
    //     PsqlDataProvider provider;
    //
    //     provider.db_name_ = db_name;
    //     provider.storage_model_ = SystemConfiguration::getInstance().storageModel();
    //
    //     pqxx::result res;
    //     pqxx::connection conn("user=vaultdb dbname=" + db_name);
    //
    //     try {
    //         pqxx::work txn(conn);
    //         res = txn.exec(query);
    //         txn.commit();
    //
    //
    //     } catch (const std::exception &e) {
    //         std::cerr << e.what() << std::endl;
    //
    //         throw e;
    //     }
    //
    //
    //     pqxx::row first_row = *(res.begin());
    //     size_t row_cnt = res.size();
    //
    //
    //     // auto schema = QuerySchema("name:varchar(20), data_type:varchar(2))";
    //     // QueryTable<bool> *dst_table = QueryTable<bool>::getTable(row_cnt, schema);
    // // TODO: unwind this with getTuple...
    //
    //     vector<string> primary_keys;
    //     // for (int i = 0; i < row_cnt; ++i) {
    //     //     PlainField col_name = res->getField(i, 0);
    //     //     primary_keys.push_back(col_name.toString());
    //     // }
    //
    //     return primary_keys;
    // }




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
