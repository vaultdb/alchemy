#ifndef _BUFFERED_COLUMN_DATA_H_
#define _BUFFERED_COLUMN_DATA_H_
#include "query_table/query_table.h"
#include <filesystem>
#include "query_table/secure_tuple.h"
#include "util/operator_utilities.h"
#include "util/emp_manager/outsourced_mpc_manager.h"
#include <fstream>

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace vaultdb {
    class BufferedColumnTable : public QueryTable<Bit> {
        public:
            SystemConfiguration &conf_ = SystemConfiguration::getInstance();

            std::string table_file_name_; // file in the disk
            mutable fstream table_shares_file_; // file pointer for read and write


            BufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {}

            BufferedColumnTable(const BufferedColumnTable &src) : QueryTable<Bit>(src) {}

            virtual std::vector<emp::Bit> readPage(const PageId pid) const = 0;
            virtual std::vector<emp::Bit> readPage(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, fstream &src_data_file) const = 0;

            virtual std::vector<int8_t> serializeEMPBits(const PageId & pid) const = 0;
            virtual std::vector<int8_t> serializeEMPBits(const std::vector<emp::Bit> bits) const = 0;
            static BufferedColumnTable *deserialize(const TableMetadata & md, const vector<int> & ordinals, const int & limit = -1);

            void getPage(const PageId &pid, int8_t *bit) override {
                
            }

            void flushPage(const PageId &pid, int8_t *bit) override {
                
            }

            Field<Bit> getField(const int &row, const int &col) const override {
                return Field<Bit>();
            }

            inline void setField(const int &row, const int &col, const Field<Bit> &f) override {}

            SecureTable *secretShare() override  {
                assert(this->isEncrypted());
                throw; // can't secret share already encrypted table
            }

            void appendColumn(const QueryFieldDesc & desc) override {}

            void resize(const size_t &tuple_cnt) override {}

            Bit getDummyTag(const int & row)  const override {
                return emp::Bit(0);
            }

            void setDummyTag(const int & row, const Bit & val) override {}



            QueryTuple<Bit> getRow(const int & idx) override {
                return QueryTuple<Bit>();
            }

            void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {}

            void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {}

            void cloneTable(const int & dst_row, const int & dst_col, QueryTable<Bit> *src) override {}

            void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {}

            void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {}

            void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row, const int & copies) override {}

            void cloneColumn(const int & dst_col, const QueryTable<Bit> *src_table, const int &src_col) override {}

            void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {}

            StorageModel storageModel() const override { return StorageModel::OUTSOURCED_COLUMN_STORE; }

            void deserializeRow(const int & row, vector<int8_t> & src) override {}

            static inline long bytesPerColumn(const QueryFieldDesc & desc, const int & tuple_cnt) {
                return 0;
            }


            ~BufferedColumnTable() {}
    };
}

#else
namespace vaultdb {
    class BufferedColumnTable : public QueryTable<Bit> {
    public:
        SystemConfiguration &conf_ = SystemConfiguration::getInstance();
        int page_size_bits_ = bpm_.page_size_bytes_ / sizeof(Bit);

        std::string table_file_name_; // file in the disk
        mutable fstream table_shares_file_; // file pointer for read and write

        int emp_bit_disk_size_;

        std::map<int, int> fields_per_page_;
        std::map<int, int> pages_per_field_;
        std::map<int, int> pages_per_col_;
        map<int, long> ordinal_offsets_;

        OutsourcedMpcManager *manager_ = (OutsourcedMpcManager *) conf_.emp_manager_;

        BufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {
            assert(this->conf_.storageModel() == this->storageModel() && this->conf_.buffer_pool_enabled_);
            setSchema(schema);

            // set up file handle
            table_file_name_ = conf_.temp_db_path_ + "/table_" + std::to_string(table_id_) + "_shares.p" + std::to_string(conf_.party_);

            this->bpm_.registerTable(this);
        }

        BufferedColumnTable(const BufferedColumnTable &src) : QueryTable<Bit>(src) {
            assert(src.isEncrypted() && this->isEncrypted() && src.conf_.storageModel() == src.storageModel() && this->conf_.storageModel() == this->storageModel() && this->conf_.buffer_pool_enabled_);

            // set up file handle
            table_file_name_ = conf_.temp_db_path_ + "/table_" + std::to_string(table_id_) + "_shares.p" + std::to_string(conf_.party_);

        }

