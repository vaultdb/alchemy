#ifndef _INPUT_PARTY_BUFFERED_COLUMN_TABLE_
#define _INPUT_PARTY_BUFFERED_COLUMN_TABLE_

#include "query_table/buffered_column_table.h"



namespace  vaultdb {
    class InputPartyBufferedColumnTable : public BufferedColumnTable {
    public:

        InputPartyBufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : BufferedColumnTable(tuple_cnt, schema, sort_def) {
            emp_bit_disk_size_ = 1;

            vector<Bit> zero_page(page_size_bits_, Bit(0));
            vector<int8_t> serialized_zero_page = serializeEMPBits(zero_page);

            vector<Bit> one_page(page_size_bits_, Bit(1));
            vector<int8_t> serialized_one_page = serializeEMPBits(one_page);

            std::ofstream create(table_file_name_, std::ios::binary | std::ios::app);
            create.close();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            table_shares_file_.open(table_file_name_, std::ios::in | std::ios::out | std::ios::binary);

            if (!table_shares_file_.is_open()) {
                throw std::runtime_error(
                        std::string("In constructor: Failed to open file '") + table_file_name_ + "': " + strerror(errno)
                );
            }

            // initialize the secret shares file with zeros
            for(int i = 0; i < schema_.getFieldCount(); ++i) {
                table_shares_file_.seekp(ordinal_offsets_[i], ios::beg);
                for(int j = 0; j < pages_per_col_[i]; ++j) {
                    table_shares_file_.write(reinterpret_cast<char*>(serialized_zero_page.data()), serialized_zero_page.size());
                }
            }
            // initialize the dummy tag column with ones
            table_shares_file_.seekp(ordinal_offsets_[-1], ios::beg);
            for(int i = 0; i < pages_per_col_[-1]; ++i) {
                table_shares_file_.write(reinterpret_cast<char*>(serialized_one_page.data()), serialized_one_page.size());
            }

            table_shares_file_.flush();
        }

        InputPartyBufferedColumnTable(const InputPartyBufferedColumnTable &src) : BufferedColumnTable(src) {
            emp_bit_disk_size_ = 1;

            size_t src_table_size_bytes = this->ordinal_offsets_.at(-1)
+ Utilities::pageAlignedBytes(this->field_sizes_bytes_.at(-1), src.tuple_cnt_, bpm_.page_size_bytes_);
            Utilities::runCommand("touch " + table_file_name_);
            std::filesystem::resize_file(table_file_name_.c_str(), src_table_size_bytes);

            table_shares_file_.open(table_file_name_.c_str(),  std::ios::in | std::ios::out | std::ios::binary);

            if (!table_shares_file_.is_open()) {
                throw std::runtime_error("ColumnTable copy constructor cannot open table file to write at " + table_file_name_);
            }

            // TODO: consider doing this in files instead of in the BPM
            for (int i = -1; i < src.schema_.getFieldCount(); ++i) {

                int page_cnt = Utilities::getPageCount(this->field_sizes_bytes_.at(i), this->tuple_cnt_, bpm_.page_size_bytes_);
                for (int j = 0; j < page_cnt; ++j) {
                    PageId dst_pid(this->table_id_, i, j);
                    PageId src_pid(src.table_id_, i, j);
                    bpm_.clonePage<Bit>(dst_pid, src_pid);

                }
            }

        }

        QueryTable<Bit> *clone()  override {
            return new InputPartyBufferedColumnTable(*this);
        }



        void writeToFile(const string &fq_filename) const override {
            throw runtime_error("Not yet implemented!");

        }

        std::vector<int8_t> serializeEMPBits(const PageId & pid) const override {
            int col = pid.col_id_;
            size_t read_byte_size = fields_per_page_.at(col) * schema_.getField(col).size() * emp_bit_disk_size_;
            size_t read_offset = ordinal_offsets_.at(col) + (size_t) (pid.page_idx_ * read_byte_size);
            vector<int8_t> dst = vector<int8_t>(read_byte_size, 0);
            table_shares_file_.seekg(read_offset, ios::beg);
            table_shares_file_.read(reinterpret_cast<char*>(dst.data()), read_byte_size);
            return dst;
        }

        std::vector<int8_t> serializeEMPBits(const std::vector<emp::Bit> bits) const override {
            std::vector<int8_t> write_buffer(bits.size() * emp_bit_disk_size_, 0);
            int8_t *write_cursor = write_buffer.data();
            auto mgr =  (OutsourcedMpcManager *) SystemConfiguration::getInstance().emp_manager_;

            // Input party only need lambda in the authentication (not data itself)
            for(size_t i = 0; i < bits.size(); ++i) {
                emp::Bit cur_bit = bits[i];
                int8_t l = mgr->getLambda(cur_bit);
                memcpy(write_cursor, &l, 1);
                ++write_cursor;
            }

            return write_buffer;
        }

        std::vector<emp::Bit> readPage(const PageId pid) const override {
            int col = pid.col_id_;
            int fields_per_page = this->fields_per_page_.at(col);
            size_t remaining_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * fields_per_page;
            size_t fields_to_read = (remaining_tuple_cnt < fields_per_page) ? remaining_tuple_cnt : fields_per_page;

            auto secure_field = this->schema_.getField(col);
            size_t dst_bit_cnt = fields_to_read * secure_field.size();

            size_t fread_offset = this->ordinal_offsets_.at(col) + (pid.page_idx_ * bpm_.page_size_bytes_);
            table_shares_file_.seekg(fread_offset, ios::beg);

            auto mgr = (OutsourcedMpcManager *) SystemConfiguration::getInstance().emp_manager_;

            vector<int8_t> dst_bytes(dst_bit_cnt * emp_bit_disk_size_, 0);
            table_shares_file_.read(reinterpret_cast<char*>(dst_bytes.data()), dst_bytes.size());
            return mgr->deserializeBits(dst_bytes.data(), dst_bit_cnt, false, true);
        }

        std::vector<emp::Bit> readPage(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, fstream &src_data_file) const override {
            int col = pid.col_id_;
            int fields_per_page = this->fields_per_page_.at(col);
            size_t remaining_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * fields_per_page;
            size_t tuple_cnt_to_read = (remaining_tuple_cnt < fields_per_page) ? remaining_tuple_cnt : fields_per_page;

            QuerySchema secure_schema = QuerySchema::toSecure(schema);
            auto secure_field = this->schema_.getField(col);
            size_t dst_bit_cnt = tuple_cnt_to_read * secure_field.size();

            auto ordinal_offsets = BufferedColumnTable::getOrdinalOffsets(secure_schema, tuple_cnt);
            vector<int8_t> dst(dst_bit_cnt * emp_bit_disk_size_, 0);
            auto fread_offset = ordinal_offsets[src_col] + (size_t) (pid.page_idx_ * bpm_.page_size_bytes_);
            src_data_file.seekg(fread_offset, ios::beg);
            src_data_file.read(reinterpret_cast<char*>(dst.data()), dst_bit_cnt);
            auto mgr = (OutsourcedMpcManager *) SystemConfiguration::getInstance().emp_manager_;
            return mgr->deserializeBits(dst.data(), dst_bit_cnt, false, true);
        }
    };
}
#endif
