#ifndef _PACKED_COLUMN_TABLE_H_
#define _PACKED_COLUMN_TABLE_H_
#include "query_table/query_table.h"
#include "query_table/secure_tuple.h"

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")


namespace vaultdb {
class PackedColumnTable : public QueryTable<Bit> {

public:
    // setup for buffer pool
    int table_id_ = SystemConfiguration::getInstance().num_tables_++;
    // maps ordinal to wire count.
    map<int, int> fields_per_wire_;
    // if a field spans more than one block per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple wires
    map<int, int> wires_per_field_;

    PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition())  : QueryTable<Bit>(tuple_cnt, schema, sort_def) {

    }

    PackedColumnTable(const PackedColumnTable &src)  : QueryTable<Bit>(src) {

    }

  static PackedColumnTable *deserialize(const TableMetadata & md, const int & limit = -1)  { return nullptr; }
  static PackedColumnTable *deserialize(const TableMetadata & md, const vector<int> & ordinals, const int & limit = -1) { return nullptr; }
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

     static PackedColumnTable *deserialize(const QuerySchema & schema, const int & tuple_cnt, const SortDefinition & collation, vector<int8_t> &packed_wires) { return nullptr; }

    void deserializeRow(const int & row, vector<int8_t> & src) override {

    }

    ~PackedColumnTable() {

    }

    std::vector<emp::Bit> getPage(const PageId &pid) override {
        return std::vector<emp::Bit>();
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
    mutable std::map<int, std::vector<int8_t>> packed_pages_; // map<col id, vector of serialized OMPCPackedWires>

    // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
    // maps ordinal to wire count.  floor(128/field_size_bits)
    map<int, int> fields_per_wire_;
    // if a field spans more than one block per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple wires
    map<int, int> wires_per_field_;
    OMPCPackedWire one_wire_; // consists of one wire of all true values, need to pack in the constructor so TP participates
    EmpManager *manager_;

    int block_size_bytes_ = sizeof(block); // 16 bytes per block = 128 bits
    int packed_wire_size_bytes_ = (2 * bpm_.block_n_ + 1) * block_size_bytes_;

    // how many bytes to allocate for a given column definition with a given tuple count?
    // for use in deserializing DB
    // may need to create this for ColumnTable and others or merge into a single util based on DB type
    // this specialization is needed because of mapping from `Bit` to wire.
    static inline long bytesPerColumn(const QueryFieldDesc & desc, const int & tuple_cnt)  {

        int bits_per_wire = BufferPoolManager::getInstance().unpacked_page_size_bits_;

        int fields_per_wire, wires_per_field;
        // multiple wires per field
        if(desc.size() / bits_per_wire > 0) {
            fields_per_wire  = 1;
            wires_per_field =  desc.size() / bits_per_wire + (desc.size() % bits_per_wire != 0);;
        }
        else {
            fields_per_wire = bits_per_wire / desc.size();
            wires_per_field = 1;
        }

        int packed_wires = (wires_per_field == 1) ? (tuple_cnt / fields_per_wire + (tuple_cnt % fields_per_wire != 0)) : (tuple_cnt * wires_per_field);
        int packed_wire_size_bytes =  (2 * BufferPoolManager::getInstance().block_n_ + 1) * sizeof(block);
        return packed_wires * packed_wire_size_bytes;
    }


    PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def), manager_(SystemConfiguration::getInstance().emp_manager_) {
        assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
        setSchema(schema);

        vector<Bit> dummy_tags(bpm_.unpacked_page_size_bits_, Bit(true));
        one_wire_ = OMPCPackedWire(bpm_.block_n_);
        manager_->pack(dummy_tags.data(), reinterpret_cast<Bit *>(&one_wire_), bpm_.unpacked_page_size_bits_);

        bpm_.registerTable(table_id_, (QueryTable<Bit> *) this);
    }

    PackedColumnTable(const PackedColumnTable &src) : QueryTable<Bit>(src), manager_(SystemConfiguration::getInstance().emp_manager_) {
        assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
        setSchema(src.schema_);
        one_wire_ = src.one_wire_;
        if(src.tuple_cnt_ == 0)
            return;

        bpm_.registerTable(table_id_, (QueryTable<Bit> *) this);

    }

    vector<int8_t> serializePackedWire(OMPCPackedWire & wire) {
        vector<int8_t> dst((2 * bpm_.block_n_ + 1) * sizeof(block), 0);

        int8_t * write_ptr = dst.data();
        memcpy(write_ptr, (int8_t *) &wire.spdz_tag, sizeof(block));

        write_ptr += sizeof(block);
        memcpy(write_ptr, (int8_t *) wire.packed_masked_values.data(), bpm_.block_n_ * sizeof(block));

        write_ptr += bpm_.block_n_ * sizeof(block);
        memcpy(write_ptr, (int8_t *) wire.packed_lambdas.data(), bpm_.block_n_ * sizeof(block));

        return dst;
    }

