#ifndef _PACKED_COLUMN_TABLE_H_
#define _PACKED_COLUMN_TABLE_H_
#include "query_table/query_table.h"
#include "query_table/secure_tuple.h"
#include <filesystem>
#include <fstream>

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")


namespace vaultdb {
class PackedColumnTable : public QueryTable<Bit> {

public:
    // setup for buffer pool
    int table_id_ = SystemConfiguration::getInstance().table_cnt_++;
    // maps ordinal to wire count.
    map<int, int> fields_per_wire_;
    // if a field spans more than one block per instance (e.g., a string with length > 16 chars) then we need to map the field to multiple wires
    map<int, int> fields_per_page_;

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

    void getPage(const PageId &pid, int8_t *dst) override {

    }

    void flushPage(const PageId &pid, int8_t *src) override {

    }

    int8_t * getFieldPtr(const int &row, const int &col) const override {
        return nullptr;
    }

    void cloneColumn(const int &dst_col, const QueryTable<Bit> *src, const int &src_col) override {

    }

    void putTuple(const int &idx, const QueryTuple<Bit> &tuple) override {

    }

    vector<int8_t> serialize() override {
        return vector<int8_t>();
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

    void writeToFile(const string &fq_filename) const override {
    }

};
}

#else
// only supports Bit for now
namespace vaultdb {
class PackedColumnTable : public QueryTable<Bit> {

public:

    // setup for buffer pool
    BufferPoolManager  & bpm_ = SystemConfiguration::getInstance().bpm_;

    string table_file_name_; // file in the disk
    mutable fstream packed_pages_file_; // file pointer for read and write


    // choosing to branch instead of storing this in a float for now, need to analyze trade-off on this one
    // maps ordinal to wire count.  floor(128/field_size_bits)
    map<int, int> fields_per_page_;
    OMPCPackedWire one_wire_; // consists of one wire of all true values, need to pack in the constructor so TP participates
    EmpManager *manager_;
    int page_size_bits_ = bpm_.page_size_bytes_ / sizeof(Bit);
    int block_n_ = (page_size_bits_ / 128) + (page_size_bits_ % 128 != 0);

    int block_size_bytes_ = sizeof(block); // 16 bytes per block = 128 bits
    int packed_wire_size_bytes_ = (2 * block_n_ + 1) * block_size_bytes_;

    PackedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def), manager_(SystemConfiguration::getInstance().emp_manager_) {
        assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
        setSchema(schema);

        vector<Bit> ones(page_size_bits_,Bit(true));

        one_wire_ = OMPCPackedWire(block_n_);
        manager_->pack(ones.data(), reinterpret_cast<Bit *>(&one_wire_), page_size_bits_);

        bpm_.registerTable(this);
    }

    PackedColumnTable(const PackedColumnTable &src) : QueryTable<Bit>(src), manager_(SystemConfiguration::getInstance().emp_manager_) {
        assert(SystemConfiguration::getInstance().storageModel() == StorageModel::PACKED_COLUMN_STORE);
        setSchema(src.schema_);
        one_wire_ = src.one_wire_;
        if(src.tuple_cnt_ == 0)
            return;

        bpm_.registerTable(this);
    }

    virtual vector<int8_t> serializePackedWire(const PageId & pid) const = 0;
    virtual vector<int8_t> serializePackedWire(OMPCPackedWire & wire) const = 0;
    virtual emp::OMPCPackedWire deserializePackedWire(int8_t *serialized_packed_wire) const = 0;

    virtual void writePackedWire(const PageId & pid, OMPCPackedWire & wire) = 0;
    virtual OMPCPackedWire readPackedWire(const PageId & pid) const  = 0;

    static PackedColumnTable *deserialize(const TableMetadata & md, const int & limit = -1);
    static PackedColumnTable *deserialize(const TableMetadata & md, const vector<int> & ordinals, const int & limit = -1);


