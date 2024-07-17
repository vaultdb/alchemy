#include "stored_table_scan.h"
#include "util/system_configuration.h"
#include "query_table/packed_column_table.h"

using namespace vaultdb;

template<typename B>
QueryTable<B> *StoredTableScan<B>::readStoredTable(string table_name, const vector<int> & col_ordinals, const int & limit) {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    assert(conf.table_metadata_.find(table_name) != conf.table_metadata_.end());
    TableMetadata md = conf.table_metadata_.at(table_name);

        if(conf.storageModel() == StorageModel::PACKED_COLUMN_STORE) {
            return (QueryTable<B> *) PackedColumnTable::deserialize(md, col_ordinals, limit);
        }
        else {
            return QueryTable<B>::deserialize(md, col_ordinals, limit);
        }


}

template class vaultdb::StoredTableScan<bool>;
template class vaultdb::StoredTableScan<emp::Bit>;
