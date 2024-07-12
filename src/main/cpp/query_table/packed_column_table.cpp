#include "packed_column_table.h"
#include "computing_party_packed_column_table.h"
#include "input_party_packed_column_table.h"

using namespace vaultdb;


PackedColumnTable *PackedColumnTable::deserialize(const QuerySchema & schema, const int & tuple_cnt, const SortDefinition & collation, vector<int8_t> &packed_wires) {
    if(SystemConfiguration::getInstance().party_ == SystemConfiguration::getInstance().input_party_) {
        return new InputPartyPackedColumnTable(tuple_cnt, schema, collation);
    }

    ComputingPartyPackedColumnTable *table = new ComputingPartyPackedColumnTable(tuple_cnt, schema, collation);
    int8_t *read_cursor = packed_wires.data();
    for(int i = -1; i < schema.getFieldCount(); ++i) {
        int8_t *dst = table->packed_pages_[i].data();
        int write_size_bytes =  table->packed_pages_[i].size();
        memcpy(dst, read_cursor, write_size_bytes);
        read_cursor += write_size_bytes;

    }

    return table;
}