    virtual void writePackedWire(const PageId & pid, OMPCPackedWire & wire) = 0;
    virtual OMPCPackedWire readPackedWire(const PageId & pid)  const  = 0;


    vector<int8_t> serializePackedWire(int col_id, int page_idx) {
        vector<int8_t> dst(packed_wire_size_bytes_);
        int8_t *read_ptr = packed_pages_[col_id].data() + page_idx * packed_wire_size_bytes_;
        memcpy(dst.data(), read_ptr, packed_wire_size_bytes_);
        return dst;
    }


    emp::OMPCPackedWire deserializePackedWire(int8_t *serialized_packed_wire) const {
        int8_t *read_ptr = serialized_packed_wire;
        int read_stride = bpm_.block_n_ * sizeof(block);

        emp::OMPCPackedWire dst(bpm_.block_n_);

        memcpy((int8_t *) &dst.spdz_tag, read_ptr, sizeof(block));
        read_ptr += sizeof(block);

        memcpy((int8_t *) dst.packed_masked_values.data(), read_ptr, read_stride);
        read_ptr += read_stride;

        memcpy((int8_t *) dst.packed_lambdas.data(), read_ptr, read_stride);

        return dst;
    }



    // writes all packed wires out to buffer
     vector<int8_t> serialize() override {
        size_t output_buffer_len = 0L;
        bpm_.flushTable(table_id_); // flush all dirty pages to packed wires

        for(auto col_entry : packed_pages_) {
            output_buffer_len += col_entry.second.size();
        }

        vector<int8_t> output_buffer(output_buffer_len);
        // for input party case -  no secret shares!
        if(output_buffer_len == 0) return output_buffer;

        int8_t *write_ptr = output_buffer.data();
        // write cols followed by dummy tag
        // this is to match QueryTable<B> and others
        for(int i = 0; i < schema_.getFieldCount(); ++i) {
            memcpy(write_ptr, packed_pages_.at(i).data(), packed_pages_.at(i).size());
            write_ptr += packed_pages_.at(i).size();
        }

        // write dummy tag
        memcpy(write_ptr, packed_pages_.at(-1).data(), packed_pages_.at(-1).size());

        return output_buffer;

    }



    static PackedColumnTable *deserialize(const TableMetadata & md, const int & limit = -1);
    static PackedColumnTable *deserialize(const TableMetadata & md, const vector<int> & ordinals, const int & limit = -1);

    std::vector<emp::Bit> getPage(const PageId &pid) override {
        OMPCPackedWire src = readPackedWire(pid);
        std::vector<emp::Bit> dst(bpm_.unpacked_page_size_bits_, 0);
        bpm_.emp_manager_->unpack((Bit *) &src, dst.data(), bpm_.unpacked_page_size_bits_);
        ++bpm_.unpack_calls_;
        return dst;
    }



    Field<Bit> getField(const int  & row, const int & col)  const override {
        int field_blocks = wires_per_field_.at(col);

        if(field_blocks == 1) {
            PageId pid(table_id_, col, row / fields_per_wire_.at(col));
            emp::Bit *read_ptr = bpm_.getUnpackedPagePtr(pid) + ((row % fields_per_wire_.at(col)) * schema_.getField(col).size());
            return  Field<Bit>::deserialize(schema_.getField(col), (int8_t *) read_ptr);
        }

        vector<Bit> bits(field_blocks * bpm_.unpacked_page_size_bits_);
        Bit *write_ptr = bits.data();
        PageId pid(table_id_, col, row * field_blocks);
        // this sidesteps the buffer pool.  Need to think more about how to make this data collocated in one buffer where that page size is of varying lengths (but > 1 OMPCPackedWire)
        for(int i = 0; i < field_blocks; ++i) {
           OMPCPackedWire wire = readPackedWire(pid);
           manager_->unpack((Bit *) &wire, write_ptr, bpm_.block_n_);
           ++pid.page_idx_;
           write_ptr += bpm_.unpacked_page_size_bits_;
        }
        return  Field<Bit>::deserialize(schema_.getField(col), (int8_t *) bits.data());
    }


