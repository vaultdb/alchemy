#include "table_manager.h"

using namespace vaultdb;

PlainTable *TableManager::getPlainTable(const string &table_name) {
    if(plain_tables_.find(table_name) != plain_tables_.end()) return plain_tables_[table_name];
    if(schemas_.find(table_name) != schemas_.end()) return QueryTable<bool>::getTable(0, schemas_[table_name]);
    return nullptr;
}

SecureTable *TableManager::getSecureTable(const string &table_name) {
    if(secure_tables_.find(table_name) != secure_tables_.end()) return secure_tables_[table_name];
    if(schemas_.find(table_name) != schemas_.end())  return QueryTable<Bit>::getTable(0, QuerySchema::toSecure(schemas_[table_name]));
    return nullptr;
}


void TableManager::putPlainTable(const string &table_name, QueryTable<bool> *table) {
    if(plain_tables_.find(table_name) != plain_tables_.end()) {
        delete plain_tables_[table_name];
    }
    if(schemas_.find(table_name) == schemas_.end()) {
        schemas_[table_name] = table->getSchema();
    }
    else if(schemas_[table_name] != table->getSchema()) {
        throw std::runtime_error("TableManager::putTable: schema mismatch");
    }

    plain_tables_[table_name] = table->clone();
}


void TableManager::putSecureTable(const string &table_name, QueryTable<Bit> *table) {
    if(secure_tables_.find(table_name) != secure_tables_.end()) {
        delete secure_tables_[table_name];
    }
    if(schemas_.find(table_name) == schemas_.end()) {
        schemas_[table_name] = QuerySchema::toPlain(table->getSchema());
    }
    else if(schemas_[table_name] != QuerySchema::toPlain(table->getSchema())) {
        throw std::runtime_error("TableManager::putTable: schema mismatch");
    }

    secure_tables_[table_name] = table->clone();

}