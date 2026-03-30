#ifndef _COLUMN_TABLE_BUFFER_FREE_H_
#define _COLUMN_TABLE_BUFFER_FREE_H_
#include "query_table/query_table.h"
#include <vector>
#include <filesystem>
#include <string>

#include "util/logger.h"

// used for initial secret sharing and wire packing in OMPC
// needed to keep TP in sync with other parties
// for packing and unpacking.  Otherwise, buffer pool works
// with a mixture of packed and unpacked pages
// and TP has to evict and pack prematurely
namespace vaultdb {

template<typename B>
class BufferFreeColumnTable : public QueryTable<B> {
public:
    string table_file_name_;
    mutable fstream table_file_handle_; // file pointer for read and write
    bool persistent_ = false; // if set to true, table will not be deleted automatically
    BufferPoolManager &bpm_ = SystemConfiguration::getInstance().bpm_;
    vector<int8_t> current_page_; // buffer for current page
    PageId current_page_id_ = INVALID_PAGE_ID; // invalid page id
    bool current_page_dirty_ = false;

    // empty sort definition for default case
    BufferFreeColumnTable<B>(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition())
            : QueryTable<B>(tuple_cnt, schema, sort_def), persistent_(false) { // default to temp if it has no filename

        table_file_name_ = SystemConfiguration::getInstance().temp_db_path_ + "/table_" + std::to_string(this->table_id_) + ".p" + std::to_string(SystemConfiguration::getInstance().party_);
        current_page_ = vector<int8_t>(SystemConfiguration::getInstance().bp_page_size_bytes_);
        setup();

    }

    BufferFreeColumnTable<B>(const size_t &tuple_cnt, const QuerySchema &schema, const string & filename, bool init_file, const SortDefinition &sort_def = SortDefinition())
            : QueryTable<B>(tuple_cnt, schema, sort_def), persistent_(true), table_file_name_(filename) { // default to persistent if it has a filename

        this->setSchema(schema);
        current_page_ = vector<int8_t>(SystemConfiguration::getInstance().bp_page_size_bytes_);

        if (table_file_name_.empty()) {
            table_file_name_ = SystemConfiguration::getInstance().temp_db_path_ + "/table_" + std::to_string(this->table_id_) + ".p" + std::to_string(SystemConfiguration::getInstance().party_);
        }

        if (init_file) {
            setup(); // start with a blank file
            return;
        }

        // else read in from previous file
        this->ordinal_offsets_ = QueryTable<B>::getOrdinalOffsets(this->schema_, this->tuple_cnt_);

        int fsize = this->ordinal_offsets_.at(-1) + Utilities::pageAlignedBytes(this->field_sizes_bytes_.at(-1), this->tuple_cnt_, bpm_.page_size_bytes_);

        // is this the right size for the tuple count?
        assert (std::filesystem::file_size(table_file_name_.c_str()) == fsize);

        table_file_handle_.open(table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);

        if (!table_file_handle_.is_open()) {
            throw std::runtime_error("ColumnTable cannot open table file to write at " + table_file_name_);
        }

    }

    explicit BufferFreeColumnTable<B>(const BufferFreeColumnTable<B> &src) : QueryTable<B>(src) {


        table_file_name_ = SystemConfiguration::getInstance().temp_db_path_ + "/table_" + std::to_string(this->table_id_) + ".p" + std::to_string(SystemConfiguration::getInstance().party_);
        current_page_ = vector<int8_t>(SystemConfiguration::getInstance().bp_page_size_bytes_);

        BufferFreeColumnTable<B> *src_table = const_cast<BufferFreeColumnTable<B> *>(&src);

        src_table->flushPage(src_table->current_page_id_, current_page_.data());

        std::filesystem::copy_file(src_table->table_file_name_.c_str(),
            table_file_name_.c_str(),
            std::filesystem::copy_options::overwrite_existing);

        src_table->table_file_handle_.open(src_table->table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);
        table_file_handle_.open(table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);

        if (!table_file_handle_.is_open()) {
            throw std::runtime_error("BufferFreeColumnTable copy constructor: Failed to open file '" + table_file_name_ + "': " + strerror(errno));
        }


    }