    Field<Bit> getField(const int  & row, const int & col) const override {
            PageId pid(table_id_, col, row / fields_per_page_.at(col));
            emp::Bit *read_ptr = ((Bit *)  bpm_.getPagePtr<Bit>(pid)) + ((row % fields_per_page_.at(col)) * schema_.getField(col).size());
            return  Field<Bit>::deserialize(schema_.getField(col), (int8_t *) read_ptr);
    }


    inline void setField(const int  & row, const int & col, const Field<Bit> & f)  override {
            PageId pid(table_id_, col, row /fields_per_page_.at(col));
            emp::Bit *write_ptr =  ((Bit *)  bpm_.getPagePtr<Bit>(pid)) +   ((row % fields_per_page_.at(col)) * schema_.getField(col).size());
            f.serialize((int8_t *) write_ptr, f, schema_.getField(col));
            bpm_.markDirty(pid);

    }

    SecureTable *secretShare() override  {
        assert(this->isEncrypted());
        throw; // can't secret share already encrypted table
    }



    Bit getDummyTag(const int & row)  const override {
        auto pid = bpm_.getPageId(table_id_, -1, row, fields_per_page_.at(-1));
        emp::Bit *read_ptr =  ((Bit *)  bpm_.getPagePtr<Bit>(pid)) + (row % fields_per_page_.at(-1));
        return *read_ptr;
    }

    void setDummyTag(const int & row, const Bit & val) override {
        auto pid = bpm_.getPageId(table_id_, -1, row, fields_per_page_.at(-1));
        emp::Bit *write_ptr =  ((Bit *)  bpm_.getPagePtr<Bit>(pid)) + (row % fields_per_page_.at(-1));
        *write_ptr = val;
        bpm_.markDirty(pid);
    }






    QueryTuple<Bit> getRow(const int & idx) override {
        SecureTuple tuple(&schema_);
        Bit *write_ptr = (Bit *) tuple.getData();

        for(int i = 0; i < schema_.getFieldCount(); ++i) {
            auto f = getField(idx, i);
            Field<Bit>::serialize((int8_t *) write_ptr, f, schema_.getField(i));
            write_ptr += schema_.getField(i).size();
        }

        Bit dummy_tag = getDummyTag(idx);
        *write_ptr = dummy_tag;

        return tuple;
    }

