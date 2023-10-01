#ifndef _TABLE_UTILITIES_H_
#define _TABLE_UTILITIES_H_

#include "query_table/query_table.h"
#include "query_table/row_table.h"
#include "query_table/column_table.h"

namespace vaultdb {
template<typename B>
    class TableFactory {

    public:
        static QueryTable<B> *deserialize(const QuerySchema &schema, const std::vector<int8_t> &table_bytes, const StorageModel & model) {
            if(model == StorageModel::ROW_STORE) {
                return RowTable<B>::deserialize(schema, table_bytes);
            }
            else if(model == StorageModel::COLUMN_STORE) {
                return ColumnTable<B>::deserialize(schema, table_bytes);
            }
            throw;

        }


        static QueryTable<B> * getTable(const size_t &tuple_cnt, const QuerySchema &schema, const StorageModel &storage_model,
                                        const SortDefinition &sort_def = SortDefinition()) {

            if (storage_model == StorageModel::ROW_STORE) {
                return new RowTable<B>(tuple_cnt, schema, sort_def);
            }
            else if (storage_model == StorageModel::COLUMN_STORE) {
                return new ColumnTable<B>(tuple_cnt, schema, sort_def);
            }
            throw;

        }

    };

}
#endif
