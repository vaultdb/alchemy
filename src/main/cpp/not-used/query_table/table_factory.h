#ifndef _TABLE_UTILITIES_H_
#define _TABLE_UTILITIES_H_

#include "query_table/query_table.h"
#include "query_table/row_table.h"
#include "query_table/column_table.h"

namespace vaultdb {
template<typename B>
    class TableFactory {

    public:
        static QueryTable<B> *deserialize(const QuerySchema &schema, const std::vector<int8_t> &table_bytes) {

            bool is_plaintext = std::is_same_v<B, bool>;
            assert(is_plaintext);
            // first byte is storage model

            const int8_t *read_ptr = table_bytes.data();
            StorageModel model = (StorageModel) *read_ptr;

            if(model == StorageModel::ROW_STORE) {
                return RowTable<B>::deserialize(schema, table_bytes);
            }
            else if(model == StorageModel::COLUMN_STORE) {
                return ColumnTable<B>::deserialize(schema, table_bytes);
            }
            throw;

        }

        static QueryTable<B> *deserialize(const QuerySchema &schema, const std::vector<int8_t> &table_bytes, const StorageModel & model) {
            bool is_plaintext = std::is_same_v<B, bool>;
            assert(is_plaintext);


            if(model == StorageModel::ROW_STORE) {
                return RowTable<B>::deserialize(schema, table_bytes);
            }
            else if(model == StorageModel::COLUMN_STORE) {
                return ColumnTable<B>::deserialize(schema, table_bytes);
            }
            throw;

        }





        static QueryTable<B> *getTable(const size_t &tuple_cnt, const QuerySchema &schema,
                                       const SortDefinition &sort_def = SortDefinition()) {

            if (SystemConfiguration::getInstance().storageModel() == StorageModel::ROW_STORE) {
                return new RowTable<B>(tuple_cnt, schema, sort_def);
            }
            else if (SystemConfiguration::getInstance().storageModel() == StorageModel::COLUMN_STORE) {
                return new ColumnTable<B>(tuple_cnt, schema, sort_def);
            }
            throw;

        }

    };

}
#endif
