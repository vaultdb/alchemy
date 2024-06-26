#ifndef _TABLE_MANAGER_
#define _TABLE_MANAGER_

#include <string>
#include <query_table/query_table.h>
#include <query_table/packed_column_table.h>

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

            SecureTable *dst = getSecureTable(table_name);
            // confirm we are inserting a table that matches the expected schema
            assert(dst->getSchema() == src->getSchema());

            if(dst->empty()) {
                putSecureTable(table_name, src);
            }

            // if a table already exists, append to it
            auto dst_tuple_cnt = dst->tuple_cnt_;
            int new_table_len = dst_tuple_cnt + src->tuple_cnt_;
            dst->resize(new_table_len);

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

        // e.g., wires/tpch_unioned_150, 1
        void initializePackedWires(const string & path, const int & party) {
            SystemConfiguration & config = SystemConfiguration::getInstance();
            assert(config.storageModel() == StorageModel::PACKED_COLUMN_STORE);

            block delta;
            // if input party, initialize delta first from file
            if(party == SystemConfiguration::getInstance().input_party_) {
                cout << "Reading delta from " << path << "/delta" << endl;

                auto d = DataUtilities::readFile(path + "/delta");
                assert(d.size() == sizeof(block));

                memcpy((int8_t *) &delta, d.data(), sizeof(block));
            }

            config.emp_manager_->setDelta(delta);

            string all_tables = Utilities::runCommand("ls *.metadata", path);
            vector<string> tables = Utilities::splitStringByNewline(all_tables);

            for(auto & metadata_file : tables) {
                string table_name = metadata_file.substr(0, metadata_file.size() - 9);


                auto metadata = DataUtilities::readTextFile(path + "/" + metadata_file);
                // drop ".metadata" suffix

                auto schema = QuerySchema(metadata.at(0));
                SortDefinition  collation = DataUtilities::parseCollation(metadata.at(1));
                int tuple_cnt = atoi(metadata.at(2).c_str());

                cout << "Parsed table " << table_name << " with schema " << schema << " collation: " << DataUtilities::printSortDefinition(collation) << " tuple count: " << tuple_cnt << endl;
                SecureTable *table;
                if(party == SystemConfiguration::getInstance().input_party_) {
                    // if input party, no secret shares
                    table = new PackedColumnTable(tuple_cnt, schema, collation);
                }
                else {
                    vector<int8_t> packed_wires = DataUtilities::readFile(path + "/" + table_name + "." + std::to_string(party));
                    table = PackedColumnTable::deserialize(schema, tuple_cnt, collation, packed_wires);
                }
                putSecureTable(table_name, table);
            }

        }

    private:
        // this makes it a singleton
        TableManager() {}


    };
}

#endif