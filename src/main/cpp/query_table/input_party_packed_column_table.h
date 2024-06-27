#ifndef _INPUT_PARTY_PACKED_COLUMN_TABLE_
#define _INPUT_PARTY_PACKED_COLUMN_TABLE_

#include "query_table/packed_column_table.h"
#include <new>


// substitute for PackedColumnTable if SystemConfiguration::party_ == SystemConfiguration::input_party_
// input party / TP (trusted party) does not need to hold secret shares, instead it produces zero block for every pack/unpack operation
// this is just a placeholder for now.
// really need an abstract PackedColumnTable with purely virtual methods for readPackedWire and writePackedWire
// this will force it to "use the right one" for the buffer pool while keeping them interchangeable in the code
#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace vaultdb {
    class InputPartyPackedColumnTable : public PackedColumnTable {
         InputPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) {}

    };
}
#else
/*namespace  vaultdb {
    class InputPartyPackedColumnTable : public QueryTable<Bit> {
    public:

        // setup for buffer pool
        int table_id_ = SystemConfiguration::getInstance().num_tables_++;
        BufferPoolManager  & bpm_ = SystemConfiguration::getInstance().bpm_;
        mutable std::map<int, std::vector<int8_t>> packed_pages_; // map<col id, vector of serialized OMPCPackedWires>

        // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
        // maps ordinal to wire count.  floor(128/field_size_bits)
        map<int, int> fields_per_wire_;
        // if a field spans more than one block per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple wires
        map<int, int> blocks_per_field_;

        EmpManager *manager_;

        int block_size_bytes_ = sizeof(block); // 16 bytes per block = 128 bits
        int packed_wire_size_bytes_ = (2 * bpm_.block_n_ + 1) * block_size_bytes_;




        InputPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {

            BufferPoolManager bpm = SystemConfiguration::getInstance().bpm_;
            zero_block_ = OMPCPackedWire(bpm.block_n_);
            bpm.registerTable(table_id_, (PackedColumnTable *) this);
            // still need the right metadata
            this->tuple_cnt_ = tuple_cnt;

        }

        OMPCPackedWire readPackedWire(const PageId & pid) const  {
            cout << "TP reading packed wire!" << endl;
            return zero_block_;
        }

        void writePackedWire(const PageId & pid, OMPCPackedWire & wire)  {
            // do nothing
            cout << "TP writing packed wire!" << endl;
        }

    private:
        OMPCPackedWire zero_block_;
    };
}*/
#endif

#endif //_INPUT_PARTY_PACKED_COLUMN_TABLE_