    ~BufferFreeColumnTable<B>()  override {
        bpm_.removeTable<B>(this->table_id_);
        if (table_file_handle_.is_open()) {
            table_file_handle_.close();
        }
        // if temp, remove the file
        string temp_dir = SystemConfiguration::getInstance().temp_db_path_;
        if (!persistent_) {
            filesystem::remove(table_file_name_.c_str());
        }


    }

    void getPage(const PageId &pid, int8_t *dst) override {
        if (current_page_id_ == pid) { // already cached it
            return;
        }

        flushPage(current_page_id_, current_page_.data());

        int read_offset = this->ordinal_offsets_.at(pid.col_id_) + pid.page_idx_ * bpm_.page_size_bytes_; // bytes

        table_file_handle_.clear();
        table_file_handle_.seekg(read_offset, ios::beg);
        table_file_handle_.clear();
        table_file_handle_.read(reinterpret_cast<char*>(dst), bpm_.page_size_bytes_);
        current_page_id_ = pid;
        current_page_dirty_ = false;

        if (!table_file_handle_.good()) {
            auto logger = Logging::get_log();
            logger->write("I/O error while reading page " + pid.toString());
            cout << "BufferFreeColumnTable reading page at offset: " << read_offset << " for pid: " << pid.toString() << ", read starts with " << DataUtilities::printByteArray(dst, 16) << '\n';
            cout << "Error bits: eof: " << std::ios_base::eofbit << ", failbit: " << std::ios_base::failbit << ", bad bit: " << std::ios::badbit << std::endl;

            table_file_handle_.clear();

        }
    }

    // for debugging only, bypasses the buffer pool manager
    Field<B> getFieldFromDisk(const int & row, const int & col) const {
        auto read_offset = this->ordinal_offsets_.at(col) + (row / this->fields_per_page_.at(col)) * bpm_.page_size_bytes_; // bytes from start of file
        table_file_handle_.seekg(read_offset, ios::beg);

        vector<int8_t> page(bpm_.page_size_bytes_);
        table_file_handle_.read(reinterpret_cast<char*>(page.data()), bpm_.page_size_bytes_);

        int8_t *src = page.data() + (row % this->fields_per_page_.at(col)) * this->field_sizes_bytes_.at(col);
        return Field<B>::deserialize(this->schema_.getField(col), src);
    }

    void flushPage(const PageId &pid, int8_t *src) override {

        if (!current_page_dirty_ || current_page_id_ == INVALID_PAGE_ID) return;

        int write_offset = this->ordinal_offsets_.at(pid.col_id_) + pid.page_idx_ * bpm_.page_size_bytes_; // bytes from start of file

        table_file_handle_.seekp(write_offset, ios::beg);
        table_file_handle_.clear();

        // serialize bits to write
        table_file_handle_.write(reinterpret_cast<char*>(src), bpm_.page_size_bytes_);

        if (!table_file_handle_.good()) {
            auto logger = Logging::get_log();
            logger->write("I/O error while writing page " + pid.toString());
            cout << "BufferFreeColumnTable flushing page at offset: " << write_offset << " for pid: " << pid.toString() << ", write starts with " << DataUtilities::printByteArray(src, 16) << '\n';
            cout << "Error bits: eof: " << std::ios_base::eofbit << ", failbit: " << std::ios_base::failbit << ", bad bit: " << std::ios::badbit << std::endl;
            table_file_handle_.clear();
        }

        table_file_handle_.flush();
        current_page_dirty_ = false;
    }



    Field<B> getField(const int  & row, const int & col)  const override {
        auto ptr = this->getFieldPtr(row, col);
        QueryFieldDesc desc = this->schema_.getField(col);
        return Field<B>::deserialize(desc, ptr);
    }



    void setField(const int &row, const int &col, const Field<B> &f) override {
        auto ptr = this->getFieldPtr(row, col);
        Field<B>::serialize(ptr, f, this->schema_.getField(col));
        current_page_dirty_ = true;
    }


    B getDummyTag(const int &row) const override {
        B *ptr =  (B *) this->getFieldPtr(row, -1);
        return *ptr;
    }

