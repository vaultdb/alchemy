#ifndef _TABLE_MANAGER_
#define _TABLE_MANAGER_

#include <string>
#include <query_table/query_table.h>

// a simple container to store materialized tables that are globally accessible to query operators
// analogous to a system catalog in a traditional DBMS
// manage this as a singleton

namespace vaultdb {
    class TableManager {
    public:
        // for instance
        static TableManager& getInstance() {
            static TableManager  instance;
            return instance;
        }

        map<string, SecureTable *> secure_tables_;
        map<string, PlainTable *>  plain_tables_;
        // TableManager may accept two tables with the same name as long as they have equivalent schemas.
        // e.g., secure_tables_["foo"] has the schema of QuerySchema::toSecure(plain_tables_["foo"].schema_);
        // everything stored in schemas_ is plaintext for simplicity
        map<string, QuerySchema> schemas_;

        ~TableManager() {
            for(auto & table : secure_tables_) {
                delete table.second;
            }
            for(auto & table : plain_tables_) {
                delete table.second;
            }
        }

        void addEmptyTable(const string & table_name, const QuerySchema & schema) {
            schemas_[table_name] = QuerySchema::toPlain(schema);
        }

        template<typename B>
        QueryTable<B> *getTable(const string & table_name);

        template<>
        QueryTable<Bit> *getTable(const string & table_name);

        template<>
        QueryTable<bool> *getTable(const string & table_name);


        // simply overwrite any pre-existing table
        // use this with caution
        // recommend insertTable below when possible
        // it appends to any pre-existing secret shares
        template<typename B>
        void putTable(const string & table_name, QueryTable<B> *table);

        template<>
        void putTable<Bit>(const string & table_name, QueryTable<Bit> *table);

        template<>
        void putTable<bool>(const string & table_name, QueryTable<bool> *table);

        QuerySchema getSchema(const string & table_name) {
            return schemas_[table_name];
        }

        void insertTable(const string & table_name, SecureTable *src) {

            if(secure_tables_.find(table_name) == secure_tables_.end()
            && schemas_.find(table_name) == schemas_.end()) {
                putTable<Bit>(table_name, src);
                return;
            }

            SecureTable *dst = getTable<Bit>(table_name);
            cout << "Inserting " << table_name << " rows.\n";
            cout << "Src schema: " << src->getSchema() << ",\n dst: " << dst->getSchema() << endl;
            assert(dst->getSchema() == src->getSchema());
            auto dst_tuple_cnt = dst->tuple_cnt_;
            int new_table_len = dst_tuple_cnt + src->tuple_cnt_;
            dst->resize(new_table_len);

            // append to the existing cols
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                dst->cloneColumn(i, dst_tuple_cnt, src, i);
            }
            dst->cloneColumn(-1, dst_tuple_cnt, src, -1);

        }

        void insertTable(const string & table_name, PlainTable *src) {
            if(plain_tables_.find(table_name) == plain_tables_.end()
               && schemas_.find(table_name) == schemas_.end()) {
                putTable<bool>(table_name, src);
                return;
            }

            PlainTable *dst = nullptr;
            if(plain_tables_.find(table_name) != plain_tables_.end())  dst = plain_tables_[table_name];
            else if(schemas_.find(table_name) != schemas_.end()) dst =  QueryTable<bool>::getTable(0, schemas_[table_name]);

            assert(dst->getSchema() == src->getSchema());

            auto dst_tuple_cnt = dst->tuple_cnt_;
            int new_table_len = dst_tuple_cnt + src->tuple_cnt_;
            dst->resize(new_table_len);

            // append to the existing cols
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                dst->cloneColumn(i, dst_tuple_cnt, src, i);
            }
            dst->cloneColumn(-1, dst_tuple_cnt, src, -1);
        }

    private:
        // this makes it a singleton
        TableManager() {}


    };
}

#endif