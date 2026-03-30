#ifndef _INPUT_PARTY_PACKED_COLUMN_TABLE_
#define _INPUT_PARTY_PACKED_COLUMN_TABLE_

#include "query_table/packed_column_table.h"

#include <unordered_set>
#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace vaultdb {


    class InputPartyPackedColumnTable : public PackedColumnTable {
    public:
        InputPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) {}

        InputPartyPackedColumnTable(const InputPartyPackedColumnTable & src) : PackedColumnTable(src) {}


        QueryTable<Bit> *clone()  override {
            return new InputPartyPackedColumnTable(*this);
        }

        void writeToFile(const string &fq_filename) const override {

        }



    };
}
#else
namespace  vaultdb {
    class InputPartyPackedColumnTable : public PackedColumnTable {
    public:
        InputPartyPackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : PackedColumnTable(tuple_cnt, schema, sort_def) {
            zero_block_ = OMPCPackedWire(block_cnt_);
            zero_ = Bit(0);

        }

        InputPartyPackedColumnTable(const InputPartyPackedColumnTable &src) : PackedColumnTable(src), zero_block_(src.zero_block_), zero_(src.zero_) {
            InputPartyPackedColumnTable *src_table = const_cast<InputPartyPackedColumnTable *>(&src);
            bpm_.flushTable<Bit>(src_table->table_id_);
        }

        ~InputPartyPackedColumnTable()  {
                // Flush pages in buffer pool
                bpm_.removeTable<Bit>(this->table_id_);

        }

        QueryTable<Bit> *clone()  override {
            return new InputPartyPackedColumnTable(*this);
        }



        vector<int8_t> serializePackedWire(const PageId & pid) const override {
            return zero_vector_;
        }

        vector<int8_t> serializePackedWire(OMPCPackedWire & wire) const override {
            return zero_vector_;
        }

        emp::OMPCPackedWire deserializePackedWire(int8_t *serialized_packed_wire) const override{
            return zero_block_;
        }

        OMPCPackedWire readPackedWire(const PageId & pid) const override {
            return zero_block_;
        }

        void writePackedWire(const PageId & pid, OMPCPackedWire & wire) override {
           // do nothing
        }


        void getPage(const PageId &pid, int8_t *d) override {
            cout << "Reading packed wire for " << pid.toString() << " from zero page." << endl;
            Bit *dst = (Bit *) d;
            memcpy(dst, zero_page_.data(), page_size_bits_ * sizeof(Bit));
            manager_->unpack((Bit *) &zero_block_, dst, page_size_bits_);
            cout << "   Page starts with " << DataUtilities::printByteArray((int8_t *) dst, 16) << '\n';
        }

        void flushPage(const PageId &pid, int8_t *s) override {
            cout << "Packing " << DataUtilities::printByteArray(s, 16) << " for " << pid.toString() << '\n';
            Bit *src = (Bit *) s;
            OMPCPackedWire wire(block_cnt_);
            manager_->pack(src,  (Bit *) &wire, page_size_bits_);

        }

        void appendColumn(const QueryFieldDesc & desc) override {
            appendColumnSetup(desc);
        }

        void writeToFile(const string &fq_filename) const override {
            throw runtime_error("Not yet implemented!");

        }

    private:
        OMPCPackedWire zero_block_;
        Bit zero_;
        vector<int8_t> zero_vector_ = {0};
    };
}
#endif
#endif