    void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {
        assert(*tuple.schema_ == schema_);

        for (int i = 0; i < schema_.getFieldCount(); ++i) {
            auto f = tuple.getField(i);
            setField(idx, i, f);
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
        // cloneColumn is not enough here because we can't guarantee that dst_row is aligned with wire packing boundaries
        // TODO: can add a check for this to implement this with memcpy when possible
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
            PageId src_pid = bpm_.getPageId(src_table->table_id_, i, src_row, src_table->fields_per_page_.at(i));
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
        assert(src_table->storageModel() == StorageModel::PACKED_COLUMN_STORE);

        if(this->tuple_cnt_ == src_table->tuple_cnt_) {
            int bytes_to_clone = PackedColumnTable::bytesPerColumn(schema_.getField(dst_col), this->tuple_cnt_);
            PackedColumnTable *src = (PackedColumnTable *) src_table;
            bpm_.flushColumn<Bit>(src->table_id_, src_col);
            src->packed_pages_file_.flush();

            copyAndWriteWires(src->packed_pages_file_, src->ordinal_offsets_.at(src_col), packed_pages_file_, ordinal_offsets_.at(dst_col), bytes_to_clone);
        }
        else {
            cloneColumn(dst_col, 0, src_table, src_col, 0);
        }
    }

    void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {
        assert(src->getSchema().getField(src_col) == this->getSchema().getField(dst_col));
        assert(src->storageModel() == StorageModel::PACKED_COLUMN_STORE);

        PackedColumnTable *src_table = (PackedColumnTable *) src;

        // if dst_row == -1, we initialize the column with zero based on src schema.
        if(dst_row == -1) {
            bpm_.loadColumn<Bit>(table_id_, dst_col, src_table->tuple_cnt_, src_table->fields_per_page_.at(src_col));
            return;
        }

        if((dst_row == 0) && (src_row == 0) && (tuple_cnt_ >= src->tuple_cnt_)) {
            // copy over packed pages to start of dst_col
            bpm_.flushColumn<Bit>(src_table->table_id_, src_col);
            src_table->packed_pages_file_.flush();

            int bytes_to_clone = PackedColumnTable::bytesPerColumn(schema_.getField(dst_col), this->tuple_cnt_);
            copyAndWriteWires(src_table->packed_pages_file_, src_table->ordinal_offsets_[src_col], packed_pages_file_, ordinal_offsets_[dst_col], bytes_to_clone);
            return;
        }

        int rows_to_cp = src->tuple_cnt_ - src_row;
        if(rows_to_cp > (this->tuple_cnt_ - dst_row)) {
            rows_to_cp = this->tuple_cnt_ - dst_row; // truncate to our available slots
        }

        // this does not work 1:1 for copying whole wires
        // because we are pulling from arbitrary row offsets
        // and those row offsets might not align with page boundaries
        int read_cursor = src_row;
        int write_cursor = dst_row;

        for (int i = 0; i < rows_to_cp; ++i) {
            Field<Bit> f = src->getField(read_cursor, src_col);
            setField(write_cursor, dst_col, f);
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

            cursor += to_read;
            ++write_idx;
        }

        emp::Bit *dummy_tag = (emp::Bit*) (src.data() + src.size() - sizeof(emp::Bit));
        setDummyTag(row, *dummy_tag);
    }

    void resize(const size_t &tuple_cnt) override {
        if(tuple_cnt == this->tuple_cnt_) return;

        bpm_.flushTable<Bit>(this->table_id_);

        int old_tuple_cnt = this->tuple_cnt_;
        this->tuple_cnt_ = tuple_cnt;

        auto new_ordinal_offsets = getOrdinalOffsets(schema_, this->tuple_cnt_);

        int old_dummy_page_cnt = old_tuple_cnt / fields_per_page_.at(-1) + (old_tuple_cnt % fields_per_page_.at(-1) != 0);
        int new_dummy_page_cnt = this->tuple_cnt_ / fields_per_page_.at(-1) + (this->tuple_cnt_ % fields_per_page_.at(-1) != 0);

        if(!SystemConfiguration::getInstance().inputParty()) {
            packed_pages_file_.flush();
            // create temp file
            string temp_file_name = table_file_name_ + ".temp";
            fstream temp_fp;
            temp_fp.open(temp_file_name.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
            OMPCPackedWire zero(block_n_);
            vector<int8_t> zero_block = serializePackedWire(zero);
            vector<int8_t> one_block = serializePackedWire(one_wire_);

            // adding rows
            if (old_tuple_cnt < this->tuple_cnt_) {
                for (int i = 0; i < schema_.getFieldCount(); ++i) {
                    int old_page_cnt =
                            old_tuple_cnt / fields_per_page_.at(i) + (old_tuple_cnt % fields_per_page_.at(i) != 0);
                    int new_page_cnt = this->tuple_cnt_ / fields_per_page_.at(i) +
                                       (this->tuple_cnt_ % fields_per_page_.at(i) != 0);

                    temp_fp.seekp(new_ordinal_offsets[i], ios::beg);
                    for (int j = 0; j < old_page_cnt; ++j) {
                        vector<int8_t> packed_wire = serializePackedWire(PageId(this->table_id_, i, j));
                        temp_fp.write(reinterpret_cast<char*>(packed_wire.data()), packed_wire.size());
                    }

                    for (int j = old_page_cnt; j < new_page_cnt; ++j) {
                        temp_fp.write(reinterpret_cast<char*>(zero_block.data()), zero_block.size());
                    }
                }

                temp_fp.seekp(new_ordinal_offsets[-1], ios::beg);
                for (int j = 0; j < old_dummy_page_cnt; ++j) {
                    vector<int8_t> packed_wire = serializePackedWire(PageId(this->table_id_, -1, j));
                    temp_fp.write(reinterpret_cast<char*>(packed_wire.data()), packed_wire.size());
                }

                for (int j = old_dummy_page_cnt; j < new_dummy_page_cnt; ++j) {
                    temp_fp.write(reinterpret_cast<char*>(one_block.data()), one_block.size());
                }
            } else {
                // remove rows
                for (int i = 0; i < schema_.getFieldCount(); ++i) {
                    int new_page_cnt = this->tuple_cnt_ / fields_per_page_.at(i) +
                                       (this->tuple_cnt_ % fields_per_page_.at(i) != 0);

                    temp_fp.seekp(new_ordinal_offsets[i], ios::beg);
                    for (int j = 0; j < new_page_cnt; ++j) {
                        vector<int8_t> packed_wire = serializePackedWire(PageId(this->table_id_, i, j));
                        temp_fp.write(reinterpret_cast<char*>(packed_wire.data()), packed_wire.size());
                    }
                }

                temp_fp.seekp(new_ordinal_offsets[-1], ios::beg);
                int new_dummy_page_cnt =
                        this->tuple_cnt_ / fields_per_page_.at(-1) + (this->tuple_cnt_ % fields_per_page_.at(-1) != 0);
                for (int j = 0; j < new_dummy_page_cnt; ++j) {
                    vector<int8_t> packed_wire = serializePackedWire(PageId(this->table_id_, -1, j));
                    temp_fp.write(reinterpret_cast<char*>(packed_wire.data()), packed_wire.size());
                }
            }

            // swap temp file with original
            temp_fp.flush();
            temp_fp.close();

            packed_pages_file_.flush();
            packed_pages_file_.close();

            string cmd = "mv " + temp_file_name + " " + table_file_name_;
            system(cmd.c_str());
            fopenPackedPagesFile();
        }

        this->ordinal_offsets_ = new_ordinal_offsets;

        // remained dummy tags in the old last page need to manually set to 1.
        int dummy_fields_per_wire = fields_per_page_.at(-1);
        int old_last_dummy_tag_offset = old_tuple_cnt % dummy_fields_per_wire;
        if(old_last_dummy_tag_offset != 0) {
            int new_last_dummy_offset = tuple_cnt_ % dummy_fields_per_wire;
            bool isSameLastPage = old_dummy_page_cnt == new_dummy_page_cnt;
            int update_dummy_tags_cnt = (isSameLastPage ? new_last_dummy_offset : dummy_fields_per_wire) - old_last_dummy_tag_offset;

            int last_dummy_page_offset = (old_dummy_page_cnt - 1) * dummy_fields_per_wire + old_last_dummy_tag_offset;
            for (int i = 0; i < update_dummy_tags_cnt; ++i) {
                setDummyTag(last_dummy_page_offset + i, Bit(true));
            }
        }
    }

    // how many bytes to allocate for a given column definition with a given tuple count?
    // for use in deserializing DB
    // may need to create this for ColumnTable and others or merge into a single util based on DB type
    // this specialization is needed because of mapping from `Bit` to wire.
    static inline long bytesPerColumn(const QueryFieldDesc & desc, const int & tuple_cnt)  {

        int bits_per_wire = BufferPoolManager::getInstance().page_size_bytes_ / sizeof(Bit);
        int block_n = (bits_per_wire / 128) + (bits_per_wire % 128 != 0);

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
        int packed_wire_size_bytes =  (2 * block_n + 1) * sizeof(block);
        return packed_wires * packed_wire_size_bytes;
    }


    static void copyAndWriteWires(fstream &src_file, long src_offset, fstream &dst_file, long dst_offset, int bytes_to_copy) {
        if(SystemConfiguration::getInstance().inputParty()) {
            return;
        }

        if(!(src_file.is_open() && src_file.good())) {
            throw std::runtime_error("src file is not open");
        }

        if(!(dst_file.is_open() && dst_file.good())) {
            throw std::runtime_error("dst file is not open");
        }

        vector<int8_t> src_arr(bytes_to_copy);
        src_file.seekg(src_offset, ios::beg);
        src_file.read(reinterpret_cast<char*>(src_arr.data()), bytes_to_copy);

        dst_file.seekp(dst_offset, ios::beg);
        dst_file.write(reinterpret_cast<char*>(src_arr.data()), bytes_to_copy);
        dst_file.flush();
    }

    ~PackedColumnTable() {
        // Flush pages in buffer pool
        bpm_.removeTable<Bit>(this->table_id_);

        // close the file
        if (packed_pages_file_.is_open() && packed_pages_file_.good()) {
            packed_pages_file_.close();
        }

        // remove the file
        string temp_dir = SystemConfiguration::getInstance().temp_db_path_;
        if (table_file_name_.substr(0, temp_dir.length()) == temp_dir) {
            Utilities::runCommand("rm " + table_file_name_);
        }
    }

protected:
    // for use by InputPartyPackedColumnTable
    PackedColumnTable(const QuerySchema &schema,
                      const SortDefinition &sort_def = SortDefinition()) :  QueryTable<Bit>(0, schema, sort_def) {
        setSchema(schema);

    }

    void appendColumnSetup(const QueryFieldDesc & desc) {
        int ordinal = desc.getOrdinal();
        assert(ordinal == this->schema_.getFieldCount());

        this->schema_.putField(desc);
        //this->schema_.initializeFieldOffsets();

        int field_size_bytes = desc.size() * sizeof(emp::Bit);
        tuple_size_bytes_ += field_size_bytes;
        field_sizes_bytes_[ordinal] = field_size_bytes;

        int size_threshold = page_size_bits_;
        int packed_wires = desc.size() / size_threshold + (desc.size() % size_threshold != 0);

        if(desc.size() / size_threshold > 0) {
            fields_per_page_[ordinal] = 1;
        }
        else {
            fields_per_page_[ordinal] = size_threshold / desc.size();
        }

        ordinal_offsets_[ordinal] = ordinal_offsets_[ordinal - 1] + PackedColumnTable::bytesPerColumn(schema_.getField(ordinal-1), tuple_cnt_);
        ordinal_offsets_[-1] = ordinal_offsets_[ordinal] + PackedColumnTable::bytesPerColumn(schema_.getField(ordinal), tuple_cnt_);
    }

    inline int getPageOffset(const PageId &pid) const {
        return ordinal_offsets_.at(pid.col_id_) + pid.page_idx_ * packed_wire_size_bytes_;
    }

    int8_t *getFieldPtr(const int & row, const int & col) const  override {
        PageId pid(table_id_, col, row / fields_per_page_.at(col));
        int8_t *ptr = bpm_.getPagePtr<Bit>(pid);
        int offset = (row % fields_per_page_.at(col)) * schema_.getField(col).size() * sizeof(emp::Bit);
        return ptr + offset;
    }

    void putTuple(const int &idx, const QueryTuple<Bit_T<OMPCWire<3>>> &tuple) override {
        for (int i  = 0; i < schema_.getFieldCount(); ++i) {
            auto f = tuple.getField(i);
            setField(idx, i, f);
        }
    }

    vector<int8_t> serialize() override {
        throw std::runtime_error("serialize not yet implemented");
    }


    inline void fopenPackedPagesFile() {
        if (std::filesystem::exists(table_file_name_)) {
            packed_pages_file_.open(table_file_name_, std::ios::in | std::ios::out | std::ios::binary);
        }
        else {
            packed_pages_file_.open(table_file_name_, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        }

        if (!packed_pages_file_.is_open()) {
            throw std::runtime_error(
                    std::string("Failed to open file '") + table_file_name_ + "': " + strerror(errno)
            );
        }
    }


};
} // namespace vaultdb
#endif

#endif
