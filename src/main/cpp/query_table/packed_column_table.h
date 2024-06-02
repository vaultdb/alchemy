#ifndef _PACKED_COLUMN_TABLE_H_
#define _PACKED_COLUMN_TABLE_H_
#include "query_table.h"
#include "query_table/secure_tuple.h"

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/zk.h")
// only supports Bit for now
namespace vaultdb {
    class PackedColumnTable : public QueryTable<Bit> {

    public:

        // setup for buffer pool
        int table_id_ = SystemConfiguration::getInstance().num_tables_++;
        BufferPoolManager *bpm_ = SystemConfiguration::getInstance().bpm_;
        mutable std::map<int, std::vector<emp::OMPCPackedWire>> packed_buffer_pool_; // map<col id, vector of OMPCPackedWires>

        // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
        // maps ordinal to wire count.  floor(128/field_size_bits)
        map<int, int> fields_per_wire_;
        // if a field spans more than one block per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple blocks
        map<int, int> blocks_per_field_;

        int tuple_packed_size_bytes_ = 0;
        std::map<int, int> field_packed_sizes_bytes_;

        EmpManager *manager_;

        int block_byte_size_ = 16; // 16 bytes per block = 128 bits


        PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def), manager_(SystemConfiguration::getInstance().emp_manager_) {

        }

        PackedColumnTable(const PackedColumnTable &src) : QueryTable<Bit>(src), manager_(SystemConfiguration::getInstance().emp_manager_) {

        }

        vector<int8_t> serializePackedWire(int col_id, int page_idx) {
            vector<int8_t> dst(1, 0);
            return dst;
        }

        emp::OMPCPackedWire deserializePackedWire(vector<int8_t> serialized_packed_wire) {
            return emp::OMPCPackedWire();
        }

        Field<Bit> getField(const int  & row, const int & col)  const override {
            return Field<Bit>();
        }


        inline void setField(const int  & row, const int & col, const Field<Bit> & f)  override {

        }

        SecureTable *secretShare() override  {
            assert(this->isEncrypted());
            throw; // can't secret share already encrypted table
        }

        void appendColumn(const QueryFieldDesc & desc) override {

        }

        void resize(const size_t &tuple_cnt) override {

        }

        Bit getDummyTag(const int & row)  const override {
            return emp::Bit(0);
        }

        void setDummyTag(const int & row, const Bit & val) override {

        }


        QueryTable<Bit> *clone()  override {
            return new PackedColumnTable(*this);
        }

        void setSchema(const QuerySchema &schema) override {

        }

        QueryTuple<Bit> getRow(const int & idx) override {
            return QueryTuple<Bit>();
        }

        void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {

        }


        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {

        }


        void cloneTable(const int & dst_row, const int & dst_col, QueryTable<Bit> *src) override {

        }

        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {

        }

        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {

        }

        void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row, const int & copies) override {

        }

        void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {

        }

        StorageModel storageModel() const override { return StorageModel::COLUMN_STORE; }

        void deserializeRow(const int & row, vector<int8_t> & src) override {

        }

        ~PackedColumnTable() {

        }
    };
}

#else
// only supports Bit for now
namespace vaultdb {
    class PackedColumnTable : public QueryTable<Bit> {

    public:

        // setup for buffer pool
        int table_id_ = SystemConfiguration::getInstance().num_tables_++;
        BufferPoolManager  & bpm_ = SystemConfiguration::getInstance().bpm_;
        mutable std::map<int, std::vector<emp::OMPCPackedWire>> packed_buffer_pool_; // map<col id, vector of OMPCPackedWires>

        // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
        // maps ordinal to wire count.  floor(128/field_size_bits)
        map<int, int> fields_per_wire_;
        // if a field spans more than one block per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple blocks
        map<int, int> blocks_per_field_;

        int tuple_packed_size_bytes_ = 0;
        std::map<int, int> field_packed_sizes_bytes_;

        EmpManager *manager_;

        int block_byte_size_ = 16; // 16 bytes per block = 128 bits


        PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def), manager_(SystemConfiguration::getInstance().emp_manager_) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
            setSchema(schema);

            if(tuple_cnt == 0)
                return;

