#ifndef _PACKED_COLUMN_TABLE_H_
#define _PACKED_COLUMN_TABLE_H_
#include "query_table.h"
#include "query_table/secure_tuple.h"

// only supports Bit for now
namespace vaultdb {
    class PackedColumnTable : public QueryTable<Bit> {

    public:

        int table_id_ = -1;
        bool bp_enabled_ = SystemConfiguration::getInstance().bp_enabled_;
        BufferPoolManager *bgm_ = bp_enabled_ ? SystemConfiguration::getInstance().bpm_ : nullptr;

        // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
        // maps ordinal to wire count.  floor(128/field_size_bits)
        map<int, int> fields_per_wire_;
        // if a field spans more than one block per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple blocks
        map<int, int> blocks_per_field_;

        int tuple_packed_size_bytes_ = 0;
        std::map<int, int> field_packed_sizes_bytes_;

        EmpManager *manager_;

        // cache packed_block: blocks_per_field_[col] * 128 bits
        typedef struct unpacked_t {
            std::vector<emp::Bit> unpacked_blocks_;
            int page_idx_;
            bool dirty_bit_ = false;
        }  Unpacked;

        mutable std::map<int, Unpacked> unpacked_wires_;

        mutable std::map<int, std::vector<emp::OMPCPackedWire> > packed_column_data_;

        int block_byte_size_ = 16; // 16 bytes per block = 128 bits


        PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def), manager_(SystemConfiguration::getInstance().emp_manager_) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
            setSchema(schema);
            if(tuple_cnt == 0)
                return;

            table_id_ = SystemConfiguration::getInstance().num_tables_++;

