#include "packed_column_table.h"
#include "computing_party_packed_column_table.h"
#include "input_party_packed_column_table.h"
#include "util/operator_utilities.h"

using namespace vaultdb;

// no ordinals, so read all cols, but only the first limit rows
PackedColumnTable *PackedColumnTable::deserialize(const TableMetadata  & md, const int & limit) {
    int src_tuple_cnt = md.tuple_cnt_;
    int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

    /*if(SystemConfiguration::getInstance().inputParty()) {
        return new InputPartyPackedColumnTable(tuple_cnt, md.schema_, md.collation_);
    }*/

    ComputingPartyPackedColumnTable *dst = new ComputingPartyPackedColumnTable(tuple_cnt, md.schema_, md.collation_);
    string filename = Utilities::getFilenameForTable(md.name_);

    FILE*  fp = fopen(filename.c_str(), "rb");
    for(int i = 0; i < md.schema_.getFieldCount(); ++i) {
        fread(dst->packed_pages_[i].data(), 1, dst->packed_pages_[i].size(), fp);
        fseek(fp, PackedColumnTable::bytesPerColumn(md.schema_.getField(i), src_tuple_cnt), SEEK_CUR);
    }

    // dummy tag
    fread(dst->packed_pages_[-1].data(), 1, dst->packed_pages_[-1].size(), fp);
    fclose(fp);

    return dst;
}

PackedColumnTable *PackedColumnTable::deserialize(const TableMetadata  & md, const vector<int> & ordinals, const int & limit) {
    int src_tuple_cnt = md.tuple_cnt_;
    int tuple_cnt = (limit == -1 || limit > src_tuple_cnt) ? src_tuple_cnt : limit;

    // if no projection
    if(ordinals.empty()) {
       return deserialize(md, limit);
    }


    // else, first construct dst schema from projection
    auto dst_schema = OperatorUtilities::deriveSchema(md.schema_, ordinals);
    auto dst_collation = OperatorUtilities::deriveCollation(md.collation_, ordinals);

    /*if(SystemConfiguration::getInstance().inputParty()) {
        return new InputPartyPackedColumnTable(tuple_cnt, dst_schema, dst_collation);
    }*/

    // else
    // find the offsets to read in serialized file for each column
    map<int, long> ordinal_offsets;
    long array_byte_cnt = 0L;

    ComputingPartyPackedColumnTable *dst = (ComputingPartyPackedColumnTable *) QueryTable<Bit>::getTable(tuple_cnt, dst_schema, dst_collation);

    cout << "Mapping offsets of " << md.schema_.getFieldCount() << " fields (+ dummy tag)\n";

    ordinal_offsets[0] = 0;
    int field_cnt = md.schema_.getFieldCount();
    for(int i = 1; i < field_cnt; ++i) {
        array_byte_cnt += PackedColumnTable::bytesPerColumn(md.schema_.getField(i-1), src_tuple_cnt);
        ordinal_offsets[i] = array_byte_cnt;
    }
    array_byte_cnt += PackedColumnTable::bytesPerColumn(md.schema_.getField(field_cnt - 1), src_tuple_cnt);
    ordinal_offsets[-1] = array_byte_cnt;
    array_byte_cnt += PackedColumnTable::bytesPerColumn(md.schema_.getField(-1), src_tuple_cnt);

    auto filename = Utilities::getFilenameForTable(md.name_);
    FILE*  fp = fopen(filename.c_str(), "rb");

    int dst_ordinal = 0;

    for(auto src_ordinal : ordinals) {
        fseek(fp,  ordinal_offsets[src_ordinal], SEEK_SET); // read read_offset bytes from beginning of file
        fread(dst->packed_pages_[dst_ordinal].data(), 1, dst->packed_pages_[dst_ordinal].size(), fp);
        ++dst_ordinal;
    }

    // read dummy tag unconditionally
    fseek(fp, ordinal_offsets[-1], SEEK_SET);
    fread(dst->packed_pages_[-1].data(),  1, dst->packed_pages_[-1].size(), fp);

    fclose(fp);
    return dst;

}