            // initialize packed buffer pool including dummy tags
            for(int i = -1; i < schema_.getFieldCount(); ++i) {
                int col_packed_wires = tuple_cnt_ / fields_per_wire_.at(i) + (tuple_cnt_ % fields_per_wire_.at(i) != 0);
                packed_buffer_pool_[i] = std::vector<emp::OMPCPackedWire>(col_packed_wires, emp::OMPCPackedWire(bpm_.block_n_));
                cout << "Initalizing buffer pool for table " << table_id_ << ",  col " << i << " with " << packed_buffer_pool_[i][0].block_n << " block_n at " << (size_t ) packed_buffer_pool_[i].data() << endl;
                bpm_.packed_buffer_pool_[table_id_][i] = (OMPCPackedWire *) packed_buffer_pool_[i].data();
            }
        }

        PackedColumnTable(const PackedColumnTable &src) : QueryTable<Bit>(src), manager_(SystemConfiguration::getInstance().emp_manager_) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
            setSchema(src.schema_);

            if(src.tuple_cnt_ == 0)
                return;

            // initialize packed buffer pool including dummy tags
            for(int i = -1; i < schema_.getFieldCount(); ++i) {
                int max_packed_wires = tuple_cnt_ / fields_per_wire_.at(i) + (tuple_cnt_ % fields_per_wire_.at(i) != 0);
                packed_buffer_pool_[i] = std::vector<emp::OMPCPackedWire>(max_packed_wires, emp::OMPCPackedWire(bpm_.block_n_));
                bpm_.packed_buffer_pool_[table_id_][i] = packed_buffer_pool_[i].data();
            }

            PackedColumnTable *src_table = (PackedColumnTable *) &src;

            for(int i = -1; i < src_table->getSchema().getFieldCount(); ++i) {
                for(int j = 0; j < src_table->tuple_cnt_; j = j + src_table->fields_per_wire_.at(i)) {
                    PageId src_pid = bpm_.getPageId(src_table->table_id_, i, j, src_table->fields_per_wire_.at(i));
                    PageId dst_pid = bpm_.getPageId(table_id_, i, j, fields_per_wire_.at(i));
                    bpm_.clonePage(src_pid, dst_pid);
                }
            }
        }

        vector<int8_t> serializePackedWire(int col_id, int page_idx) {
            emp::OMPCPackedWire *wire_ptr = packed_buffer_pool_[col_id].data() + page_idx;
            vector<int8_t> dst((2 * bpm_.block_n_ + 1) * sizeof(block), 0);

            int8_t * write_ptr = dst.data();

            memcpy(write_ptr, (int8_t *) &wire_ptr->spdz_tag, sizeof(block));

            write_ptr += sizeof(block);

            memcpy(write_ptr, (int8_t *) wire_ptr->packed_masked_values.data(), bpm_.block_n_ * sizeof(block));

            write_ptr += bpm_.block_n_ * sizeof(block);

            memcpy(write_ptr, (int8_t *) wire_ptr->packed_lambdas.data(), bpm_.block_n_ * sizeof(block));

            return dst;
        }
        emp::OMPCPackedWire deserializePackedWire(int8_t *serialized_packed_wire) {
            int8_t *read_ptr = serialized_packed_wire;

            emp::OMPCPackedWire deserialized_wire(bpm_.block_n_);

            memcpy((int8_t *) &deserialized_wire.spdz_tag, read_ptr, sizeof(block));
            read_ptr += sizeof(block);

            memcpy((int8_t *) deserialized_wire.packed_masked_values.data(), read_ptr, bpm_.block_n_ * sizeof(block));
            read_ptr += bpm_.block_n_ * sizeof(block);

            memcpy((int8_t *) deserialized_wire.packed_lambdas.data(), read_ptr, bpm_.block_n_ * sizeof(block));

            return deserialized_wire;
        }

        emp::OMPCPackedWire deserializePackedWire(vector<int8_t> serialized_packed_wire) {
            int8_t *read_ptr = serialized_packed_wire.data();

            emp::OMPCPackedWire deserialized_wire(bpm_.block_n_);

            memcpy((int8_t *) &deserialized_wire.spdz_tag, read_ptr, sizeof(block));

            read_ptr += sizeof(block);

            memcpy((int8_t *) deserialized_wire.packed_masked_values.data(), read_ptr, bpm_.block_n_ * sizeof(block));

            read_ptr += bpm_.block_n_ * sizeof(block);

            memcpy((int8_t *) deserialized_wire.packed_lambdas.data(), read_ptr, bpm_.block_n_ * sizeof(block));

            return deserialized_wire;
        }

        Field<Bit> getField(const int  & row, const int & col)  const override {
            PageId pid = bpm_.getPageId(table_id_, col, row, fields_per_wire_.at(col));
            emp::Bit *read_ptr = bpm_.getUnpackedPagePtr(pid) + ((row % fields_per_wire_.at(col)) * schema_.getField(col).size());
            SecureField f  =  Field<Bit>::deserialize(schema_.getField(col), (int8_t *) read_ptr);
            bpm_.unpinPage(pid);
            return f;
        }


        inline void setField(const int  & row, const int & col, const Field<Bit> & f)  override {
            PageId pid = bpm_.getPageId(table_id_, col, row, fields_per_wire_.at(col));
            emp::Bit *write_ptr = bpm_.getUnpackedPagePtr(pid) + ((row % fields_per_wire_.at(col)) * schema_.getField(col).size());
            f.serialize((int8_t *) write_ptr, f, schema_.getField(col));
            bpm_.markDirty(pid);
            bpm_.unpinPage(pid);

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

            int size_threshold = bpm_.unpacked_page_size_bits_;
            int packed_blocks = desc.size() / size_threshold + (desc.size() % size_threshold != 0);

            if(desc.size() / size_threshold > 0) {
                fields_per_wire_[ordinal] = 1;
                blocks_per_field_[ordinal] = packed_blocks;
            }
            else {
                fields_per_wire_[ordinal] = size_threshold / desc.size();
                blocks_per_field_[ordinal] = 1;
            }

            int field_packed_size_bytes = (1 + 2 * packed_blocks) * block_byte_size_;
            tuple_packed_size_bytes_ += field_packed_size_bytes;
            field_packed_sizes_bytes_[ordinal] = field_packed_size_bytes;

            // TODO: check if we need to create empty pages for the new column
            cloneColumn(ordinal, -1, this, ordinal);
        }

        void resize(const size_t &tuple_cnt) override {
            if(tuple_cnt == this->tuple_cnt_) return;

            int old_tuple_cnt = this->tuple_cnt_;

            this->tuple_cnt_ = tuple_cnt;

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                int fields_per_wire = fields_per_wire_.at(i);

                if(this->tuple_cnt_ > old_tuple_cnt) {
                    int old_pos_in_last_page = old_tuple_cnt % fields_per_wire;

                    if(this->tuple_cnt_ - old_tuple_cnt > fields_per_wire - old_pos_in_last_page - 1) {
                        int rows_to_add = (this->tuple_cnt_ - old_tuple_cnt) - (fields_per_wire - old_pos_in_last_page - 1);

                        int pages_to_add = rows_to_add / fields_per_wire + ((rows_to_add % fields_per_wire) != 0);
                        bpm_.addPageSequence(table_id_, i, old_tuple_cnt + fields_per_wire - old_pos_in_last_page, pages_to_add, fields_per_wire);
                    }
                }
//                else{
//                    int new_pos_in_last_page = this->tuple_cnt_ % fields_per_wire;
//
//                    if(old_tuple_cnt - this->tuple_cnt_ > fields_per_wire - new_pos_in_last_page - 1) {
//                        int rows_to_remove = (old_tuple_cnt - this->tuple_cnt_) - (fields_per_wire - new_pos_in_last_page - 1);
//
//                        int pages_to_remove = rows_to_remove / fields_per_wire + ((rows_to_remove % fields_per_wire) != 0);
//                        bpm_.removePageSequence(table_id_, i, this->tuple_cnt_ + fields_per_wire - new_pos_in_last_page, pages_to_remove, fields_per_wire);
//                    }
//                }
            }
        }

        Bit getDummyTag(const int & row)  const override {
            auto pid = bpm_.getPageId(table_id_, -1, row, fields_per_wire_.at(-1));
            emp::Bit *read_ptr = bpm_.getUnpackedPagePtr(pid) + (row % fields_per_wire_.at(-1));
            return *read_ptr;
        }

        void setDummyTag(const int & row, const Bit & val) override {
            auto pid = bpm_.getPageId(table_id_, -1, row, fields_per_wire_.at(-1));
            emp::Bit *write_ptr = bpm_.getUnpackedPagePtr(pid) + (row % fields_per_wire_.at(-1));
            *write_ptr = val;
            bpm_.markDirty(pid);
            bpm_.unpinPage(pid);
        }


        QueryTable<Bit> *clone()  override {
            return new PackedColumnTable(*this);
        }

        void setSchema(const QuerySchema &schema) override {
            schema_ = schema;
            this->plain_schema_ = QuerySchema::toPlain(schema);

            tuple_size_bytes_ = 0;

            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                QueryFieldDesc desc = schema_.fields_.at(i);

                int size_threshold = bpm_.unpacked_page_size_bits_;
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
            fields_per_wire_[-1] = bpm_.unpacked_page_size_bits_;

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


        void cloneTable(const int & dst_row, const int & dst_col, QueryTable<Bit> *src) override {
            // TODO: need to check why cloneColumn does not work here.
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                for(int j = 0; j < src->tuple_cnt_; ++j) {
                    setField(dst_row + j, dst_col + i, src->getField(j, i));
                    setDummyTag(dst_row + j, src->getDummyTag(j));
                }
            }
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

        void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row, const int & copies) override {
            assert(src->storageModel() == StorageModel::PACKED_COLUMN_STORE);
            PackedColumnTable *src_table = (PackedColumnTable *) src;

            for(int i = 0; i < src_table->getSchema().getFieldCount(); ++i) {
                assert(src_table->getSchema().getField(i).size() == this->getSchema().getField(dst_col + i).size());
            }

            int write_idx = dst_col;
            for(int i = 0; i < src_table->getSchema().getFieldCount(); ++i) {
                // Get src unpacked page and pin it.
                PageId src_pid = bpm_.getPageId(src_table->table_id_, i, src_row, src_table->fields_per_wire_.at(i));
                bpm_.pinPage(src_pid);

                Field<Bit> src_field = src_table->getField(src_row, i);

                int write_row_idx = dst_row;
                // Write n copies of src rows to dst rows
                // TODO: maybe we can optimize this by cloneColumn.
                for(int j = 0; j < copies; ++j) {
                    setField(write_row_idx, write_idx, src_field);
                    ++write_row_idx;
                }

                ++write_idx;
                bpm_.unpinPage(src_pid);
            }

            // Copy dummy tag
            Bit dummy_tag = src_table->getDummyTag(src_row);

            int write_dummy_row_idx = dst_row;
            for(int i = 0; i < copies; ++i) {
                setDummyTag(write_dummy_row_idx, dummy_tag);
                ++write_dummy_row_idx;
            }
        }

        void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {
            assert(src->getSchema().getField(src_col) == this->getSchema().getField(dst_col));

            // if dst_row == -1, we initialize the column with zero based on src schema.
            if(dst_row == -1) {
                PackedColumnTable *src_table = (PackedColumnTable *) src;
                bpm_.loadColumn(table_id_, dst_col, src_table->tuple_cnt_, src_table->fields_per_wire_.at(src_col));
                return;
            }

            int rows_to_cp = src->tuple_cnt_ - src_row;
            if(rows_to_cp > (this->tuple_cnt_ - dst_row)) {
                rows_to_cp = this->tuple_cnt_ - dst_row; // truncate to our available slots
            }

            int read_cursor = src_row;
            int write_cursor = dst_row;

            PackedColumnTable *src_table = (PackedColumnTable *) src;

            for (int i = 0; i < rows_to_cp; i = i + src_table->fields_per_wire_.at(src_col)) {
                PageId src_pid = bpm_.getPageId(src_table->table_id_, src_col,
                                                                    read_cursor,
                                                                    src_table->fields_per_wire_.at(src_col));
                PageId dst_pid = bpm_.getPageId(table_id_, dst_col, write_cursor,
                                                                    fields_per_wire_.at(dst_col));
                bpm_.clonePage(src_pid, dst_pid);
                bpm_.unpinPage(src_pid);
                bpm_.unpinPage(dst_pid);
                read_cursor += src_table->fields_per_wire_.at(src_col);
                write_cursor += fields_per_wire_.at(dst_col);
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

                cursor += to_read;
                ++write_idx;
            }

            emp::Bit *dummy_tag = (emp::Bit*) (src.data() + src.size() - sizeof(emp::Bit));
            setDummyTag(row, *dummy_tag);
        }

        ~PackedColumnTable() {
            // Flush pages in buffer pools
            bpm_.removeUnpackedPagesByTable(this->table_id_);
            bpm_.packed_buffer_pool_.erase(this->table_id_);
        }

        PlainTable *revealInsecure(const int & party = emp::PUBLIC) override {
            if(!isEncrypted()) {
                return (QueryTable<bool> *) this;
            }

            QuerySchema dst_schema = QuerySchema::toPlain(schema_);

            auto dst_table = PlainTable::getTable(tuple_cnt_, dst_schema, order_by_);

            for(int i = -1; i < schema_.getFieldCount(); ++i) {
                PageId pid = BufferPoolManager::getPageId(table_id_, i, 0, fields_per_wire_.at(i));
                int cursor = 0;
                QueryFieldDesc field_desc = schema_.getField(i);

                for(int j = 0; j < packed_buffer_pool_[i].size(); ++j) {
                    bpm_.loadPage(pid);
                    // loop over the page
                    while(cursor < tuple_cnt_ && cursor < (j + 1) * fields_per_wire_.at(i)){
                        SecureField f = getField(cursor, i);
                        PlainField plain = f.reveal(field_desc, party);
                        dst_table->setField(cursor, i, plain);
                        ++cursor;
                    }
                    bpm_.unpinPage(pid);
                    ++pid.page_idx_;
                }
            }

            return dst_table;
        }
    };
}
#endif

#endif