    void setDummyTag(const int & row, const B & val) override {
        B *ptr =  (B *) this->getFieldPtr(row, -1);
        *ptr = val;
        current_page_dirty_ = true;
    }


     int8_t *getFieldPtr(const int & row, const int & col) const override {
        PageId pid{this->table_id_, col, row / this->fields_per_page_.at(col)};
        int8_t *dst = const_cast<int8_t *>(current_page_.data());
        const_cast<BufferFreeColumnTable *>(this)->getPage(pid, dst);
        auto src = current_page_.data();
        return const_cast<int8_t *>(src + (row % this->fields_per_page_.at(col)) * this->field_sizes_bytes_.at(col));
    }

    QueryTable<B> *clone()  override {
        return  new BufferFreeColumnTable<B>(*this);

    }


    void cloneColumn(const int &dst_col, const QueryTable<B> *src_table, const int &src_col)  override {

        assert(src_table->getSchema().getField(src_col) == this->schema_.getField(dst_col));

        if(this->tuple_cnt_ == src_table->tuple_cnt_ && src_table->storageModel() == StorageModel::BUFFER_FREE_COLUMN_STORE) {

            BufferFreeColumnTable *src = (BufferFreeColumnTable *) src_table;
            auto page_cnt = Utilities::getPageCount(src->field_sizes_bytes_.at(src_col), src->tuple_cnt_, bpm_.page_size_bytes_);

            for(int i = 0; i < page_cnt; ++i) {
                PageId src_pid(src->table_id_, src_col, i);
                PageId dst_pid(this->table_id_, dst_col, i);
                src->getPage(src_pid, src->current_page_.data());
                auto src_ptr = src->current_page_.data();

                this->getPage(dst_pid, this->current_page_.data());
                auto dst_ptr = this->current_page_.data();
                memcpy(dst_ptr, src_ptr, bpm_.page_size_bytes_);
                this->current_page_dirty_ = true;
                this->flushPage(dst_pid, this->current_page_.data());
            }

            return;
        }
        // otherwise, copy column data field by field from src to dst
        this->cloneColumn(dst_col, 0, src_table, src_col, 0);

    }


    // no guarantee copies are page-aligned, so doing this one field at a time
    void cloneColumn(const int &dst_col, const int &dst_row, const QueryTable<B> *src, const int &src_col, const int &src_row) override {

        assert(this->getSchema().getField(dst_col) == src->getSchema().getField(src_col));
        assert(src->storageModel() == StorageModel::BUFFER_FREE_COLUMN_STORE);

        // TODO: do this with file handles outside of BPM
        // similar to PackedColumnTable copy constructor
        if (src_row == dst_row && dst_row == 0 && src->tuple_cnt_ == this->tuple_cnt_) {
            // if we are cloning the first row and both tables have the same tuple count, we can clone the entire column a page at a time.
            this->cloneColumn(dst_col, src, src_col);
            return;
        }

        // otherwise, copy column data field by field from src to dst)

        auto src_tuples = src->tuple_cnt_ - src_row;
        if(src_tuples > (this->tuple_cnt_ - dst_row)) {
            src_tuples = this->tuple_cnt_ - dst_row; // truncate to our available slots
        }

        for (int i = 0; i < src_tuples; ++i) { // move onto the next page in our scan
            Field<B> f = src->getField(src_row + i, src_col);
            this->setField(dst_row + i, dst_col, f);
        }

    }



    vector<int8_t> serialize() override {
        // serializing everything in one go, not page-aligned.
        // doing it this way to save on space and to make it easy to deduce later how many rows we have without additional metadata

        if (this->current_page_dirty_) flushPage(this->current_page_id_, this->current_page_.data());

        long table_bytes = 0;
        for (int i = -1; i < this->schema_.getFieldCount(); ++i) {
            table_bytes += this->field_sizes_bytes_.at(i) * this->tuple_cnt_;
        }

        vector<int8_t> serialized( table_bytes );
        int8_t *dst = serialized.data();

        for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
            int field_size_bytes = this->field_sizes_bytes_.at(i);
            for (int j = 0; j < this->tuple_cnt_; ++j) {
                auto ptr = this->getFieldPtr(j, i);
                memcpy(dst, ptr, field_size_bytes);
                dst += field_size_bytes;
            }
        }

