#ifndef _STORED_DB_MANAGER_
#define _STORED_DB_MANAGER_

#include <string>
#include "query_table/query_schema.h"
#include "utilities.h"
#include "system_configuration.h"
#include "data_utilities.h"

using namespace std;

namespace vaultdb {

    typedef struct table_metadata_ {
        string name_;
        QuerySchema schema_;
        SortDefinition collation_;
        size_t tuple_cnt_;
    } TableMetadata;


    // handles I/O interface with tables
    // only supports PackedColumnTable for now.
    // encode DB  with a variant of `scripts/secret_share_tpch_instance.sh` or `scripts/wire_pack_tpch_instance.sh`
    // this basically just maintains metadata for StoredTableScan to parse.

    // TODO: add ColumnTable for baseline
    class StoredDbManager {
    public:

        string db_path_;
        // for lazy initialization of tables
        map<string, TableMetadata> table_metadata_;

        // for singleton
        static StoredDbManager & getInstance() {
            static StoredDbManager  instance;
            return instance;
        }

        void initializeWirePacking(const string & db_path) {
            db_path_ = db_path;


            SystemConfiguration & config = SystemConfiguration::getInstance();
            assert(config.storageModel() == StorageModel::PACKED_COLUMN_STORE);

            block delta;
            // if input party, initialize delta first from file
            if(config.inputParty()) {
                auto d = DataUtilities::readFile(db_path_ + "/delta");
                assert(d.size() == sizeof(block));
                memcpy((int8_t *) &delta, d.data(), sizeof(block));
            }

            config.emp_manager_->setDelta(delta);

            string all_tables = Utilities::runCommand("ls *.metadata", db_path_);
            vector<string> tables = Utilities::splitStringByNewline(all_tables);

            for(auto & metadata_file : tables) {
                TableMetadata md;
                md.name_ = metadata_file.substr(0, metadata_file.size() - 9);
                auto metadata = DataUtilities::readTextFile(db_path_ + "/" + metadata_file);
                // drop ".metadata" suffix
                md.schema_ = QuerySchema(metadata.at(0));
                md.collation_ = DataUtilities::parseCollation(metadata.at(1));
                md.tuple_cnt_ = atoi(metadata.at(2).c_str());
                table_metadata_[md.name_] = md;
            }


        }

    private:
        StoredDbManager() {}


    };
}

#endif
