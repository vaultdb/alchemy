#include "buffered_column_table.h"

using namespace vaultdb;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid) {
    return std::vector<emp::Bit>();
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, const string &src_data_path) {
    return std::vector<emp::Bit>();
}

std::vector<int8_t> BufferedColumnTable::convertEMPBitToWriteBuffer(const std::vector<emp::Bit> bits) {
    return std::vector<int8_t>();
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {}

#else
#include "util/emp_manager/outsourced_mpc_manager.h"

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid) {
    int col = pid.col_id_;
    int remained_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * this->fields_per_page_[col];
    int reading_tuple_cnt = (remained_tuple_cnt < this->fields_per_page_[col]) ? remained_tuple_cnt : this->fields_per_page_[col];

    auto secure_field = this->schema_.getField(col);
    int dst_bit_cnt = reading_tuple_cnt * secure_field.size();

    auto *auth_shares = new AuthShare<emp::N>[dst_bit_cnt];
    bool *masked_values = new bool[dst_bit_cnt]();

    FILE*  fp = fopen(this->secret_shares_path_.c_str(), "rb");
    int64_t fread_offset = this->serialized_col_bytes_offsets_on_disk_[col] + (int64_t) (pid.page_idx_ * this->fields_per_page_[col] * secure_field.size() * current_emp_bit_size_on_disk_);
    fseek(fp, fread_offset, SEEK_SET);

    for(int i = 0; i < dst_bit_cnt; ++i) {
        AuthShare<emp::N> cur_auth_share;

        if(!this->conf_.inputParty()) {
            fread((int8_t *) &cur_auth_share.mac, 1, emp::N * sizeof(emp::block), fp);
            fread((int8_t *) &cur_auth_share.key, 1, emp::N * sizeof(emp::block), fp);
        }

        int8_t cur_lambda = 0;
        fread((int8_t *) &cur_lambda, 1, 1, fp);
        cur_auth_share.lambda = ((cur_lambda & 1) != 0);

        auth_shares[i] = cur_auth_share;

        if(this->conf_.party_ == 1) {
            int8_t cur_masked_value = 0;
            fread((int8_t *) &cur_masked_value, 1, 1, fp);
            masked_values[i] = ((cur_masked_value & 1) != 0);
        }
    }

    fclose(fp);

    Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);
    ((OutsourcedMpcManager *) this->conf_.emp_manager_)->protocol_->regen_label(dst_int.bits.data(), masked_values, auth_shares, dst_bit_cnt);

    return dst_int.bits;
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const int src_col, const string &src_data_path) {
    int col = pid.col_id_;
    int remained_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * this->fields_per_page_[col];
    int reading_tuple_cnt = (remained_tuple_cnt < this->fields_per_page_[col]) ? remained_tuple_cnt : this->fields_per_page_[col];

    QuerySchema secure_schema = QuerySchema::toSecure(schema);
    auto secure_field = this->schema_.getField(col);
    int dst_bit_cnt = reading_tuple_cnt * secure_field.size();

    int64_t col_bytes_cnt = 0L;
    int target_offset = 0;
    if(col != 0) {
        for (int i = 1; i < secure_schema.getFieldCount(); ++i) {
            col_bytes_cnt += secure_schema.getField(i - 1).size() * tuple_cnt * current_emp_bit_size_on_disk_;
            target_offset = col_bytes_cnt;

            if(i == src_col) break;
        }
    }
    if(col == -1) {
        col_bytes_cnt += secure_schema.getField(secure_schema.getFieldCount() - 1).size() * tuple_cnt * current_emp_bit_size_on_disk_;
        target_offset = col_bytes_cnt;
    }

    auto *auth_shares = new AuthShare<emp::N>[dst_bit_cnt];
    bool *masked_values = new bool[dst_bit_cnt]();

    FILE*  fp = fopen(src_data_path.c_str(), "rb");
    int fread_offset = target_offset + pid.page_idx_ * this->fields_per_page_[col] * secure_field.size() * current_emp_bit_size_on_disk_;
    fseek(fp, fread_offset, SEEK_SET);

    for(int i = 0; i < dst_bit_cnt; ++i) {
        AuthShare<emp::N> cur_auth_share;

        if(!this->conf_.inputParty()) {
            fread((int8_t *) &cur_auth_share.mac, 1, emp::N * sizeof(emp::block), fp);
            fread((int8_t *) &cur_auth_share.key, 1, emp::N * sizeof(emp::block), fp);
        }

        int8_t cur_lambda = 0;
        fread((int8_t *) &cur_lambda, 1, 1, fp);
        cur_auth_share.lambda = ((cur_lambda & 1) != 0);

        auth_shares[i] = cur_auth_share;

        if(this->conf_.party_ == 1) {
            int8_t cur_masked_value = 0;
            fread((int8_t *) &cur_masked_value, 1, 1, fp);
            masked_values[i] = ((cur_masked_value & 1) != 0);
        }
    }

    fclose(fp);

    Integer dst_int(dst_bit_cnt, 0L, emp::PUBLIC);
    ((OutsourcedMpcManager *) this->conf_.emp_manager_)->protocol_->regen_label(dst_int.bits.data(), masked_values, auth_shares, dst_bit_cnt);

    return dst_int.bits;
}

std::vector<int8_t> BufferedColumnTable::convertEMPBitToWriteBuffer(const std::vector<emp::Bit> bits) {
    std::vector<int8_t> write_buffer(bits.size() * current_emp_bit_size_on_disk_, 0);
    int8_t *write_cursor = write_buffer.data();

    for(int i = 0; i < bits.size(); ++i) {
        emp::Bit cur_bit = bits[i];

        if(!this->conf_.inputParty()) {
            memcpy(write_cursor, (int8_t *) &cur_bit.bit.auth.mac, emp::N * sizeof(emp::block));
            write_cursor += emp::N * sizeof(emp::block);

            memcpy(write_cursor, (int8_t *) &cur_bit.bit.auth.key, emp::N * sizeof(emp::block));
            write_cursor += emp::N * sizeof(emp::block);
        }

        memcpy(write_cursor, (int8_t *) &cur_bit.bit.auth.lambda, 1);
        ++write_cursor;

        if(this->conf_.party_ == 1) {
            memcpy(write_cursor, (int8_t *) &cur_bit.bit.masked_value, 1);
            ++write_cursor;
        }
    }

    return write_buffer;
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {
    assert(this->isEncrypted());

    int col = pid.col_id_;
    int remained_tuple_cnt = this->tuple_cnt_ - pid.page_idx_ * this->fields_per_page_[col];
    int writing_tuple_cnt = (remained_tuple_cnt < this->fields_per_page_[col]) ? remained_tuple_cnt : this->fields_per_page_[col];
    std::vector<emp::Bit> src_bits(bits, bits + writing_tuple_cnt * this->schema_.getField(col).size());
    std::vector<int8_t> write_buffer = convertEMPBitToWriteBuffer(src_bits);

    FILE*  fp = fopen(this->secret_shares_path_.c_str(), "rb+");
    int write_offset = this->serialized_col_bytes_offsets_on_disk_[col] + (int64_t) (pid.page_idx_ * this->fields_per_page_[col] * this->schema_.getField(col).size() * current_emp_bit_size_on_disk_);
    fseek(fp, write_offset, SEEK_SET);

    fwrite(write_buffer.data(), 1, write_buffer.size(), fp);
    fclose(fp);
}

#endif