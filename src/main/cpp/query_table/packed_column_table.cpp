#include "packed_column_table.h"
#include "computing_party_packed_column_table.h"
#include "input_party_packed_column_table.h"
#include "util/operator_utilities.h"

using namespace vaultdb;

// TODO: deserialize from file for better throughput
PackedColumnTable *PackedColumnTable::deserialize(const QuerySchema & schema, const int & src_tuple_cnt, const SortDefinition & collation, vector<int8_t> &packed_wires, const int & limit) {

    int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

    if(SystemConfiguration::getInstance().sendingParty()) {
        return new InputPartyPackedColumnTable(tuple_cnt, schema, collation);
    }

    ComputingPartyPackedColumnTable *table = new ComputingPartyPackedColumnTable(tuple_cnt, schema, collation);
    int8_t *read_cursor = packed_wires.data();
    for (int i = -1; i < schema.getFieldCount(); ++i) {
            int8_t *dst = table->packed_pages_[i].data();
            int write_size_bytes = table->packed_pages_[i].size();
            memcpy(dst, read_cursor, write_size_bytes);
            read_cursor += PackedColumnTable::bytesPerColumn(schema.getField(i), src_tuple_cnt);
        }
        return table;
    }

PackedColumnTable *PackedColumnTable::deserialize(const QuerySchema & schema, const int & src_tuple_cnt, const SortDefinition & src_collation, const string & filename, const vector<int> & ordinals, const int & limit) {
    int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;


    // else, first construct dst schema
    QuerySchema dst_schema;
    int cnt = 0;
    for(auto ord : ordinals) {
        auto desc = schema.getField(ord);
        desc.setOrdinal(cnt);
        dst_schema.putField(desc);
        ++cnt;
    }

    dst_schema.initializeFieldOffsets();
    auto dst_collation = OperatorUtilities::deriveCollation(src_collation, ordinals);

    // set up a dummy QueryTable to get offsets
    // what offsets to read in serialized file
    map<int, long> ordinal_offsets;
    ordinal_offsets[-1] = 0L;
    long cursor = 0;

    cout << "Mapping offsets of " << schema.getFieldCount() << " fields (+ dummy tag)\n";
    for(int i = -1; i < schema.getFieldCount();) {
        int bytes_per_col = PackedColumnTable::bytesPerColumn(schema.getField(i), src_tuple_cnt);
        cursor += bytes_per_col;
        ++i;
        ordinal_offsets[i] = cursor;
        cout << "   " << i << ": " << cursor << '\n';
    }
    long array_byte_cnt = cursor;
    // that's all we need!
    if(SystemConfiguration::getInstance().sendingParty()) return new InputPartyPackedColumnTable(tuple_cnt, dst_schema, dst_collation);

    // else
    ComputingPartyPackedColumnTable *dst = (ComputingPartyPackedColumnTable *) QueryTable<Bit>::getTable(tuple_cnt, dst_schema, dst_collation);

    FILE*  fp = fopen(filename.c_str(), "rb");
    // read dummy tag unconditionally
    long dummy_tag_bytes =  ordinal_offsets[0];
    fread(dst->packed_pages_[-1].data(),  1, dummy_tag_bytes, fp);
    int dst_ordinal = 0;

    for(auto src_ordinal : ordinals) {
        long read_offset = ordinal_offsets[src_ordinal];
        long read_size_bytes = (src_ordinal == (schema.getFieldCount() - 1)) ?   (array_byte_cnt - read_offset) : (ordinal_offsets[src_ordinal + 1] - read_offset);
        fseek(fp, read_offset, SEEK_SET); // read read_offset bytes from beginning of file
        fread(dst->packed_pages_[dst_ordinal].data(), 1, read_size_bytes, fp);
    }

    fclose(fp);

    return dst;

}