#ifndef _COMPUTING_PARTY_PACKED_COLUMN_TABLE_
#define _COMPUTING_PARTY_PACKED_COLUMN_TABLE_
#include "query_table/packed_column_table.h"
#include <vector>
#include <string>

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")
namespace vaultdb {
class ComputingPartyPackedColumnTable : public PackedColumnTable  {
public:
    ComputingPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) { }

    ComputingPartyPackedColumnTable(const ComputingPartyPackedColumnTable &src) : PackedColumnTable(src) {}

    QueryTable<Bit> *clone()  override {
        return new ComputingPartyPackedColumnTable(*this);
    }


};
} // namespace vaultdb


#else
namespace vaultdb {
class ComputingPartyPackedColumnTable : public PackedColumnTable  {
public:
    ComputingPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) {

        if(tuple_cnt == 0)
            return;


        OMPCPackedWire zero(bpm_.block_n_);
        vector<int8_t> zero_block = serializePackedWire(zero);

        assert(zero_block.size() == packed_wire_size_bytes_);

        // initialize packed wires
        for(int i = 0; i < schema_.getFieldCount(); ++i) {
            int col_packed_wires = (wires_per_field_.at(i) == 1) ? tuple_cnt_ / fields_per_wire_.at(i) + (tuple_cnt_ % fields_per_wire_.at(i) != 0) : (tuple_cnt_ * wires_per_field_.at(i));

            packed_pages_[i] = std::vector<int8_t>(col_packed_wires * packed_wire_size_bytes_);
            for(int j = 0; j < col_packed_wires; ++j) {
                memcpy(packed_pages_[i].data() + j * packed_wire_size_bytes_, zero_block.data(), packed_wire_size_bytes_);
            }
        }

        // initialize dummy tags to true until the row is initialized
        vector<int8_t> one_block = serializePackedWire(one_wire_);
        int dummy_tag_packed_wires = tuple_cnt_ / bpm_.unpacked_page_size_bits_ + ((tuple_cnt_ % bpm_.unpacked_page_size_bits_) != 0);
        packed_pages_[-1] = std::vector<int8_t>(dummy_tag_packed_wires * packed_wire_size_bytes_);
        for(int i = 0; i < dummy_tag_packed_wires; ++i) {
            memcpy(packed_pages_[-1].data() + i * packed_wire_size_bytes_, one_block.data(), packed_wire_size_bytes_);
        }


    }

    ComputingPartyPackedColumnTable(const ComputingPartyPackedColumnTable &src) : PackedColumnTable(src){

        ComputingPartyPackedColumnTable *src_table = const_cast<ComputingPartyPackedColumnTable *>(&src);
        bpm_.flushTable(src_table->table_id_);

        for(auto col_entry : src.packed_pages_) {
            packed_pages_[col_entry.first] = col_entry.second;
        }
    }

    QueryTable<Bit> *clone()  override {
        return new ComputingPartyPackedColumnTable(*this);
    }

     void writePackedWire(const PageId & pid, OMPCPackedWire & wire) override {
        int8_t *write_ptr = packed_pages_[pid.col_id_].data() + pid.page_idx_ * packed_wire_size_bytes_;

        memcpy(write_ptr, reinterpret_cast<int8_t *>(&wire.spdz_tag), block_size_bytes_);
        write_ptr += block_size_bytes_;

        memcpy(write_ptr, reinterpret_cast<int8_t *>(wire.packed_masked_values.data()), bpm_.block_n_ * block_size_bytes_);
        write_ptr += bpm_.block_n_ * sizeof(block);

        memcpy(write_ptr, reinterpret_cast<int8_t *>(wire.packed_lambdas.data()), bpm_.block_n_ * block_size_bytes_);

    }

     OMPCPackedWire readPackedWire(const PageId & pid)  const override {
        return deserializePackedWire(packed_pages_[pid.col_id_].data() + pid.page_idx_ * packed_wire_size_bytes_);
    }

};
} // namespace vaultdb

#endif

#endif