#ifndef _INPUT_PARTY_BUFFERED_COLUMN_TABLE_
#define _INPUT_PARTY_BUFFERED_COLUMN_TABLE_

#include "query_table/buffered_column_table.h"


// substitute for PackedColumnTable if SystemConfiguration::party_ == SystemConfiguration::input_party_
// input party / TP (trusted party) does not need to hold secret shares, instead it produces zero block for every pack/unpack operation
#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

namespace vaultdb {


    class InputPartyBufferedColumnTable : public BufferedColumnTable {
    public:
         InputPartyBufferedColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : BufferedColumnTable(tuple_cnt, schema, sort_def) {}

         InputPartyBufferedColumnTable(const InputPartyBufferedColumnTable & src) : BufferedColumnTable(src) {}


        QueryTable<Bit> *clone()  override {
            return new InputPartyBufferedColumnTable(*this);
        }

         void writeToFile(const string &fq_filename) const override {

         }

         int8_t * getFieldPtr(const int &row, const int &col) const override {
             return nullptr;
         }

         void putTuple(const int &idx, const QueryTuple<Bit> &tuple) override {

         }

         vector<int8_t> serialize() override {
            return vector<int8_t>();
         }

         std::vector<int8_t> serializeEMPBits(const PageId & pid) const override {
            return std::vector<int8_t>();
        }

        std::vector<int8_t> serializeEMPBits(const std::vector<emp::Bit> bits) const override {
            return std::vector<int8_t>();
        }

        std::vector<emp::Bit> readPage(const PageId pid) const override {
            return std::vector<emp::Bit>();
        }

        std::vector<emp::Bit> readPage(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, fstream &src_data_file) const override {
            return std::vector<emp::Bit>();
        }
    };
}
#else
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

            InputPartyBufferedColumnTable *src_table = const_cast<InputPartyBufferedColumnTable *>(&src);
            bpm_.flushTable<Bit>(src_table->table_id_);
            src_table->table_shares_file_.flush();
            src_table->table_shares_file_.close();

            std::filesystem::copy_file(src_table->table_file_name_.c_str(), table_file_name_.c_str(), std::filesystem::copy_options::overwrite_existing);
            src_table->table_shares_file_.open(src_table->table_file_name_.c_str(), std::ios::in | std::ios::out | std::ios::binary);
            table_shares_file_.open(table_file_name_, std::ios::in | std::ios::out | std::ios::binary);

            if (!table_shares_file_.is_open()) {
                throw std::runtime_error(
                        std::string("In copy constructor: Failed to open file '") + table_file_name_ + "': " + strerror(errno)
                );
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

            // Input party only need lambda in the authentication (not data itself)
            for(size_t i = 0; i < bits.size(); ++i) {
                emp::Bit cur_bit = bits[i];

                memcpy(write_cursor, (int8_t *) &cur_bit.bit.auth.lambda, 1);
                ++write_cursor;
            }

            return write_buffer;
        }

        std::vector<emp::Bit> readPage(const PageId pid) const override {
            int col = pid.col_id_;
            int current_fields_per_page = this->fields_per_page_.at(col);
            size_t remained_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * current_fields_per_page;
            size_t reading_tuple_cnt = (remained_tuple_cnt < current_fields_per_page) ? remained_tuple_cnt : current_fields_per_page;

            auto secure_field = this->schema_.getField(col);
            size_t dst_bit_cnt = reading_tuple_cnt * secure_field.size();

            auto *auth_shares = new AuthShare<emp::N>[dst_bit_cnt];
            bool *masked_values = new bool[dst_bit_cnt];

            size_t fread_offset = this->ordinal_offsets_.at(col) + (int64_t) (pid.page_idx_ * current_fields_per_page * secure_field.size() * emp_bit_disk_size_);
            table_shares_file_.seekg(fread_offset, ios::beg);

            for(size_t i = 0; i < dst_bit_cnt; ++i) {
                AuthShare<emp::N> cur_auth_share;

                int8_t cur_lambda = 0;
                table_shares_file_.read(reinterpret_cast<char*>(&cur_lambda), 1);
                cur_auth_share.lambda = ((cur_lambda & 1) != 0);

                auth_shares[i] = cur_auth_share;
            }

            Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);
            manager_->protocol_->regen_label(dst_int.bits.data(), masked_values, auth_shares, dst_bit_cnt);

            delete [] auth_shares;
            delete [] masked_values;

            return dst_int.bits;
        }

        std::vector<emp::Bit> readPage(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, fstream &src_data_file) const override {
            int col = pid.col_id_;
            int current_fields_per_page = this->fields_per_page_.at(col);
            size_t remained_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * current_fields_per_page;
            size_t reading_tuple_cnt = (remained_tuple_cnt < current_fields_per_page) ? remained_tuple_cnt : current_fields_per_page;

            QuerySchema secure_schema = QuerySchema::toSecure(schema);
            auto secure_field = this->schema_.getField(col);
            size_t dst_bit_cnt = reading_tuple_cnt * secure_field.size();

            auto ordinal_offsets = BufferedColumnTable::getOrdinalOffsets(secure_schema, tuple_cnt);

            auto *auth_shares = new AuthShare<emp::N>[dst_bit_cnt];
            bool *masked_values = new bool[dst_bit_cnt];

            size_t fread_offset = ordinal_offsets[src_col] + pid.page_idx_ * current_fields_per_page * secure_field.size() * emp_bit_disk_size_;
            src_data_file.seekg(fread_offset, ios::beg);

            for(size_t i = 0; i < dst_bit_cnt; ++i) {
                AuthShare<emp::N> cur_auth_share;

                int8_t cur_lambda = 0;
                src_data_file.read(reinterpret_cast<char*>(&cur_lambda), 1);
                cur_auth_share.lambda = ((cur_lambda & 1) != 0);

                auth_shares[i] = cur_auth_share;
            }

            Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);
            manager_->protocol_->regen_label(dst_int.bits.data(), masked_values, auth_shares, dst_bit_cnt);

            delete [] auth_shares;
            delete [] masked_values;

            return dst_int.bits;
        }
    };
}
#endif
#endif //_INPUT_PARTY_BUFFERED_COLUMN_TABLE_