    inline void setField(const int  & row, const int & col, const Field<Bit> & f)  override {
        int field_blocks = wires_per_field_.at(col);

        if(field_blocks == 1) {
            PageId pid = bpm_.getPageId(table_id_, col, row, fields_per_wire_.at(col));
            emp::Bit *write_ptr = bpm_.getUnpackedPagePtr(pid) +   ((row % fields_per_wire_.at(col)) * schema_.getField(col).size());
            f.serialize((int8_t *) write_ptr, f, schema_.getField(col));
            bpm_.markDirty(pid);
            return;
        }

        vector<Bit> bits(field_blocks * bpm_.unpacked_page_size_bits_);
        f.serialize((int8_t *) bits.data(), f, schema_.getField(col));
        PageId pid(table_id_, col, row * field_blocks);
        Bit *read_ptr = bits.data();

        for(int i = 0; i < field_blocks; ++i) {
            Bit *write_ptr = bpm_.getUnpackedPagePtr(pid);
            memcpy(write_ptr, read_ptr, bpm_.unpacked_page_size_bits_ * sizeof(Bit));
            bpm_.markDirty(pid);
            ++pid.page_idx_;
            read_ptr += bpm_.unpacked_page_size_bits_;
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

        int size_threshold = bpm_.unpacked_page_size_bits_;
        int packed_blocks = desc.size() / size_threshold + (desc.size() % size_threshold != 0);

        if(desc.size() / size_threshold > 0) {
            fields_per_wire_[ordinal] = 1;
            wires_per_field_[ordinal] = packed_blocks;
        }
        else {
            fields_per_wire_[ordinal] = size_threshold / desc.size();
            wires_per_field_[ordinal] = 1;
        }

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
    }




    void setSchema(const QuerySchema &schema) override {
        schema_ = schema;
        this->plain_schema_ = QuerySchema::toPlain(schema);

        tuple_size_bytes_ = 0;

        for(int i = 0; i < schema_.getFieldCount(); ++i) {
            QueryFieldDesc desc = schema_.fields_.at(i);

            int bits_per_page = bpm_.unpacked_page_size_bits_;

            if(desc.size() / bits_per_page > 0) {
                fields_per_wire_[i] = 1;
                wires_per_field_[i] = desc.size() / bits_per_page + (desc.size() % bits_per_page != 0);;
            }
            else {
                fields_per_wire_[i] = bits_per_page / desc.size();
                wires_per_field_[i] = 1;
            }

            int field_size_bytes = desc.size() * sizeof(emp::Bit);
            tuple_size_bytes_ += field_size_bytes;
            field_sizes_bytes_[i] = field_size_bytes;
        }

        wires_per_field_[-1] = 1;
        fields_per_wire_[-1] = bpm_.unpacked_page_size_bits_;

        tuple_size_bytes_ += sizeof(emp::Bit);
        field_sizes_bytes_[-1] = sizeof(emp::Bit);
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
            Field<Bit> src_field = src_table->getField(src_row, i);

            int write_row_idx = dst_row;
            // Write n copies of src rows to dst rows
            // TODO: maybe we can optimize this by cloneColumn.
            for(int j = 0; j < copies; ++j) {
                setField(write_row_idx, write_idx, src_field);
                ++write_row_idx;
            }

            ++write_idx;
        }

        // Copy dummy tag
        Bit dummy_tag = src_table->getDummyTag(src_row);

        int write_dummy_row_idx = dst_row;
        for(int i = 0; i < copies; ++i) {
            setDummyTag(write_dummy_row_idx, dummy_tag);
            ++write_dummy_row_idx;
        }
    }

    void cloneColumn(const int & dst_col, const QueryTable<Bit> *src_table, const int & src_col) override {

        assert(src_table->getSchema().getField(src_col) == this->getSchema().getField(dst_col));
        // 1:1 copy
        // if we're just copying whole pages, there are one of two paths if the source data are in memory and dirty:
        // 1) Unpack dst page, copy src page to dst page, mark dst page dirty
        // 2) Pack src page, mark src page as "not dirty", cp src page to dst page
        // for #1, both will likely need re-packing in the long run and this will create churn in the cache.
        // sidestep this by flushing and memcpying
        if(this->tuple_cnt_ == src_table->tuple_cnt_ && src_table->storageModel() == StorageModel::PACKED_COLUMN_STORE) {
            int bytes_to_clone = this->packed_pages_[dst_col].size();
            PackedColumnTable *src = (PackedColumnTable *) src_table;
            bpm_.flushColumn(src->table_id_, src_col);
            memcpy(packed_pages_[dst_col].data(), src->packed_pages_[src_col].data(), bytes_to_clone);
            return;
        }
        // else
        this->cloneColumn(dst_col, 0, src_table, src_col, 0);
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
        // Flush pages in buffer pool
        bpm_.removeTable(this->table_id_);

    }

protected:
    // for use by InputPartyPackedColumnTable
    PackedColumnTable(const QuerySchema &schema,
                      const SortDefinition &sort_def = SortDefinition()) :  QueryTable<Bit>(0, schema, sort_def) {
        setSchema(schema);

    }


};
} // namespace vaultdb
#endif

#endif
