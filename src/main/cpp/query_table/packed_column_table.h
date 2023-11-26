#ifndef _PACKED_COLUMN_TABLE_H_
#define _PACKED_COLUMN_TABLE_H_
#include "query_table.h"

// only supports Bit for now
namespace vaultdb {
    class PackedColumnTable : public QueryTable<Bit> {

    public:


        // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
        // maps ordinal to wire count.  floor(128/field_size_bits)
        map<int, int> fields_per_wire_;
        // if a field spans more than one wire per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple wires
        map<int, int> wires_per_field_;

        // Number of wires needed for each column
        map<int, int> wires_cnt_;

        // cache packed_wires[each column] * 128 bits
        typedef struct unpacked_t {
            std::vector<emp::Bit> unpacked_wire;
            int page_idx_;
            bool dirty_bit_ = false;
        }  Unpacked;

        mutable map<int, Unpacked> unpacked_wires_;


        PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
            setSchema(schema);

            if(tuple_cnt == 0)
                return;

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                column_data_[i] = std::vector<int8_t>(wires_cnt_[i] * sizeof(OMPCPackedWire));
                unpacked_wires_[i].unpacked_wire = std::vector<emp::Bit>(wires_per_field_[i] * 128);
            }

            int dummy_tag_wire_cnt = tuple_cnt / 128 + (tuple_cnt % 128 != 0);
            column_data_[-1] = std::vector<int8_t>(dummy_tag_wire_cnt * sizeof(OMPCPackedWire));
            unpacked_wires_[-1].unpacked_wire = std::vector<emp::Bit>(128);
        }

        PackedColumnTable(const PackedColumnTable &src) : QueryTable<Bit>(src) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
            setSchema(src.schema_);

            if(src.tuple_cnt_ == 0)
                return;

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                unpacked_wires_[i].unpacked_wire = std::vector<emp::Bit>(wires_per_field_[i] * 128);
                memcpy(unpacked_wires_[i].unpacked_wire.data(), src.unpacked_wires_[i].unpacked_wire.data(), wires_per_field_[i] * 128);
            }

            unpacked_wires_[-1].unpacked_wire = std::vector<emp::Bit>(128);
            memcpy(unpacked_wires_[-1].unpacked_wire.data(), src.unpacked_wires_[-1].unpacked_wire.data(), 128);
        }

        int getWireIdx(const int & row, const int & col) const {
            if(wires_per_field_.at(col) == 1) {
                return row / fields_per_wire_.at(col);
            }
            else {
                return row * wires_per_field_.at(col);
            }
        }

        int getUnpackedWiresOffset(const int & row, const int & col) const {
            if(wires_per_field_.at(col) == 1) {
                int target_offset = row % fields_per_wire_.at(col);
                int field_bits_size = schema_.getField(col).size();
                return target_offset * field_bits_size;
            }
            else {
                return 0;
            }
        }

        void cacheField(const int & row, const int & col) const {
	        EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;

            int current_page_idx = getWireIdx(row, col);

	        if(unpacked_wires_[col].page_idx_ != current_page_idx) {
                if(unpacked_wires_[col].dirty_bit_) {
                    // Flush to column_data_
                    emp::OMPCPackedWire *pack_wire = ((emp::OMPCPackedWire *) column_data_.at(col).data()) + unpacked_wires_[col].page_idx_;
		            manager->pack(unpacked_wires_[col].unpacked_wire.data(), (Bit *) pack_wire, wires_per_field_.at(col) * 128);
                    unpacked_wires_[col].dirty_bit_ = false;
                }

                emp::OMPCPackedWire *unpack_wire = ((emp::OMPCPackedWire *) column_data_.at(col).data()) + current_page_idx;
                manager->unpack((Bit *) unpack_wire, unpacked_wires_[col].unpacked_wire.data(), wires_per_field_.at(col) * 128);

                unpacked_wires_[col].page_idx_ = current_page_idx;
            }
        }

        Field<Bit> getField(const int  & row, const int & col)  const override {
            cacheField(row, col);

            Bit *read_ptr = unpacked_wires_.at(col).unpacked_wire.data() + getUnpackedWiresOffset(row, col);
            return Field<Bit>::deserialize(schema_.getField(col), (int8_t *) read_ptr);
        }


        void setField(const int  & row, const int & col, const Field<Bit> & f)  override {
            cacheField(row, col);

            Bit *write_ptr = unpacked_wires_.at(col).unpacked_wire.data() + getUnpackedWiresOffset(row, col);
            f.serialize((int8_t *) write_ptr, f, schema_.getField(col));
            unpacked_wires_[col].dirty_bit_ = true;
        }

        SecureTable *secretShare() override  {
            assert(!this->isEncrypted());
            SystemConfiguration & conf = SystemConfiguration::getInstance();
            return conf.emp_manager_->secretShare((PlainTable *) this);
        }

        void appendColumn(const QueryFieldDesc & desc) override {
            // NYI
            throw;
        }

        void resize(const size_t &tuple_cnt) override {
            if(tuple_cnt == this->tuple_cnt_) return;

            // NYI
            throw;
        }

        Bit getDummyTag(const int & row)  const override {
            int target_offset = row % 128;

            cacheField(row, -1);
            return unpacked_wires_.at(-1).unpacked_wire[target_offset];
        }

        void setDummyTag(const int & row, const Bit & val) override {
            int target_offset = row % 128;

            cacheField(row, -1);

            unpacked_wires_[-1].unpacked_wire[target_offset] = val;
            unpacked_wires_[-1].dirty_bit_ = true;
        }


        QueryTable<Bit> *clone()  override {
            this->flush();
            return new PackedColumnTable(*this);
        }

        void setSchema(const QuerySchema &schema) override {
            schema_ = schema;
            this->plain_schema_ = QuerySchema::toPlain(schema);

            tuple_size_bytes_ = 0;
            QueryFieldDesc desc;
            int field_size_bytes;
            int running_count = 0;

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                auto desc = schema_.fields_.at(i);
                int packed_wires = desc.packedWires();
                field_size_bytes = desc.packedWires() * sizeof(emp::OMPCPackedWire);
                tuple_size_bytes_ += field_size_bytes;
                field_sizes_bytes_[i] = field_size_bytes;
                running_count += field_size_bytes;
                if(packed_wires == 1) {
                    fields_per_wire_[i] = 128 / desc.size();
                    wires_per_field_[i] = 1;
                    wires_cnt_[i] = tuple_cnt_ / fields_per_wire_[i] + (tuple_cnt_ % fields_per_wire_[i] != 0);
                }
                else {
                    wires_per_field_[i] = packed_wires;
                    fields_per_wire_[i] = 1;
                    wires_cnt_[i] = tuple_cnt_ * packed_wires;
                }
            }

            wires_per_field_[-1] = 1;
            fields_per_wire_[-1] = 128;
            wires_cnt_[-1] = tuple_cnt_ / 128 + (tuple_cnt_ % 128 != 0);

        }

        QueryTuple<Bit> getRow(const int & idx) override {
            throw;
            // NYI
            return QueryTuple<Bit>();
        }
        void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {
            throw;
            // NYI
        }


        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {
            throw;
            // NYI
        }

        void cloneTable(const int & dst_row, QueryTable<Bit> *src) override {
            throw;
            // NYI
        }

        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {
            throw;
            // NYI, make sure to include dummy tag in cloning
        }

        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {
            throw;
            // NYI, make sure to include dummy tag in cloning
        }

        void cloneColumn(const int & dst_col, const int & dst_idx, const QueryTable<Bit> *s, const int & src_col, const int & src_offset = 0) override {
            throw;
            // NYI
        }

        StorageModel storageModel() const override { return StorageModel::PACKED_COLUMN_STORE; }

        void deserializeRow(const int & row, vector<int8_t> & src) override {
            throw;
            // NYI
        }

        void flush() {
            for(auto unpacked_wire : unpacked_wires_) {
                if(unpacked_wire.second.dirty_bit_) {
                    emp::OMPCPackedWire *pack_wire = ((emp::OMPCPackedWire *) column_data_.at(unpacked_wire.first).data()) + unpacked_wire.second.page_idx_;

                    SystemConfiguration::getInstance().emp_manager_->pack(unpacked_wire.second.unpacked_wire.data(), (Bit *) pack_wire, wires_per_field_.at(unpacked_wire.first) * 128);
                    unpacked_wire.second.dirty_bit_ = false;
                }
            }
        }

    };
}
#endif