        // dummy tag
        int field_size_bytes = this->field_sizes_bytes_.at(-1);
        for (int j = 0; j < this->tuple_cnt_; ++j) {
            auto ptr = this->getFieldPtr(j, -1);
            memcpy(dst, ptr, field_size_bytes);
            dst += field_size_bytes;
        }

        return serialized;
    }

    void putTuple(const int &idx, const QueryTuple<B> &tuple) override {
        assert(*tuple.getSchema() == this->schema_);

        for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
            auto f = tuple.getField(i);
            this->setField(idx, i, f); // needed to maintain dirty bit in BPM
        }

        B dummy_tag = tuple.getDummyTag();
        this->setDummyTag(idx, dummy_tag);

    }



    void resize(const size_t &new_tuple_cnt) override {
        if(new_tuple_cnt == this->tuple_cnt_) return;

        table_file_handle_.flush();

        if (new_tuple_cnt == 0) {
            table_file_handle_.close();
            std::filesystem::remove(table_file_name_.c_str());
            this->tuple_cnt_ = 0;
            this->ordinal_offsets_  = QueryTable<B>::getOrdinalOffsets(this->schema_, this->tuple_cnt_);
            table_file_handle_.open(table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
            if (!table_file_handle_.is_open()) {
                throw std::runtime_error(
                        std::string("In resize: Failed to open file '") + table_file_name_ + "': " + strerror(errno)
                );
            }

            return;
        }

        auto new_ordinal_offsets = QueryTable<B>::getOrdinalOffsets(this->schema_, new_tuple_cnt);

        // create temp file
        string temp_file_name = table_file_name_ + ".temp";

        Utilities::runCommand("touch " + temp_file_name);
        int fsize = new_ordinal_offsets.at(-1) + Utilities::pageAlignedBytes(this->field_sizes_bytes_.at(-1), new_tuple_cnt, bpm_.page_size_bytes_);
        std::filesystem::resize_file(temp_file_name.c_str(),  fsize);


        fstream temp_fp;
        temp_fp.open(temp_file_name.c_str(),  std::ios::out | std::ios::binary);

        bool growing = (new_tuple_cnt > this->tuple_cnt_);

        int table_pages = (fsize / bpm_.page_size_bytes_);
        for (int i = -1; i < this->schema_.getFieldCount(); ++i) {
            int fields_per_page = this->fields_per_page_.at(i);
            int write_page_cnt = (growing) ? this->tuple_cnt_ / fields_per_page + (this->tuple_cnt_ % fields_per_page != 0)
                                            : new_tuple_cnt / fields_per_page + (new_tuple_cnt % fields_per_page != 0); // if shrinking, only copy the first dst_page pages

            temp_fp.clear();
            temp_fp.seekp(new_ordinal_offsets[i], ios::beg);
            for (int j = 0; j < write_page_cnt; ++j) {
                PageId pid{this->table_id_, i, j};
                this->getPage(pid, current_page_.data());
                auto src = current_page_.data();
                temp_fp.write((char *) src, bpm_.page_size_bytes_);
            }

        }

        if (growing) {
            // seek to the last page we wrote to with the old tuple count
            int last_offset = new_ordinal_offsets.at(-1) + Utilities::pageAlignedBytes(this->field_sizes_bytes_.at(-1), this->tuple_cnt_, bpm_.page_size_bytes_);

            temp_fp.clear();
            temp_fp.seekp(last_offset, ios::beg);

            // pad the remaining pages with dummy tag = 1
            int pages_remaining = table_pages - (last_offset / bpm_.page_size_bytes_);

            vector<int8_t> dummy_page(bpm_.page_size_bytes_);

            if (std::is_same_v<B, bool>) {
                memset(dummy_page.data(), 1, bpm_.page_size_bytes_);
            } else {
                Bit *dummy_ptr = reinterpret_cast<Bit *>(dummy_page.data());
                Bit dummy_out = Bit(1);

                for (int k = 0; k < this->fields_per_page_.at(-1); ++k) {
                    *dummy_ptr = dummy_out;
                    ++dummy_ptr;
                }


            }

            for (int k = 0; k < pages_remaining; ++k) {
               temp_fp.write((char *) dummy_page.data(), bpm_.page_size_bytes_);
            }

        }


        this->tuple_cnt_ = new_tuple_cnt;
        this->ordinal_offsets_ = new_ordinal_offsets;

        Logging::Logger* log = Logging::get_log();

        // rename temp file to original file
        if (!temp_fp.good()) {
            log->write("Bad write to temp file: " + temp_file_name + '\n');
            temp_fp.clear();
        }

        temp_fp.flush();
        temp_fp.close();


        table_file_handle_.flush();
        table_file_handle_.close();

        std::filesystem::remove(table_file_name_.c_str());
        std::filesystem::rename(temp_file_name.c_str(), table_file_name_.c_str());

        // cout << "Input file size: " << Utilities::runCommand("stat -f%z " + table_file_name_);
        table_file_handle_.open(table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);

        if (!table_file_handle_.is_open()) {
            throw std::runtime_error(
                    std::string("In resize: Failed to open file '") + table_file_name_ + "': " + strerror(errno)
            );
        }

    }

    QueryTuple<B> getRow(const int & idx) override {
        QueryTuple<B>  row(&this->schema_);
        int8_t *write_pos = row.getData();

        for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
            auto read_pos = getFieldPtr(idx, i);
            memcpy(write_pos, read_pos, this->field_sizes_bytes_[i]);
            write_pos += this->field_sizes_bytes_[i];
        }
        memcpy(write_pos, getFieldPtr(idx, -1), this->field_sizes_bytes_[-1]);

        return row;
    }

    void setRow(const int & idx, const QueryTuple<B> &tuple) override;

    void compareSwap(const B & swap, const int  & lhs_row, const int & rhs_row) override;

    void appendColumn(const QueryFieldDesc & desc) override {
        int new_ordinal = desc.getOrdinal();
        assert(new_ordinal == this->schema_.getFieldCount());

        int dummy_page_cnt = this->tuple_cnt_ / this->fields_per_page_.at(-1) + (this->tuple_cnt_ % this->fields_per_page_.at(-1) != 0);

        auto temp = this->getSchema();
        temp.putField(desc);
        temp.initializeFieldOffsets();
        auto new_ordinal_offsets = QueryTable<B>::getOrdinalOffsets(temp, this->tuple_cnt_);

        // first, close and resize the file
        flushPage(this->current_page_id_, current_page_.data());
        table_file_handle_.flush();
        table_file_handle_.close();

        int new_fsize = new_ordinal_offsets[-1] + Utilities::pageAlignedBytes(this->field_sizes_bytes_.at(-1), this->tuple_cnt_, bpm_.page_size_bytes_);
        std::filesystem::resize_file(table_file_name_.c_str(),  new_fsize);

        table_file_handle_.open(table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);

        int new_dummy_tag_offset = new_ordinal_offsets[-1];

        for (int i = 0; i < dummy_page_cnt; ++i) {
            PageId pid(this->table_id_, -1, i);
            this->getPage(pid, current_page_.data());
            auto page_ptr = current_page_.data();
            table_file_handle_.seekp(new_dummy_tag_offset, ios::beg);
            table_file_handle_.write(reinterpret_cast<char*>(page_ptr), bpm_.page_size_bytes_);
            new_dummy_tag_offset += bpm_.page_size_bytes_;
        }

        this->setSchema(temp);

        auto new_field_size_bytes = this->field_sizes_bytes_.at(new_ordinal); //  (std::is_same_v<B, bool>) ? desc.size() / 8 : desc.size() * sizeof(emp::Bit);
        this->ordinal_offsets_ = new_ordinal_offsets;


        // insert zeros for new column
        table_file_handle_.seekp(this->ordinal_offsets_[new_ordinal], ios::beg);
        int write_size_bytes = this->ordinal_offsets_[-1] - this->ordinal_offsets_[new_ordinal]; // new dummy offset vs old one

        table_file_handle_.clear();

        char zero = 0;
        for (int i = 0; i < write_size_bytes; ++i) {
            table_file_handle_.write(&zero, 1); // write a single byte
        }

    }

