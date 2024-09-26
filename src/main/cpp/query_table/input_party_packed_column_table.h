#ifndef _INPUT_PARTY_PACKED_COLUMN_TABLE_
#define _INPUT_PARTY_PACKED_COLUMN_TABLE_

#include "query_table/packed_column_table.h"


// substitute for PackedColumnTable if SystemConfiguration::party_ == SystemConfiguration::input_party_
// input party / TP (trusted party) does not need to hold secret shares, instead it produces zero block for every pack/unpack operation
#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace vaultdb {


    class InputPartyPackedColumnTable : public PackedColumnTable {
    public:
         InputPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) {}

         InputPartyPackedColumnTable(const InputPartyPackedColumnTable & src) : PackedColumnTable(src) {}


        QueryTable<Bit> *clone()  override {
            return new InputPartyPackedColumnTable(*this);
        }



    };
}
#else
namespace  vaultdb {
    class InputPartyPackedColumnTable : public PackedColumnTable {
    public:

        InputPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) {
            zero_block_ = OMPCPackedWire(bpm_.block_n_);

        }

        QueryTable<Bit> *clone()  override {
            return new InputPartyPackedColumnTable(*this);
        }


        OMPCPackedWire readPackedWire(const PageId & pid) const  override {
            return zero_block_;
        }

        void writePackedWire(const PageId & pid, OMPCPackedWire & wire) override {
            // do nothing
        }

    private:
        OMPCPackedWire zero_block_;
    };
}
#endif
#endif //_INPUT_PARTY_PACKED_COLUMN_TABLE_
