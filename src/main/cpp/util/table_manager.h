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

        PlainTable *getPlainTable(const string & table_name);
        SecureTable *getSecureTable(const string & table_name);


        // simply overwrite any pre-existing table
        // use this with caution
        // recommend insertTable below when possible
        // it appends to any pre-existing secret shares

        void putSecureTable(const string & table_name, QueryTable<Bit> *table);
        void putPlainTable(const string & table_name, QueryTable<bool> *table);

        QuerySchema getSchema(const string & table_name) {
            return schemas_[table_name];
        }

        void insertTable(const string & table_name, SecureTable *src) {

            if(secure_tables_.find(table_name) == secure_tables_.end() && schemas_.find(table_name) == schemas_.end()) {
                putSecureTable(table_name, src);
                return;
            }

            if(src->tuple_cnt_ == 0) return;

            SecureTable *dst = getSecureTable(table_name);
            // confirm we are inserting a table that matches the expected schema
            assert(dst->getSchema() == src->getSchema());

            if(dst->empty()) {
                putSecureTable(table_name, src);
            }

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
                putPlainTable(table_name, src);
                return;
            }

            PlainTable *dst = getPlainTable(table_name);
            assert(dst->getSchema() == src->getSchema());
            if(dst->empty()) {
                putPlainTable(table_name, src);
            }
            auto dst_tuple_cnt = dst->tuple_cnt_;
            int new_table_len = dst_tuple_cnt + src->tuple_cnt_;
            dst->resize(new_table_len);

            // append to the existing cols
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                dst->cloneColumn(i, dst_tuple_cnt, src, i);
            }
            dst->cloneColumn(-1, dst_tuple_cnt, src, -1);
        }

        void deleteTable(const string & table_name) {
            auto p = getPlainTable(table_name);
            if(p != nullptr) delete p;

            auto s = getSecureTable(table_name);
            if(s != nullptr) delete s;
        }
    private:
        // this makes it a singleton
        TableManager() {}


    };
}

#endif