// up to two-way secret share - both Alice and Bob providing private inputs
    SecureTable *secretShare() override  {
        assert(!this->isEncrypted());
        SystemConfiguration & conf = SystemConfiguration::getInstance();
        return conf.emp_manager_->secretShare(reinterpret_cast<PlainTable *>(this));
    }


    // copy all columns from src to dst
    void cloneTable(const int & dst_row,  const int & dst_col, QueryTable<B> *s) override {

        assert((s->tuple_cnt_ + dst_row) <= this->tuple_cnt_);
        assert(s->storageModel() == StorageModel::BUFFER_FREE_COLUMN_STORE);

        // don't mess with pages for now, just use getters and setters
        // TODO: batch these page-wise to avoid overhead of many get/set calls
        auto src = (BufferFreeColumnTable<B> *) s;

        for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
            this->cloneColumn(dst_col + i, dst_row, src, i, 0);;
        }

        // copy dummy tag
        this->cloneColumn(-1, dst_row, src, -1, 0);

    }

    // dummy tag included
    void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * s, const int & src_row) override{

        assert(s->storageModel() == StorageModel::BUFFER_FREE_COLUMN_STORE);
        auto src = (BufferFreeColumnTable<B> *) s;

        for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
            auto f = src->getField(src_row, i);
            this->setField(dst_row, dst_col + i, f);
        }

        B dummy_tag = src->getDummyTag(src_row);
        this->setDummyTag(dst_row, dummy_tag);
    }

    void cloneRow(const B & write, const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row) override;

    // make N copies of a row, starting at offset dst_row
    void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<B> *s, const int & src_row, const int & copies) override {
        assert(s->storageModel() == StorageModel::BUFFER_FREE_COLUMN_STORE);
        for(int i = 0; i < s->getSchema().getFieldCount(); ++i) {
            assert(s->getSchema().getField(i).size() == this->getSchema().getField(dst_col + i).size());
        }

        auto src = (BufferFreeColumnTable<B> *) s;

        int write_col = dst_col; // field indexes

        for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
            // TODO: adapt this for writing directly to pages for bulk writes
            auto src_field = src->getField(src_row, i);
            assert(src->getSchema().getField(i) == this->getSchema().getField(dst_col + i));
            int write_row = dst_row;
            for (int j = 0; j < copies; ++j) {
                this->setField(write_row, write_col, src_field);
                ++write_row;
            }
            ++write_col;

        }

        // copy dummy tag
        B dummy_tag = src->getDummyTag(src_row);
        int to_write_row = dst_row;
        for (int i = 0; i < copies; ++i) {
            setField(to_write_row, -1, dummy_tag);
            ++to_write_row;
        }

    }




    StorageModel storageModel() const override { return StorageModel::BUFFER_FREE_COLUMN_STORE; }


