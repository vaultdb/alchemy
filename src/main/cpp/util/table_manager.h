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

        ~TableManager() {
            for(auto & table : secure_tables_) {
                delete table.second;
            }
            for(auto & table : plain_tables_) {
                delete table.second;
            }
        }

        template<typename B>
        void addEmptyTable(const string & table_name, const QuerySchema & schema) {
            if(std::is_same_v<B, bool>) {
                auto s = QuerySchema::toPlain(schema);
                plain_tables_[table_name] = QueryTable<bool>::getTable(0, s);
            }
            else {
                auto s = QuerySchema::toSecure(schema);
                secure_tables_[table_name] = QueryTable<Bit>::getTable(0, s);
            }
        }

        template<typename B>
        QueryTable<B> *getTable(const string & table_name);

        // simply overwrite any pre-existing table
        // use this with caution
        // recommend insertTable below when possible
        // it appends to any pre-existing secret shares
        template<typename B>
        void putTable(const string & table_name, QueryTable<B> *table);


        QuerySchema getSchema(const string & table_name) {
            if(plain_tables_.find(table_name) != plain_tables_.end()) {
                return plain_tables_[table_name]->getSchema();
            }
            else {
                return secure_tables_[table_name]->getSchema();
            }

            throw std::runtime_error("Table not found!");
        }

        void insertTable(const string & table_name, SecureTable *src) {
            if(secure_tables_.find(table_name) == secure_tables_.end()) {
                secure_tables_[table_name] = src;
                return;
            }

            auto dst = secure_tables_[table_name];
            auto dst_tuple_cnt = dst->tuple_cnt_;
            int new_table_len = dst_tuple_cnt + src->tuple_cnt_;
            dst->resize(new_table_len);
            assert(dst->getSchema() == src->getSchema());

            // append to the existing cols
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                dst->cloneColumn(i, dst_tuple_cnt, src, i);
            }
            dst->cloneColumn(-1, dst_tuple_cnt, src, -1);

        }

        void insertTable(const string & table_name, PlainTable *src) {
            if(plain_tables_.find(table_name) == plain_tables_.end()) {
                plain_tables_[table_name] = src;
                return;
            }

            auto dst = plain_tables_[table_name];
            auto dst_tuple_cnt = dst->tuple_cnt_;
            int new_table_len = dst_tuple_cnt + src->tuple_cnt_;
            dst->resize(new_table_len);
            assert(dst->getSchema() == src->getSchema());

            // append to the existing cols
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                dst->cloneColumn(i, dst_tuple_cnt, src, i);
            }
            dst->cloneColumn(-1, dst_tuple_cnt, src, -1);
        }

    private:
        TableManager() {}


    };
}

#endif