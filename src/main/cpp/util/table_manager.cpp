#include "table_manager.h"

using namespace vaultdb;

template<>
PlainTable *TableManager::getTable<bool>(const string &table_name) {
    if(plain_tables_.find(table_name) == plain_tables_.end()) {
        return nullptr;
    }
    return plain_tables_[table_name];
}

template<>
SecureTable *TableManager::getTable<Bit>(const string &table_name) {
    if(secure_tables_.find(table_name) == secure_tables_.end()) {
        return nullptr;
    }
    return secure_tables_[table_name];
}


template<>
void TableManager::putTable<bool>(const string &table_name, QueryTable<bool> *table) {
     plain_tables_[table_name] = table;
}

template<>
void TableManager::putTable<Bit>(const string &table_name, QueryTable<Bit> *table) {
    secure_tables_[table_name] = table;
}

