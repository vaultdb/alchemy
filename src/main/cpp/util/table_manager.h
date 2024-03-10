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
        QueryTable<B> *getTable(const string & table_name) {
            if(std::is_same_v<B, bool>) return plain_tables_[table_name];
            else return secure_tables_[table_name];
        }

        template<typename B> void putTable(const string & table_name, QueryTable<B> *table) {
            if(std::is_same_v<B, bool>) plain_tables_[table_name] = (PlainTable *) table;
            else secure_tables_[table_name] = (SecureTable *) table;
        }

    private:
        TableManager() {}


    };
}

#endif