private:
    // need static context to avoid needlessly invoking default constructors for Bits

    vector<int8_t> unpackRowBytes(const int & row, const int & col_cnt) const {
        int read_len = 0;
        for(int i = 0; i < col_cnt; ++i) {
            read_len += this->field_sizes_bytes_.at(i);
        }

        vector<int8_t> dst(read_len);
        int8_t *cursor = dst.data();

        for(int i = 0; i < col_cnt; ++i) {
            int write_len = this->field_sizes_bytes_.at(i);
            memcpy(cursor, getFieldPtr(row, i), write_len);
            cursor += write_len;
        }

        return dst;

    }

    // unpack entire row
    vector<int8_t> unpackRowBytes(const int & row) const {
        vector<int8_t> dst(this->tuple_size_bytes_);
        int col_cnt = this->schema_.getFieldCount();
        int8_t *cursor = dst.data();

        for(int i = 0; i < col_cnt; ++i) {
            int write_len = this->field_sizes_bytes_.at(i);
            memcpy(cursor, getFieldPtr(row, i), write_len);
            cursor += write_len;
        }

        // copy dummy tag to last slot
        memcpy(cursor, getFieldPtr(row, -1), this->field_sizes_bytes_.at(-1));
        return dst;
    }




    void writeToFile(const string &fq_filename) const override {
        fstream dst_file(fq_filename, std::ios::out | std::ios::binary);

        int page_cnt;
        for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
             page_cnt = Utilities::getPageCount(this->field_sizes_bytes_.at(i), this->tuple_cnt_, bpm_.page_size_bytes_);

            for (int j = 0; j < page_cnt; ++j) {
                PageId pid(this->table_id_, i, j);
                int8_t *src_data = const_cast<int8_t *>(current_page_.data());
                const_cast<BufferFreeColumnTable *>(this)->getPage(pid, src_data);
                char *src = (char *) current_page_.data();
                dst_file.write(src, bpm_.page_size_bytes_);
            }
        }

        // dummy tag last
        page_cnt = Utilities::getPageCount(this->field_sizes_bytes_.at(-1), this->tuple_cnt_, bpm_.page_size_bytes_);

        for (int j = 0; j < page_cnt; ++j) {
            PageId pid(this->table_id_, -1, j);
            int8_t *src_data = const_cast<int8_t *>(current_page_.data());
            const_cast<BufferFreeColumnTable *>(this)->getPage(pid, src_data);
            char *src = (char *) current_page_.data();
            dst_file.write(src, bpm_.page_size_bytes_);
        }

        dst_file.flush();
        dst_file.sync();
        dst_file.close();
        if (!dst_file.good()) {
            throw std::runtime_error(
                    std::string("In writeToFile: Failed to write to file '") + fq_filename + "': " + strerror(errno)
            );
        }

    }

    void markDirty(const int & row, const int & col)  {
        PageId pid(this->table_id_, col, row / this->fields_per_page_.at(col));
        if (pid == current_page_id_)
            current_page_dirty_ = true;
        // otherwise already flushed and not possible to dirty
    }


    void setup() {

        this->ordinal_offsets_ = QueryTable<B>::getOrdinalOffsets(this->schema_, this->tuple_cnt_);
        Utilities::runCommand("touch " + table_file_name_);
        int fsize = this->ordinal_offsets_.at(-1) + Utilities::pageAlignedBytes(this->field_sizes_bytes_.at(-1), this->tuple_cnt_, bpm_.page_size_bytes_);
        if (fsize == 0)
            return; // nothing to initialize

        std::filesystem::resize_file(table_file_name_.c_str(),  fsize);


        table_file_handle_.open(table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);


        if (!table_file_handle_.is_open()) {
            table_file_handle_.clear();
            // create a new file
            table_file_handle_.open(table_file_name_, std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
            if (!table_file_handle_.is_open()) {
                throw std::runtime_error("BufferFreeColumnTable cannot open table file to write at " + table_file_name_);
            }
        }

        if(this->tuple_cnt_ == 0)
            return;

        vector<int8_t> zero_page(bpm_.page_size_bytes_);
        vector<int8_t> one_page(bpm_.page_size_bytes_);

        if (std::is_same_v<B, bool>) {
            memset(zero_page.data(), 0, bpm_.page_size_bytes_);
            memset(one_page.data(), 1, bpm_.page_size_bytes_);
        } else {
            Bit zero(false);
            Bit one(true);
            Bit *zero_ptr = reinterpret_cast<Bit *>(zero_page.data());
            Bit *one_ptr = reinterpret_cast<Bit *>(one_page.data());
            // populate entire page
            auto to_write = bpm_.page_size_bytes_ / sizeof(Bit);
            for (int i = 0; i < to_write; ++i) {
                *zero_ptr = zero;
                *one_ptr = one;
                ++zero_ptr;
                ++one_ptr;
            }
        }

        auto zero_page_cnt = this->ordinal_offsets_.at(-1) / bpm_.page_size_bytes_;
        for (int i = 0; i < zero_page_cnt; ++i) {
            table_file_handle_.write(reinterpret_cast<char *>(zero_page.data()), bpm_.page_size_bytes_);
        }

        auto dummy_page_cnt = Utilities::getPageCount(this->field_sizes_bytes_.at(-1), this->tuple_cnt_, bpm_.page_size_bytes_);
        for (int i = 0; i < dummy_page_cnt; ++i) {
            table_file_handle_.write(reinterpret_cast<char *>(one_page.data()), bpm_.page_size_bytes_);
        }

        table_file_handle_.flush();
    }

};
} // namespace vaultdb


#endif