        virtual std::vector<emp::Bit> readPage(const PageId pid) const = 0;
        virtual std::vector<emp::Bit> readPage(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, fstream &src_data_file) const = 0;

        virtual std::vector<int8_t> serializeEMPBits(const PageId & pid) const = 0;
        virtual std::vector<int8_t> serializeEMPBits(const std::vector<emp::Bit> bits) const = 0;
        static BufferedColumnTable *deserialize(const TableMetadata & md, const vector<int> & ordinals, const int & limit = -1);

        void getPage(const PageId &pid, int8_t *dst) override {
            auto src_bytes = this->readPage(pid);
            memcpy(dst, src_bytes.data(), src_bytes.size() * sizeof(emp::Bit));
        }

        void flushPage(const PageId &pid,  int8_t *bits) override {
            assert(this->isEncrypted());

            int col = pid.col_id_;
            size_t remained_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * this->fields_per_page_[col];
            size_t writing_tuple_cnt = (remained_tuple_cnt < this->fields_per_page_[col]) ? remained_tuple_cnt : this->fields_per_page_[col];
            std::vector<emp::Bit> src_bits(bits, bits + writing_tuple_cnt * this->schema_.getField(col).size());
            std::vector<int8_t> write_buffer = serializeEMPBits(src_bits);

            size_t write_offset = this->ordinal_offsets_[col] + (int64_t) (pid.page_idx_ * this->fields_per_page_[col] * this->schema_.getField(col).size() * emp_bit_disk_size_);

            table_shares_file_.seekp(write_offset, ios::beg);
            table_shares_file_.write(reinterpret_cast<char*>(write_buffer.data()), write_buffer.size());
        }

        Field<Bit> getField(const int &row, const int &col) const override {
            assert(this->isEncrypted() && this->conf_.buffer_pool_enabled_);

            QueryFieldDesc desc = this->schema_.getField(col);

            int field_blocks = this->pages_per_field_.at(col);

            if(field_blocks == 1) {
                PageId pid(this->table_id_, col, row / this->fields_per_page_.at(col));
                emp::Bit *read_ptr = ((Bit *) bpm_.getPagePtr<Bit>(pid)) + (row % this->fields_per_page_.at(col)) * desc.size();
                return Field<Bit>::deserialize(desc, (int8_t *) read_ptr);
            }

            throw std::runtime_error("NYI");
        }

        inline void setField(const int &row, const int &col, const Field<Bit> &f) override {
            assert(this->isEncrypted() && this->conf_.buffer_pool_enabled_);

            QueryFieldDesc desc = this->schema_.getField(col);

            int field_blocks = this->pages_per_field_.at(col);

            if(field_blocks == 1) {
                int fields_per_page = this->fields_per_page_.at(col);
                PageId pid = bpm_.getPageId(this->table_id_, col, row, fields_per_page);
                emp::Bit *write_ptr = ((Bit *) bpm_.getPagePtr<Bit>(pid)) + (row % fields_per_page) * desc.size();
                f.serialize((int8_t *) write_ptr, f, desc);
                bpm_.markDirty(pid);
                return;
            }

            throw std::runtime_error("NYI");
        }

        SecureTable *secretShare() override  {
            assert(this->isEncrypted());
            throw; // can't secret share already encrypted table
        }

        void appendColumn(const QueryFieldDesc & desc) override {
            int ordinal = desc.getOrdinal();
            assert(ordinal == this->schema_.getFieldCount());

            this->schema_.putField(desc);
            //this->schema_.initializeFieldOffsets();

            int field_size_bytes = desc.size() * sizeof(emp::Bit);
            tuple_size_bytes_ += field_size_bytes;
            field_sizes_bytes_[ordinal] = field_size_bytes;

            int size_threshold = bpm_.page_size_bytes_ / sizeof(Bit); // bytes to bits
            int pages_per_field = desc.size() / size_threshold + (desc.size() % size_threshold != 0);

            if(desc.size() / size_threshold > 0) {
                fields_per_page_[ordinal] = 1;
                pages_per_field_[ordinal] = pages_per_field;
            }
            else {
                fields_per_page_[ordinal] = size_threshold / desc.size();
                pages_per_field_[ordinal] = 1;
            }

            pages_per_col_[ordinal] = tuple_cnt_ / fields_per_page_[ordinal] + (tuple_cnt_ % fields_per_page_[ordinal] != 0);

            ordinal_offsets_[ordinal] = ordinal_offsets_[ordinal-1] + BufferedColumnTable::bytesPerColumn(schema_.getField(ordinal-1), tuple_cnt_);
            ordinal_offsets_[-1] = ordinal_offsets_[ordinal] + BufferedColumnTable::bytesPerColumn(schema_.getField(ordinal), tuple_cnt_);

            vector<Bit> zero_page(bpm_.page_size_bytes_ / sizeof(Bit), Bit(0));
            vector<int8_t> serialized_zero_page = serializeEMPBits(zero_page);

            table_shares_file_.seekp(ordinal_offsets_[ordinal], ios::beg);
            for(int i = 0; i < pages_per_col_[ordinal]; ++i) {
                table_shares_file_.write(reinterpret_cast<char*>(serialized_zero_page.data()), serialized_zero_page.size());
            }

            table_shares_file_.flush();
        }

