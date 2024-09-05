#include "buffered_column_table.h"

using namespace vaultdb;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

std::vector<emp::Bit> BufferedColumnTable::readSecretSharesFromDisk(const QuerySchema &schema, const int &limit) {
    return std::vector<emp::Bit>();
}

std::vector<int8_t> BufferedColumnTable::serializeWithRevealToXOR(std::vector<emp::Bit> &bits) {
    return std::vector<int8_t>();
}

emp::Bit *BufferedColumnTable::getFieldPtr(const int &row, const int &col) {
    return nullptr;
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {}

#else

std::vector<emp::Bit> BufferedColumnTable::readSecretSharesFromDisk(const QuerySchema &schema, const int &limit) {
    if(this->conf_.inputParty()) {
        Integer dst(this->tuple_cnt_ * this->schema_.size(), 0L, emp::PUBLIC);
        return dst.bits;
    }

    size_t src_byte_cnt = std::filesystem::file_size(this->secret_shares_path_);
    size_t src_bit_cnt = src_byte_cnt * 8;

    std::map<int, int64_t> col_bytes_offsets;
    int64_t col_bytes_cnt = 0L;
    col_bytes_offsets[0] = 0;
    for (int i = 1; i < schema.getFieldCount(); ++i) {
        col_bytes_cnt += (schema.getField(i - 1).size() * this->tuple_cnt_) / 8;
        col_bytes_offsets[i] = col_bytes_cnt;
    }
    col_bytes_cnt += (schema.getField(schema.getFieldCount() - 1).size() * this->tuple_cnt_) / 8;
    col_bytes_offsets[-1] = col_bytes_cnt;

    size_t dst_bit_cnt = this->tuple_cnt_ * this->schema_.size();
    bool *dst_bools = new bool[dst_bit_cnt];
    bool *dst_cursor = dst_bools;

    assert(src_bit_cnt % this->plain_schema_.size() == 0);
    FILE *fp = fopen(this->secret_shares_path_.c_str(), "rb");

    size_t read_tuple_cnt = (limit == -1 || limit > this->tuple_cnt_) ? this->tuple_cnt_ : limit;
    for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
        int read_size_bits = read_tuple_cnt * this->schema_.getField(i).size();
        fseek(fp, col_bytes_offsets[i], SEEK_SET);
        if (this->plain_schema_.getField(i).getType() != FieldType::BOOL) {
            std::vector<int8_t> tmp_read(read_size_bits / 8);
            fread(tmp_read.data(), 1, read_size_bits / 8, fp);
            emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), read_size_bits, false);
            dst_cursor += read_size_bits;
        } else {
            fread(dst_cursor, 1, read_size_bits, fp);
            for(int j = 0; j < read_tuple_cnt; ++j) {
                *dst_cursor = ((*dst_cursor & 1) != 0);
                ++dst_cursor;
            }
        }
    }

    fseek(fp, col_bytes_offsets[-1], SEEK_SET);
    fread(dst_cursor, 1, this->tuple_cnt_, fp);
    for(int j = 0; j < this->tuple_cnt_; ++j) {
        *dst_cursor = ((*dst_cursor & 1) != 0);
        ++dst_cursor;
    }

    fclose(fp);

    Integer dst(dst_bit_cnt, 0L, emp::PUBLIC);
    bool *to_send = (this->conf_.party_ == 1) ? dst_bools : nullptr;
    this->conf_.emp_manager_->feed(dst.bits.data(), 1, to_send, dst_bit_cnt);

    for(int i = 2; i <= N; ++i) {
        Integer tmp(dst_bit_cnt, 0L, emp::PUBLIC);
        to_send = (this->conf_.party_ == i) ? dst_bools : nullptr;
        this->conf_.emp_manager_->feed(tmp.bits.data(), i, to_send, dst_bit_cnt);
        dst = dst ^ tmp;
    }

//    std::vector<emp::Bit> dst(dst_bit_cnt);
//
//    for(int i = 0; i < dst_bit_cnt; ++i) {
//        dst[i] = emp::Bit(dst_bools[i]);
//    }

    delete [] dst_bools;

    return dst.bits;
}

std::vector<int8_t> BufferedColumnTable::serializeWithRevealToXOR(std::vector<emp::Bit> &bits) {
    bool *bools = new bool[bits.size()];

    for(int i = 0; i < bits.size(); ++i) {
        bools[i] = bits[i].reveal(emp::XOR);
    }

    // convert bools to vector of int8_t
    std::vector<int8_t> serialized(bits.size() / 8);
    bool *read_cursor = bools;
    int8_t *write_cursor = serialized.data();
    for(int i = 0; i < serialized.size(); ++i) {
        *write_cursor = Utilities::boolsToByte(read_cursor);
        ++write_cursor;
        read_cursor += 8;
    }

    delete [] bools;
    return serialized;
}

emp::Bit *BufferedColumnTable::getFieldPtr(const int &row, const int &col) {
    std::vector<emp::Bit> secret_shares = this->readSecretSharesFromDisk(this->schema_, -1);
    return secret_shares.data() + this->serialized_col_bytes_offsets_[col] * 8 + row * this->schema_.getField(col).size();
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {
    assert(this->isEncrypted());

    if(!this->conf_.inputParty()) {
        int col = pid.col_id_;
        int field_size_bytes = this->field_sizes_bytes_[col];
        int fields_per_page = this->fields_per_page_[col];
        int row = pid.page_idx_ * fields_per_page;
        std::vector<emp::Bit> secret_shares = this->readSecretSharesFromDisk(this->schema_, -1);
        std::vector<int8_t> serialized = this->serializeWithRevealToXOR(secret_shares);
        int8_t *write_ptr = serialized.data() + this->serialized_col_bytes_offsets_[col] + row * field_size_bytes;
        memcpy(write_ptr, (int8_t *) bits, fields_per_page * field_size_bytes);
        std::vector<int8_t> new_serialized = this->serializeWithRevealToXOR(secret_shares);

        DataUtilities::writeFile(this->secret_shares_path_, new_serialized);
    }
}

#endif