#ifndef _BUFFERED_COLUMN_DATA_H_
#define _BUFFERED_COLUMN_DATA_H_
#include "query_table/query_table.h"
#include <filesystem>

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace vaultdb {
    class BufferedColumnTable : public QueryTable<Bit> {
        public:
            SystemConfiguration &conf_ = SystemConfiguration::getInstance();
            int table_id_ = conf_.num_tables_++;
            std::string file_path_ = this->conf_.stored_db_path_ + "/table_" + std::to_string(this->table_id_);
            std::string secret_shares_path_ = this->file_path_ + "/table_" + std::to_string(this->table_id_) + "." + std::to_string(this->conf_.party_);
            int current_emp_bit_size_on_disk_ = 0;
            std::map<int, int64_t> serialized_col_bytes_offsets_on_disk_;

            std::map<int, int> fields_per_page_;
            std::map<int, int> pages_per_field_;
            std::map<int, int> pages_per_col;

            BufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {}

            BufferedColumnTable(const BufferedColumnTable &src) : QueryTable<Bit>(src) {}

            std::vector<emp::Bit> getPage(const PageId &pid) override {
                return std::vector<emp::Bit>();
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

            QueryTable<Bit> *clone() override {
                return new BufferedColumnTable(*this);
            }

            void setSchema(const QuerySchema &schema) override {}

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

            StorageModel storageModel() const override { return StorageModel::COLUMN_STORE; }

            void deserializeRow(const int & row, vector<int8_t> & src) override {}

            std::vector<emp::Bit> readSecretSharedPageFromDisk(const PageId pid);

            std::vector<emp::Bit> readSecretSharedPageFromDisk(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, const string &src_data_path);

            std::vector<int8_t> convertEMPBitToWriteBuffer(const std::vector<emp::Bit> bits);

            void writePageToDisk(const PageId &pid, const emp::Bit *bits);

            ~BufferedColumnTable() {}
    };
}

#else
namespace vaultdb {
    class BufferedColumnTable : public QueryTable<Bit> {
    public:
        SystemConfiguration &conf_ = SystemConfiguration::getInstance();
        int table_id_ = conf_.num_tables_++;
        std::string file_path_ = this->conf_.stored_db_path_ + "/table_" + std::to_string(this->table_id_);
        std::string secret_shares_path_ = this->file_path_ + "/table_" + std::to_string(this->table_id_) + "." + std::to_string(this->conf_.party_);
        int current_emp_bit_size_on_disk_ = (this->conf_.party_ == 1 ? empBitSizesInPhysicalBytes::evaluator_disk_size_ : (this->conf_.party_ == 10086 ? 1 : empBitSizesInPhysicalBytes::garbler_disk_size_));
        std::map<int, int64_t> serialized_col_bytes_offsets_on_disk_; // bytes offsets for each column in the serialized file

        std::map<int, int> fields_per_page_;
        std::map<int, int> pages_per_field_;
        std::map<int, int> pages_per_col;

        BufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<Bit>(tuple_cnt, schema, sort_def) {
            assert(this->conf_.storageModel() == this->storageModel() && this->conf_.bp_enabled_);
            setSchema(schema);

            if(this->tuple_cnt_ == 0) {
                return;
            }

            std::filesystem::remove_all(this->secret_shares_path_);
            Utilities::mkdir(this->file_path_);

            // initialize the secret shares file with zeros
            std::vector<int8_t> initialized_bytes(this->tuple_cnt_ * this->schema_.size() * current_emp_bit_size_on_disk_, 0);
            DataUtilities::writeFile(this->secret_shares_path_, initialized_bytes);

            this->conf_.bpm_.registerTable(this->table_id_, (QueryTable<Bit> *) this);
        }

        BufferedColumnTable(const BufferedColumnTable &src) : QueryTable<Bit>(src) {
            assert(src.isEncrypted() && this->isEncrypted() && src.conf_.storageModel() == src.storageModel() && this->conf_.storageModel() == this->storageModel() && this->conf_.bp_enabled_);
            setSchema(src.schema_);

            if(src.tuple_cnt_ == 0) {
                return;
            }

            std::filesystem::remove_all(this->secret_shares_path_);
            Utilities::mkdir(this->file_path_);

            this->conf_.bpm_.flushTable(src.table_id_);

            std::filesystem::copy_file(src.secret_shares_path_, this->secret_shares_path_, std::filesystem::copy_options::overwrite_existing);

            this->conf_.bpm_.registerTable(this->table_id_, (QueryTable<Bit> *) this);
        }

        std::vector<emp::Bit> getPage(const PageId &pid) override {
            return this->readSecretSharedPageFromDisk(pid);
        }

        Field<Bit> getField(const int &row, const int &col) const override {
            assert(this->isEncrypted() && this->conf_.bp_enabled_);

            QueryFieldDesc desc = this->schema_.getField(col);

            int field_blocks = this->pages_per_field_.at(col);

            if(field_blocks == 1) {
                PageId pid(this->table_id_, col, row / this->fields_per_page_.at(col));
                emp::Bit *read_ptr = this->conf_.bpm_.getUnpackedPagePtr(pid) + (row % this->fields_per_page_.at(col)) * desc.size();
                return Field<Bit>::deserialize(desc, (int8_t *) read_ptr);
            }

            throw std::runtime_error("NYI");
        }

        inline void setField(const int &row, const int &col, const Field<Bit> &f) override {
            assert(this->isEncrypted() && this->conf_.bp_enabled_);

            QueryFieldDesc desc = this->schema_.getField(col);

            int field_blocks = this->pages_per_field_.at(col);

            if(field_blocks == 1) {
                int fields_per_page = this->fields_per_page_.at(col);
                PageId pid = this->conf_.bpm_.getPageId(this->table_id_, col, row, fields_per_page);
                emp::Bit *write_ptr = this->conf_.bpm_.getUnpackedPagePtr(pid) + (row % fields_per_page) * desc.size();
                f.serialize((int8_t *) write_ptr, f, desc);
                this->conf_.bpm_.markDirty(pid);
                return;
            }

            throw std::runtime_error("NYI");
        }

        SecureTable *secretShare() override  {
            assert(this->isEncrypted());
            throw; // can't secret share already encrypted table
        }

        void appendColumn(const QueryFieldDesc & desc) override {}

        void resize(const size_t &tuple_cnt) override {}

        Bit getDummyTag(const int & row)  const override {
            assert(this->isEncrypted() && this->conf_.bp_enabled_);

            int fields_per_page = this->fields_per_page_.at(-1);
            PageId pid = this->conf_.bpm_.getPageId(this->table_id_, -1, row, fields_per_page);
            emp::Bit *read_ptr = this->conf_.bpm_.getUnpackedPagePtr(pid) + (row % fields_per_page);
            return *read_ptr;
        }

        void setDummyTag(const int & row, const Bit & val) override {
            assert(this->isEncrypted() && this->conf_.bp_enabled_);

            int fields_per_page = this->fields_per_page_.at(-1);
            PageId pid = this->conf_.bpm_.getPageId(this->table_id_, -1, row, fields_per_page);
            emp::Bit *write_ptr = this->conf_.bpm_.getUnpackedPagePtr(pid) + (row % fields_per_page);
            *write_ptr = val;
            this->conf_.bpm_.markDirty(pid);
        }

        QueryTable<Bit> *clone() override {
            return new BufferedColumnTable(*this);
        }

        void setSchema(const QuerySchema &schema) override {
            this->schema_ = schema;
            this->plain_schema_ = QuerySchema::toPlain(schema);

            this->tuple_size_bytes_ = 0;

            for(int i = -1; i < this->schema_.getFieldCount(); ++i) {
                QueryFieldDesc desc = this->schema_.getField(i);
                int field_size_bytes = desc.size() * this->current_emp_bit_size_on_disk_;

                this->tuple_size_bytes_ += field_size_bytes;
                this->field_sizes_bytes_[i] = field_size_bytes;

                int bits_per_page = this->conf_.bpm_.unpacked_page_size_bits_;

                if(desc.size() / bits_per_page > 0) {
                    this->fields_per_page_[i] = 1;
                    this->pages_per_field_[i] = desc.size() / bits_per_page + (desc.size() % bits_per_page != 0);
                }
                else {
                    this->fields_per_page_[i] = bits_per_page / desc.size();
                    this->pages_per_field_[i] = 1;
                }

                this->pages_per_col[i] = this->tuple_cnt_ / this->fields_per_page_[i] + (this->tuple_cnt_ % this->fields_per_page_[i] != 0);
            }

            // calculate offsets for each column in the serialized file
            int64_t col_bytes_cnt = 0L;
            this->serialized_col_bytes_offsets_on_disk_[0] = 0;
            for (int i = 1; i < this->schema_.getFieldCount(); ++i) {
                col_bytes_cnt += this->schema_.getField(i - 1).size() * this->tuple_cnt_ * current_emp_bit_size_on_disk_;
                this->serialized_col_bytes_offsets_on_disk_[i] = col_bytes_cnt;
            }
            col_bytes_cnt += this->schema_.getField(this->schema_.getFieldCount() - 1).size() * this->tuple_cnt_ * current_emp_bit_size_on_disk_;
            this->serialized_col_bytes_offsets_on_disk_[-1] = col_bytes_cnt;
        }

        QueryTuple<Bit> getRow(const int & idx) override {
            return QueryTuple<Bit>();
        }

        void setRow(const int & idx, const QueryTuple<Bit> &tuple) override {}

        void compareSwap(const Bit & swap, const int  & lhs_row, const int & rhs_row) override {}

        void cloneTable(const int & dst_row, const int & dst_col, QueryTable<Bit> *src) override {}

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

        void cloneRowRange(const int & dst_row, const int & dst_col, const QueryTable<Bit> *src, const int & src_row, const int & copies) override {}

        void cloneColumn(const int & dst_col, const QueryTable<Bit> *src_table, const int &src_col) override {
            assert(src_table->getSchema().getField(src_col) == this->schema_.getField(dst_col));

            if(this->tuple_cnt_ == src_table->tuple_cnt_ && src_table->storageModel() == StorageModel::COLUMN_STORE) {
                BufferedColumnTable *src = (BufferedColumnTable *) src_table;
                this->conf_.bpm_.flushColumn(src->table_id_, src_col);

                // direct read/write from/to disk page by page
                for(int i = 0; i < src->pages_per_col[src_col]; ++i) {
                    PageId src_pid(src->table_id_, src_col, i);
                    PageId dst_pid(this->table_id_, dst_col, i);
                    this->conf_.bpm_.clonePage(src_pid, dst_pid);
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

            for(int i = 0; i < rows_to_cp; i = i + src_table->fields_per_page_.at(src_col)) {
                PageId src_pid = this->conf_.bpm_.getPageId(src_table->table_id_, src_col, read_offset, src_table->fields_per_page_[dst_col]);
                PageId dst_pid = this->conf_.bpm_.getPageId(this->table_id_, dst_col, write_offset, this->fields_per_page_[dst_col]);

                this->conf_.bpm_.clonePage(src_pid, dst_pid);
                read_offset += src_table->fields_per_page_[src_col];
                write_offset += this->fields_per_page_[dst_col];
            }
        }

        StorageModel storageModel() const override { return StorageModel::COLUMN_STORE; }

        void deserializeRow(const int & row, vector<int8_t> & src) override {}

        std::vector<emp::Bit> readSecretSharedPageFromDisk(const PageId pid);

        std::vector<emp::Bit> readSecretSharedPageFromDisk(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, const string &src_data_path);

        std::vector<int8_t> convertEMPBitToWriteBuffer(const std::vector<emp::Bit> bits);

        void writePageToDisk(const PageId &pid, const emp::Bit *bits);

        ~BufferedColumnTable() {
            this->conf_.bpm_.removeTable(this->table_id_);
            if (std::filesystem::exists(this->file_path_)) {
                std::filesystem::remove_all(this->secret_shares_path_);

                if(std::filesystem::exists(this->file_path_) && std::filesystem::is_empty(this->file_path_)) {
                    std::filesystem::remove_all(this->file_path_);
                }
            }
        }
    };
}

#endif

#endif