        void resize(const size_t &tuple_cnt) override {
            if(tuple_cnt == this->tuple_cnt_) return;

            bpm_.flushTable<Bit>(this->table_id_);
            table_shares_file_.flush();

            int old_tuple_cnt = this->tuple_cnt_;
            this->tuple_cnt_ = tuple_cnt;

            auto new_ordinal_offsets = QueryTable<Bit>::getOrdinalOffsets(schema_, tuple_cnt_);

            int old_dummy_page_cnt = old_tuple_cnt / fields_per_page_.at(-1) + (old_tuple_cnt % fields_per_page_.at(-1) != 0);
            int new_dummy_page_cnt = this->tuple_cnt_ / fields_per_page_.at(-1) + (this->tuple_cnt_ % fields_per_page_.at(-1) != 0);

            // create temp file
            string temp_file_name = table_file_name_ + ".temp";
            fstream temp_fp;
            temp_fp.open(temp_file_name.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

            vector<Bit> zero_page(bpm_.page_size_bytes_ / sizeof(Bit), Bit(0));
            vector<int8_t> serialized_zero_page = serializeEMPBits(zero_page);

            vector<Bit> one_page(bpm_.page_size_bytes_ / sizeof(Bit), Bit(1));
            vector<int8_t> serialized_one_page = serializeEMPBits(one_page);

            // adding rows
            if (old_tuple_cnt < this->tuple_cnt_) {
                for (int i = 0; i < schema_.getFieldCount(); ++i) {
                    int fields_per_page = fields_per_page_.at(i);
                    int old_page_cnt =
                            old_tuple_cnt / fields_per_page + (old_tuple_cnt % fields_per_page != 0);
                    int new_page_cnt = this->tuple_cnt_ / fields_per_page +
                                       (this->tuple_cnt_ % fields_per_page != 0);

                    temp_fp.seekp(new_ordinal_offsets[i], ios::beg);
                    for (int j = 0; j < old_page_cnt; ++j) {
                        vector<int8_t> bit_page = serializeEMPBits(PageId(this->table_id_, i, j));
                        temp_fp.write(reinterpret_cast<char*>(bit_page.data()), bit_page.size());
                    }

                    for (int j = old_page_cnt; j < new_page_cnt; ++j) {
                        temp_fp.write(reinterpret_cast<char*>(serialized_zero_page.data()), serialized_zero_page.size());
                    }
                }

                temp_fp.seekp(new_ordinal_offsets[-1], ios::beg);
                for (int j = 0; j < old_dummy_page_cnt; ++j) {
                    vector<int8_t> dummy_page = serializeEMPBits(PageId(this->table_id_, -1, j));
                    temp_fp.write(reinterpret_cast<char*>(dummy_page.data()), dummy_page.size());
                }

                for (int j = old_dummy_page_cnt; j < new_dummy_page_cnt; ++j) {
                    temp_fp.write(reinterpret_cast<char*>(serialized_one_page.data()), serialized_one_page.size());
                }
            } else {
                // remove rows
                for (int i = 0; i < schema_.getFieldCount(); ++i) {
                    int fields_per_page = fields_per_page_.at(i);
                    int new_page_cnt = this->tuple_cnt_ / fields_per_page +
                                       (this->tuple_cnt_ % fields_per_page != 0);

                    temp_fp.seekp(new_ordinal_offsets[i], ios::beg);
                    for (int j = 0; j < new_page_cnt; ++j) {
                        vector<int8_t> bit_page = serializeEMPBits(PageId(this->table_id_, i, j));
                        temp_fp.write(reinterpret_cast<char*>(bit_page.data()), bit_page.size());
                    }
                }

                temp_fp.seekp(new_ordinal_offsets[-1], ios::beg);
                int new_dummy_page_cnt =
                        this->tuple_cnt_ / fields_per_page_.at(-1) + (this->tuple_cnt_ % fields_per_page_.at(-1) != 0);
                for (int j = 0; j < new_dummy_page_cnt; ++j) {
                    vector<int8_t> dummy_page = serializeEMPBits(PageId(this->table_id_, -1, j));
                    temp_fp.write(reinterpret_cast<char*>(dummy_page.data()), dummy_page.size());
                }
            }

            // swap temp file with original
            temp_fp.flush();
            temp_fp.close();

            table_shares_file_.flush();
            table_shares_file_.close();

            string cmd = "mv " + temp_file_name + " " + table_file_name_;
            system(cmd.c_str());
            table_shares_file_.open(table_file_name_, std::ios::in | std::ios::out | std::ios::binary);

            if (!table_shares_file_.is_open()) {
                throw std::runtime_error(
                        std::string("In resize: Failed to open file '") + table_file_name_ + "': " + strerror(errno)
                );
            }

            this->ordinal_offsets_ = new_ordinal_offsets;

            // remained dummy tags in the old last page need to manually set to 1.
            int dummy_fields_per_page = fields_per_page_.at(-1);
            int old_last_dummy_tag_offset = old_tuple_cnt % dummy_fields_per_page;
            if(old_last_dummy_tag_offset != 0) {
                int new_last_dummy_offset = tuple_cnt_ % dummy_fields_per_page;
                bool isSameLastPage = old_dummy_page_cnt == new_dummy_page_cnt;
                int update_dummy_tags_cnt = (isSameLastPage ? new_last_dummy_offset : dummy_fields_per_page) - old_last_dummy_tag_offset;

                int last_dummy_page_offset = (old_dummy_page_cnt - 1) * dummy_fields_per_page + old_last_dummy_tag_offset;
                for (int i = 0; i < update_dummy_tags_cnt; ++i) {
                    setDummyTag(last_dummy_page_offset + i, Bit(true));
                }
            }
        }

        Bit getDummyTag(const int & row)  const override {
            assert(this->isEncrypted() && this->conf_.buffer_pool_enabled_);

            int fields_per_page = this->fields_per_page_.at(-1);
            PageId pid = bpm_.getPageId(this->table_id_, -1, row, fields_per_page);
            emp::Bit *read_ptr = ((Bit *) bpm_.getPagePtr<Bit>(pid)) + (row % fields_per_page);
            return *read_ptr;
        }

        void setDummyTag(const int & row, const Bit & val) override {
            assert(this->isEncrypted() && this->conf_.buffer_pool_enabled_);

            int fields_per_page = this->fields_per_page_.at(-1);
            PageId pid = bpm_.getPageId(this->table_id_, -1, row, fields_per_page);
            emp::Bit *write_ptr = ((Bit *) bpm_.getPagePtr<Bit>(pid)) + (row % fields_per_page);
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
            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                for(int j = 0; j < src->tuple_cnt_; ++j) {
                    setField(dst_row + j, dst_col + i, src->getField(j, i));
                    setDummyTag(dst_row + j, src->getDummyTag(j));
                }
            }
        }

        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<Bit> * src, const int & src_row) override {
            assert(src->storageModel() == StorageModel::COLUMN_STORE);

            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                Field<Bit> f = src->getField(src_row, i);
                this->setField(dst_row, dst_col + i, f);
            }