            if(!bp_enabled_) {
                for (int i = 0; i < schema_.getFieldCount(); ++i) {
                    int wires_cnt = isMultiwire(i) ? tuple_cnt : (tuple_cnt_ / fields_per_wire_.at(i) +
                                                                  (tuple_cnt_ % fields_per_wire_.at(i) != 0));
                    emp::OMPCPackedWire pack_wire(blocks_per_field_.at(i));
                    packed_column_data_[i] = std::vector<emp::OMPCPackedWire>(wires_cnt, pack_wire);
                    unpacked_wires_[i].unpacked_blocks_ = std::vector<emp::Bit>(blocks_per_field_.at(i) * 128,
                                                                                emp::Bit(0));
                }

                int dummy_tag_wire_cnt = tuple_cnt_ / 128 + (tuple_cnt_ % 128 != 0);
                emp::OMPCPackedWire pack_dummy_wire(1);
                packed_column_data_[-1] = std::vector<emp::OMPCPackedWire>(dummy_tag_wire_cnt, pack_dummy_wire);
                unpacked_wires_[-1].unpacked_blocks_ = std::vector<emp::Bit>(128, emp::Bit(0));
            }
        }

        PackedColumnTable(const PackedColumnTable &src) : QueryTable<Bit>(src), manager_(SystemConfiguration::getInstance().emp_manager_) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
            setSchema(src.schema_);

            if(src.tuple_cnt_ == 0)
                return;

            table_id_ = src.table_id_;

            if(!bp_enabled_) {
                for (int i = 0; i < schema_.getFieldCount(); ++i) {
                    int wires_cnt = isMultiwire(i) ? tuple_cnt_ : (tuple_cnt_ / fields_per_wire_.at(i) +
                                                                   (tuple_cnt_ % fields_per_wire_.at(i) != 0));
                    emp::OMPCPackedWire pack_wire(blocks_per_field_.at(i));
                    packed_column_data_[i] = std::vector<emp::OMPCPackedWire>(wires_cnt, pack_wire);
                    unpacked_wires_[i].unpacked_blocks_ = std::vector<emp::Bit>(blocks_per_field_[i] * 128,
                                                                                emp::Bit(0));
                }

                int dummy_tag_wire_cnt = tuple_cnt_ / 128 + (tuple_cnt_ % 128 != 0);
                emp::OMPCPackedWire pack_dummy_wire(1);
                packed_column_data_[-1] = std::vector<emp::OMPCPackedWire>(dummy_tag_wire_cnt, pack_dummy_wire);
                unpacked_wires_[-1].unpacked_blocks_ = std::vector<emp::Bit>(128, emp::Bit(0));

                for (int i = 0; i < tuple_cnt_; ++i) {
                    for (int j = 0; j < getSchema().getFieldCount(); ++j) {
                        setField(i, j, src.getField(i, j));
                    }

                    setDummyTag(i, src.getDummyTag(i));
                }
            }
        }

        inline int getPackedWireIdx(const int & row, const int & col) const {
            if(isMultiwire(col)) {
                return row;
            }
            else {
                return row / fields_per_wire_.at(col);
            }
        }

        // offset in bytes
        inline int getUnpackedBlockOffset(const int & row, const int & col) const {
            QueryFieldDesc desc = schema_.getField(col);

            if(blocks_per_field_.at(col) == 1)  {
                return  (row % fields_per_wire_.at(col)) * desc.size();
            }
            return 0;
        }

        void packColumn(const int & col) const {
            emp::OMPCPackedWire pack_wire(blocks_per_field_.at(col));
            manager_->pack(unpacked_wires_.at(col).unpacked_blocks_.data(),(Bit *) &pack_wire, blocks_per_field_.at(col) * 128);
            packed_column_data_[col][unpacked_wires_[col].page_idx_] = pack_wire;
            unpacked_wires_[col].dirty_bit_ = false;
        }

        void cacheField(const int & row, const int & col) const {


            int current_page_idx = getPackedWireIdx(row, col);

	        if(unpacked_wires_[col].page_idx_ != current_page_idx) {
                if(unpacked_wires_[col].dirty_bit_) {
                    // Flush to column_data_
                    packColumn(col);
                }

                emp::OMPCPackedWire& packed_wire = packed_column_data_.at(col).at(current_page_idx);
                unpacked_wires_[col].unpacked_blocks_ = std::vector<emp::Bit>(blocks_per_field_.at(col) * 128, emp::Bit(0));
                emp::Bit *unpack = unpacked_wires_.at(col).unpacked_blocks_.data();
                manager_->unpack((Bit *) &packed_wire, unpack, blocks_per_field_.at(col) * 128);
                unpacked_wires_[col].page_idx_ = current_page_idx;
            }
        }

        Field<Bit> getField(const int  & row, const int & col)  const override {
            if(bp_enabled_) {
                BufferPoolManager::PageId pid = bgm_->getPageId(table_id_, col, row, fields_per_wire_.at(col));
                BufferPoolManager::UnpackedPage up = bgm_->getUnpackedPage(pid);

                Bit *read_ptr = up.page_payload_.data() + ((row % fields_per_wire_.at(col)) * schema_.getField(col).size());

                up.access_counters_++;

                bgm_->unpacked_page_buffer_pool_[bgm_->getPageIdKey(pid)] = up;

                return Field<Bit>::deserialize(schema_.getField(col), (int8_t *) read_ptr);
            }
            else {
                cacheField(row, col);

                Bit *read_ptr = unpacked_wires_.at(col).unpacked_blocks_.data() + getUnpackedBlockOffset(row, col);
                return Field<Bit>::deserialize(schema_.getField(col), (int8_t *) read_ptr);
            }
        }


        inline void setField(const int  & row, const int & col, const Field<Bit> & f)  override {
            if(bp_enabled_) {
                BufferPoolManager::PageId pid = bgm_->getPageId(table_id_, col, row, fields_per_wire_.at(col));
                BufferPoolManager::UnpackedPage up = bgm_->getUnpackedPage(pid);

                Bit *write_ptr = up.page_payload_.data() + (((row % fields_per_wire_.at(col)) * schema_.getField(col).size()));
                f.serialize((int8_t *) write_ptr, f, schema_.getField(col));
                up.access_counters_++;

                bgm_->unpacked_page_buffer_pool_[bgm_->getPageIdKey(pid)] = up;
            }
            else {
                cacheField(row, col);

                Bit *write_ptr = unpacked_wires_.at(col).unpacked_blocks_.data() + getUnpackedBlockOffset(row, col);
                f.serialize((int8_t *) write_ptr, f, schema_.getField(col));
                unpacked_wires_[col].dirty_bit_ = true;
            }
        }

        SecureTable *secretShare() override  {
            assert(this->isEncrypted());
            throw; // can't secret share already encrypted table
        }

        void appendColumn(const QueryFieldDesc & desc) override {
            int ordinal = desc.getOrdinal();
            assert(ordinal == this->schema_.getFieldCount());

            this->schema_.putField(desc);
            this->schema_.initializeFieldOffsets();

            int field_size_bytes = desc.size() * sizeof(emp::Bit);
            tuple_size_bytes_ += field_size_bytes;
            field_sizes_bytes_[ordinal] = field_size_bytes;

            int packed_blocks;
            int field_packed_size_bytes;
            if(desc.size() / 128 > 0) {
                packed_blocks = desc.size() / 128 + (desc.size() % 128 != 0);
                field_packed_size_bytes = (1 + 2 * packed_blocks) * block_byte_size_;

                fields_per_wire_[ordinal] = 1;
                blocks_per_field_[ordinal] = packed_blocks;
            }
            else {
                packed_blocks = 1;
                field_packed_size_bytes = 3 * block_byte_size_;

                fields_per_wire_[ordinal] = 128 / desc.size();
                blocks_per_field_[ordinal] = 1;
            }

            tuple_packed_size_bytes_ += field_packed_size_bytes;
            field_packed_sizes_bytes_[ordinal] = field_packed_size_bytes;

            if(!bp_enabled_) {
                int wires_cnt = isMultiwire(ordinal) ? tuple_cnt_ : (tuple_cnt_ / fields_per_wire_.at(ordinal) +
                                                                     (tuple_cnt_ % fields_per_wire_.at(ordinal) != 0));
                emp::OMPCPackedWire pack_wire(blocks_per_field_.at(ordinal));
                packed_column_data_[ordinal] = std::vector<emp::OMPCPackedWire>(wires_cnt, pack_wire);
                unpacked_wires_[ordinal].unpacked_blocks_ = std::vector<emp::Bit>(blocks_per_field_.at(ordinal) * 128,
                                                                                  emp::Bit(0));
            }
            else {
                throw; // NYI
            }
        }

        void resize(const size_t &tuple_cnt) override {
            if(tuple_cnt == this->tuple_cnt_) return;

            int old_tuple_cnt = this->tuple_cnt_;

            this->tuple_cnt_ = tuple_cnt;

            if(!bp_enabled_) {
                for (int i = 0; i < schema_.getFieldCount(); ++i) {
                    int old_wires_cnt = isMultiwire(i) ? old_tuple_cnt : (old_tuple_cnt / fields_per_wire_.at(i) +
                                                                          (old_tuple_cnt % fields_per_wire_.at(i) !=
                                                                           0));
                    int wires_cnt = isMultiwire(i) ? tuple_cnt : (tuple_cnt / fields_per_wire_.at(i) +
                                                                  (tuple_cnt % fields_per_wire_.at(i) != 0));
                    emp::OMPCPackedWire pack_wire(blocks_per_field_.at(i));
                    packed_column_data_[i].resize(wires_cnt);

                    for (int j = old_wires_cnt; j < wires_cnt; ++j) {
                        packed_column_data_[i][j] = pack_wire;
                    }
                }

                if (tuple_cnt > old_tuple_cnt) {
                    int old_dummy_tag_wire_cnt = old_tuple_cnt / 128 + (old_tuple_cnt % 128 != 0);
                    int dummy_tag_wire_cnt = tuple_cnt / 128 + (tuple_cnt % 128 != 0);
                    emp::OMPCPackedWire pack_wire(1);
                    packed_column_data_[-1].resize(dummy_tag_wire_cnt, pack_wire);

                    for (int i = old_dummy_tag_wire_cnt; i < dummy_tag_wire_cnt; ++i) {
                        packed_column_data_[-1][i] = pack_wire;
                    }
                }
            }
            else {
                throw; // NYI
            }
        }

        Bit getDummyTag(const int & row)  const override {
            if(bp_enabled_) {
                BufferPoolManager::PageId pid = bgm_->getPageId(table_id_, -1, row, fields_per_wire_.at(-1));
                BufferPoolManager::UnpackedPage up = bgm_->getUnpackedPage(pid);

                up.access_counters_++;
                bgm_->unpacked_page_buffer_pool_[bgm_->getPageIdKey(pid)] = up;

                return up.page_payload_[row % fields_per_wire_.at(-1)];
            }
            else {
                cacheField(row, -1);
                return unpacked_wires_.at(-1).unpacked_blocks_[row % 128];
            }
        }

        void setDummyTag(const int & row, const Bit & val) override {
            if(bp_enabled_) {
                BufferPoolManager::PageId pid = bgm_->getPageId(table_id_, -1, row, fields_per_wire_.at(-1));
                BufferPoolManager::UnpackedPage up = bgm_->getUnpackedPage(pid);

                up.page_payload_[row % fields_per_wire_.at(-1)] = val;
                up.access_counters_++;

                bgm_->unpacked_page_buffer_pool_[bgm_->getPageIdKey(pid)] = up;

            }
            else {
                cacheField(row, -1);
                unpacked_wires_[-1].unpacked_blocks_[row % 128] = val;
                unpacked_wires_[-1].dirty_bit_ = true;
            }
        }


        QueryTable<Bit> *clone()  override {
            if(!bp_enabled_) {
                this->flush();
            }
            return new PackedColumnTable(*this);
        }

        void setSchema(const QuerySchema &schema) override {
            schema_ = schema;
            this->plain_schema_ = QuerySchema::toPlain(schema);

            tuple_size_bytes_ = 0;

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                QueryFieldDesc desc = schema_.fields_.at(i);

                int size_threshold = bp_enabled_ ? bgm_->unpacked_page_size_ : 128;
                int packed_blocks = desc.size() / size_threshold + (desc.size() % size_threshold != 0);

                if(desc.size() / size_threshold > 0) {
                    fields_per_wire_[i] = 1;
                    blocks_per_field_[i] = packed_blocks;
                }
                else {
                    fields_per_wire_[i] = size_threshold / desc.size();
                    blocks_per_field_[i] = 1;
                }

                int field_size_bytes = desc.size() * sizeof(emp::Bit);
                tuple_size_bytes_ += field_size_bytes;
                field_sizes_bytes_[i] = field_size_bytes;

                int field_packed_size_bytes = (1 + 2 * packed_blocks) * block_byte_size_;
                tuple_packed_size_bytes_ += field_packed_size_bytes;
                field_packed_sizes_bytes_[i] = field_packed_size_bytes;
            }

            blocks_per_field_[-1] = 1;
            fields_per_wire_[-1] = bp_enabled_ ? bgm_->unpacked_page_size_ : 128;

            tuple_size_bytes_ += sizeof(emp::Bit);
            field_sizes_bytes_[-1] = sizeof(emp::Bit);

            int dummy_packed_size_bytes = (1 + 2 * blocks_per_field_[-1]) * block_byte_size_;
            tuple_packed_size_bytes_ += dummy_packed_size_bytes;
            field_packed_sizes_bytes_[-1] = dummy_packed_size_bytes;
        }

        QueryTuple<Bit> getRow(const int & idx) override {
            SecureTuple tuple(&schema_);

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                tuple.setField(i, getField(idx, i));
            }

            tuple.setDummyTag(getDummyTag(idx));

            return tuple;
        }

        void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {
            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                setField(idx, i, tuple.getField(i));
            }

            setDummyTag(idx, tuple.getDummyTag());
        }


        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {

            for(int col = 0; col < schema_.getFieldCount(); ++col) {
                Integer lhs_int = getField(lhs_row, col).getInt();
                Integer rhs_int = getField(rhs_row, col).getInt();

                emp::swap(swap, lhs_int, rhs_int);

                // write back lhs and rhs
                Field<Bit> lhs_field = Field<Bit>::deserialize(schema_.getField(col), (int8_t *) (lhs_int.bits.data()));
                Field<Bit> rhs_field = Field<Bit>::deserialize(schema_.getField(col), (int8_t *) (rhs_int.bits.data()));

                // set field
                setField(lhs_row, col, lhs_field);
                setField(rhs_row, col, rhs_field);
            }

            // swap dummy tag
            Bit lhs_dummy = getDummyTag(lhs_row);
            Bit rhs_dummy = getDummyTag(rhs_row);
            emp::swap(swap, lhs_dummy, rhs_dummy);

            setDummyTag(lhs_row, lhs_dummy);
            setDummyTag(rhs_row, rhs_dummy);
        }

        void cloneTable(const int & dst_row, QueryTable<Bit> *src) override {
            exit(-1);
            throw;
            // NYI
        }

        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                Field<Bit> f = src->getField(src_row, i);
                setField(dst_row, dst_col + i, f);
            }

            setDummyTag(dst_row, src->getDummyTag(src_row));
        }

        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                Integer dst_field_int = getField(dst_row, dst_col + i).getInt();
                Integer src_field_int = src->getField(src_row, i).getInt();

                Integer write_int = emp::If(write, src_field_int, dst_field_int);

                Field<Bit> write_field = Field<Bit>::deserialize(schema_.getField(dst_col + i), (int8_t *) write_int.bits.data());
                setField(dst_row, dst_col + i, write_field);
            }

            // copy dummy tag
            Bit dst_bit = getDummyTag(dst_row);
            Bit src_bit = src->getDummyTag(src_row);
            setDummyTag(dst_row, emp::If(write, src_bit, dst_bit));
        }

        void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {
            assert(src->getSchema().getField(src_col) == this->getSchema().getField(dst_col));


            // how many wires do we need to write?
            int rows_to_cp = src->tuple_cnt_ - src_row;
            if(rows_to_cp > (this->tuple_cnt_ - dst_row)) {
                rows_to_cp = this->tuple_cnt_ - dst_row; // truncate to our available slots
            }

            int read_cursor = src_row;
            int write_cursor = dst_row;

            // simple approach for now: simply call getField and setField for each entry
            // TODO: optimize this later
            for(int i = 0; i < rows_to_cp; ++i) {
                auto f = src->getField(read_cursor, src_col);
                this->setField(write_cursor, dst_col, f);
                ++read_cursor;
                ++write_cursor;
            }

        }

        StorageModel storageModel() const override { return StorageModel::PACKED_COLUMN_STORE; }

        void deserializeRow(const int & row, vector<int8_t> & src) override {
            int src_size_bytes = src.size() - sizeof(emp::Bit); // don't handle dummy tag until end
            int cursor = 0; // bytes
            int write_idx = 0; // column indexes

            // does not include dummy tag - handle further down in this method
            // re-pack row
            while(cursor < src_size_bytes && write_idx < this->schema_.getFieldCount()) {
                int bytes_remaining = src_size_bytes - cursor;
                int dst_len = this->field_sizes_bytes_.at(write_idx);
                int to_read = (dst_len < bytes_remaining) ? dst_len : bytes_remaining;

                vector<int8_t> dst_arr(to_read);
                memcpy(dst_arr.data(), src.data() + cursor, to_read);
                Field<Bit> dst_field = Field<Bit>::deserialize(this->schema_.getField(write_idx), dst_arr.data());
                setField(row, write_idx, dst_field);
                packColumn(write_idx);

                cursor += to_read;
                ++write_idx;
            }

            emp::Bit *dummy_tag = (emp::Bit*) (src.data() + src.size() - sizeof(emp::Bit));
            setDummyTag(row, *dummy_tag);
        }

        void flush() {
            for(auto unpacked_wire : unpacked_wires_) {
                if(unpacked_wire.second.dirty_bit_) {
                    emp::OMPCPackedWire& dst = packed_column_data_.at(unpacked_wire.first).at(unpacked_wire.second.page_idx_);
                    manager_->pack(unpacked_wire.second.unpacked_blocks_.data(), (Bit *) &dst, blocks_per_field_.at(unpacked_wire.first) * 128);
                    unpacked_wire.second.dirty_bit_ = false;
                }
            }
        }

    private:
        inline bool isMultiwire(const int & col_id) const {
            return blocks_per_field_.at(col_id) > 1;
        }

    };
}
#endif