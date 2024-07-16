#ifndef _STORED_DB_MANAGER_
#define _STORED_DB_MANAGER_

#include <string>
#include "query_table/query_schema.h"
#include "utilities.h"
#include "system_configuration.h"
#include "data_utilities.h"

using namespace std;

namespace vaultdb {



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


    private:
        StoredDbManager() {}


    };
}

#endif