            this->setDummyTag(dst_row, src->getDummyTag(src_row));
        }

        void cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row) override {
            assert(src->storageModel() == StorageModel::COLUMN_STORE);

            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
                Integer dst_field_int = this->getField(dst_row, dst_col + i).getInt();
                Integer src_field_int = src->getField(src_row, i).getInt();

                Integer write_int = emp::If(write, src_field_int, dst_field_int);

                Field<Bit> write_field = Field<Bit>::deserialize(this->schema_.getField(dst_col + i), (int8_t *) write_int.bits.data());
                this->setField(dst_row, dst_col + i, write_field);
            }

            // copy the dummy tag
            Bit dst_dummy_tag = this->getDummyTag(dst_row);
            Bit src_dummy_tag = src->getDummyTag(src_row);
            this->setDummyTag(dst_row, emp::If(write, src_dummy_tag, dst_dummy_tag));
        }

        void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row, const int & copies) override {
            assert(src->storageModel() == StorageModel::COLUMN_STORE);
            BufferedColumnTable *src_table = (BufferedColumnTable *) src;

            for(int i = 0; i < src_table->getSchema().getFieldCount(); ++i) {
                assert(src_table->getSchema().getField(i).size() == this->getSchema().getField(dst_col + i).size());
            }

            int write_idx = dst_col;
            for(int i = 0; i < src_table->getSchema().getFieldCount(); ++i) {
                // Get src unpacked page and pin it.
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

        void cloneColumn(const int & dst_col, const QueryTable<Bit> *src_table, const int &src_col) override {
            assert(src_table->getSchema().getField(src_col) == this->schema_.getField(dst_col));

            if(this->tuple_cnt_ == src_table->tuple_cnt_ && src_table->storageModel() == StorageModel::COLUMN_STORE) {
                BufferedColumnTable *src = (BufferedColumnTable *) src_table;
                bpm_.flushColumn<Bit>(src->table_id_, src_col);

                // direct read/write from/to disk page by page
                for(int i = 0; i < src->pages_per_col_[src_col]; ++i) {
                    PageId src_pid(src->table_id_, src_col, i);
                    PageId dst_pid(this->table_id_, dst_col, i);
                    bpm_.clonePage<Bit>(src_pid, dst_pid);
                }

                return;
            }
            this->cloneColumn(dst_col, 0, src_table, src_col, 0);
        }

        void cloneColumn(const int & dst_col, const int & dst_row, const QueryTable<Bit> *src, const int & src_col, const int & src_row = 0) override {
            assert(src->getSchema().getField(src_col) == this->schema_.getField(dst_col));

            BufferedColumnTable *src_table = (BufferedColumnTable *) src;

            int rows_to_cp = src->tuple_cnt_ - src_row;
            if(rows_to_cp > this->tuple_cnt_ - dst_row) {
                rows_to_cp = this->tuple_cnt_ - dst_row;
            }

            int read_offset = src_row;
            int write_offset = dst_row;

            // this does not work 1:1 for copying whole wires
            // because we are pulling from arbitrary row offsets
            // and those row offsets might not align with page boundaries
            for(int i = 0; i < rows_to_cp; ++i) {
                Field<Bit> src_field = src_table->getField(read_offset, src_col);
                setField(write_offset, dst_col, src_field);
                ++read_offset;
                ++write_offset;
            }
        }

        StorageModel storageModel() const override { return StorageModel::OUTSOURCED_COLUMN_STORE; }

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


        ~BufferedColumnTable() {
            // Flush pages in buffer pool
            bpm_.removeTable<Bit>(this->table_id_);

            // close the file
            if (table_shares_file_.is_open() && table_shares_file_.good()) {
                table_shares_file_.close();
            }

            // remove the file
            string temp_dir = SystemConfiguration::getInstance().temp_db_path_;
            if (table_file_name_.substr(0, temp_dir.length()) == temp_dir) {
                Utilities::runCommand("rm " + table_file_name_);
            }
        }

        int8_t *getFieldPtr(const int &row, const int &col) const override {
            PageId pid(this->table_id_, col, row / this->fields_per_page_.at(col));
            int8_t *src = bpm_.getPagePtr<Bit>(pid);
            return src + (row % this->fields_per_page_.at(col)) * this->schema_.getField(col).size() * sizeof(emp::Bit);

        }

        void putTuple(const int &idx, const QueryTuple<Bit_T<OMPCWire<3>>> &tuple) override {
            assert(*tuple.getSchema() == this->schema_);

            for (int i = 0; i < this->schema_.getFieldCount(); ++i) {
                auto f = tuple.getField(i);
                this->setField(idx, i, f); // needed to maintain dirty bit in BPM
            }

            Bit dummy_tag = tuple.getDummyTag();
            this->setDummyTag(idx, dummy_tag);
        }

        vector<int8_t> serialize() override {
            throw std::runtime_error("not yet implemented");
        }

    };
}

#endif

#endif
