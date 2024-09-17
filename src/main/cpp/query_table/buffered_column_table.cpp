#include "buffered_column_table.h"

using namespace vaultdb;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

std::vector<emp::Bit> BufferedColumnTable::readSecretSharesFromDisk(const int &tuple_cnt, const int &limit) {
    return vector<emp::Bit>();
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharesFromDisk(const int &tuple_cnt, const QuerySchema &schema, const vector<int> & col_ordinals, const int &limit) {
    return std::vector<emp::Bit>();
}

std::vector<int8_t> BufferedColumnTable::serializeWithRevealToXOR(std::vector<emp::Bit> &bits) {
    return std::vector<int8_t>();
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid) {
    return std::vector<emp::Bit>();
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid, const QuerySchema &schema, const int &limit) {
    return std::vector<emp::Bit>();
}

std::pair<int, int> BufferedColumnTable::getFieldPtrRange(const int &row, const int &col) {
    return nullptr;
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {}

#else

std::vector<emp::Bit> BufferedColumnTable::readSecretSharesFromDisk(const int &tuple_cnt, const int &limit) {
    bool *dst_bools;
    size_t dst_bit_cnt = this->tuple_cnt_ * this->plain_schema_.size();

    if(!this->conf_.inputParty()) {
        size_t src_byte_cnt = std::filesystem::file_size(this->secret_shares_path_);
        size_t src_bit_cnt = src_byte_cnt * 8;

        dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
        bool *dst_cursor = dst_bools;

        assert(src_bit_cnt % this->plain_schema_.size() == 0);
        FILE* fp = fopen(this->secret_shares_path_.c_str(), "rb");

        for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
            auto plain_field = this->plain_schema_.getField(i);
            auto secure_field = this->schema_.getField(i);

            int read_size_bits = this->tuple_cnt_ * plain_field.size();
            if(plain_field.getType() != FieldType::BOOL) {
                vector<int8_t> tmp_read(read_size_bits / 8);
                fread(tmp_read.data(), 1, tmp_read.size(), fp);
                emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), read_size_bits, false);
                dst_cursor += read_size_bits;
            }
            else {
                assert(plain_field.size() == (secure_field.size() + 7));
                fread(dst_cursor, 1, this->tuple_cnt_, fp);
                for(int j = 0; j < this->tuple_cnt_; ++j) {
                    *dst_cursor = ((*dst_cursor & 1) != 0);
                    ++dst_cursor;
                }
            }

            if(tuple_cnt != this->tuple_cnt_) {
                int to_seek = ((tuple_cnt - this->tuple_cnt_) * plain_field.size()) / 8;
                fseek(fp, to_seek, SEEK_CUR);
            }
        }

        fseek(fp, this->serialized_col_bits_offsets_[-1]/8, SEEK_SET);
        fread(dst_cursor, 1, this->tuple_cnt_, fp);
        for(int i = 0; i < this->tuple_cnt_; ++i) {
            *dst_cursor = ((*dst_cursor & 1) != 0);
            ++dst_cursor;
        }

        fclose(fp);
    }

    Integer dst(dst_bit_cnt, 0L, emp::PUBLIC);

    bool *to_send = (this->conf_.party_ == 1) ? dst_bools : nullptr;
    this->conf_.emp_manager_->feed(dst.bits.data(), 1, to_send, dst_bit_cnt);

    for(int i = 2; i <= N; ++i) {
        Integer tmp(dst_bit_cnt, 0L, emp::PUBLIC);
        to_send = (this->conf_.party_ == i) ? dst_bools : nullptr;
        this->conf_.emp_manager_->feed(tmp.bits.data(), i, to_send, dst_bit_cnt);
        dst = dst ^ tmp;
    }

    return dst.bits;
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharesFromDisk(const int &tuple_cnt, const QuerySchema &schema, const vector<int> & col_ordinals, const int &limit) {
    if(col_ordinals.empty()) {
        return readSecretSharesFromDisk(tuple_cnt, limit);
    }

    QuerySchema plain_schema = QuerySchema::toPlain(schema);

    bool *dst_bools;
    size_t dst_bit_cnt = this->tuple_cnt_ * this->plain_schema_.size();

    if(!this->conf_.inputParty()) {
        size_t src_byte_cnt = std::filesystem::file_size(this->secret_shares_path_);
        size_t src_bit_cnt = src_byte_cnt * 8;

        std::map<int, int64_t> col_bytes_offsets;
        int64_t col_bytes_cnt = 0L;
        col_bytes_offsets[0] = 0;
        for (int i = 1; i < plain_schema.getFieldCount(); ++i) {
            col_bytes_cnt += (plain_schema.getField(i - 1).size() * tuple_cnt) / 8;
            col_bytes_offsets[i] = col_bytes_cnt;
        }
        col_bytes_cnt += (plain_schema.getField(plain_schema.getFieldCount() - 1).size() * tuple_cnt) / 8;
        col_bytes_offsets[-1] = col_bytes_cnt;

        dst_bools = new bool[dst_bit_cnt];
        bool *dst_cursor = dst_bools;

        assert(src_bit_cnt % plain_schema.size() == 0);
        FILE *fp = fopen(this->secret_shares_path_.c_str(), "rb");

        for (auto src_ordinal : col_ordinals) {
            int read_size_bits = this->tuple_cnt_ * plain_schema.getField(src_ordinal).size();
            fseek(fp, col_bytes_offsets[src_ordinal], SEEK_SET);
            if (plain_schema.getField(src_ordinal).getType() != FieldType::BOOL) {
                std::vector<int8_t> tmp_read(read_size_bits / 8);
                fread(tmp_read.data(), 1, read_size_bits / 8, fp);
                emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), read_size_bits, false);
                dst_cursor += read_size_bits;
            } else {
                assert(plain_schema.getField(src_ordinal).size() == (schema.getField(src_ordinal).size() + 7));
                fread(dst_cursor, 1, read_size_bits, fp);
                for (int j = 0; j < this->tuple_cnt_; ++j) {
                    *dst_cursor = ((*dst_cursor & 1) != 0);
                    ++dst_cursor;
                }
            }
        }

        fseek(fp, col_bytes_offsets[-1], SEEK_SET);
        fread(dst_cursor, 1, this->tuple_cnt_, fp);
        for (int j = 0; j < this->tuple_cnt_; ++j) {
            *dst_cursor = ((*dst_cursor & 1) != 0);
            ++dst_cursor;
        }

        fclose(fp);
    }

    Integer dst(dst_bit_cnt, 0L, emp::PUBLIC);
    bool *to_send = (this->conf_.party_ == 1) ? dst_bools : nullptr;
    this->conf_.emp_manager_->feed(dst.bits.data(), 1, to_send, dst_bit_cnt);

    for(int i = 2; i <= N; ++i) {
        Integer tmp(dst_bit_cnt, 0L, emp::PUBLIC);
        to_send = (this->conf_.party_ == i) ? dst_bools : nullptr;
        this->conf_.emp_manager_->feed(tmp.bits.data(), i, to_send, dst_bit_cnt);
        dst = dst ^ tmp;
    }

    if(!this->conf_.inputParty()) delete [] dst_bools;

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
        *write_cursor = (int8_t) Utilities::boolsToByte(read_cursor);
        ++write_cursor;
        read_cursor += 8;
    }
    //emp::from_bool(bools, serialized.data(), bits.size(), false);

    delete [] bools;
    return serialized;
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid) {
    int col = pid.col_id_;
    int reading_tuple_cnt = this->fields_per_page_[col];

    auto plain_field = this->plain_schema_.getField(col);
    bool *dst_bools;
    size_t dst_bit_cnt = reading_tuple_cnt * plain_field.size();

    if(!this->conf_.inputParty()) {
        size_t src_bytes_cnt = std::filesystem::file_size(this->secret_shares_path_);
        size_t src_bits_cnt = src_bytes_cnt * 8;

        dst_bools = new bool[dst_bit_cnt];
        bool *dst_cursor = dst_bools;

        assert(src_bits_cnt % this->plain_schema_.size() == 0);
        FILE *fp = fopen(this->secret_shares_path_.c_str(), "rb");

        fseek(fp, this->serialized_col_bits_offsets_[col] / 8, SEEK_SET);

        if(plain_field.getType() != FieldType::BOOL) {
            std::vector<int8_t> tmp_read(dst_bit_cnt / 8);
            fread(tmp_read.data(), 1, dst_bit_cnt / 8, fp);
            emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), dst_bit_cnt, false);
            dst_cursor += dst_bit_cnt;
        }
        else {
            assert(plain_field.size() == (this->schema_.getField(col).size() + 7));
            fread(dst_cursor, 1, dst_bit_cnt, fp);
            for(int j = 0; j < reading_tuple_cnt; ++j) {
                *dst_cursor = ((*dst_cursor & 1) != 0);
                ++dst_cursor;
            }
        }

        fclose(fp);
    }

    Integer dst(dst_bit_cnt, 0L, emp::PUBLIC);
    bool *to_send = (this->conf_.party_ == 1) ? dst_bools : nullptr;
    this->conf_.emp_manager_->feed(dst.bits.data(), 1, to_send, dst_bit_cnt);

    for(int i = 2; i <= N; ++i) {
        Integer tmp(dst_bit_cnt, 0L, emp::PUBLIC);
        to_send = (this->conf_.party_ == i) ? dst_bools : nullptr;
        this->conf_.emp_manager_->feed(tmp.bits.data(), i, to_send, dst_bit_cnt);
        dst = dst ^ tmp;
    }

    if(!this->conf_.inputParty()) delete [] dst_bools;

    return dst.bits;
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid, const QuerySchema &schema, const int &limit) {
    int col = pid.col_id_;
    return std::vector<emp::Bit>();
}

std::pair<int, int> BufferedColumnTable::getFieldPtrRange(const int &row, const int &col) {
    int start_offset = this->serialized_col_bits_offsets_[col] + ((this->schema_.getField(col).getType() == FieldType::BOOL || col == -1) ? (row * 8) : (row * this->schema_.getField(col).size()));
    int end_offset = start_offset + ((this->schema_.getField(col).getType() == FieldType::BOOL || col == -1) ? 8 : this->schema_.getField(col).size()) * (((this->tuple_cnt_ - row) < this->fields_per_page_[col]) ? (this->tuple_cnt_ - row) : this->fields_per_page_[col]);

    return std::make_pair(start_offset, end_offset);
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {
    assert(this->isEncrypted());

    std::vector<emp::Bit> secret_shares = this->readSecretSharesFromDisk(this->tuple_cnt_, -1);

    if(!this->conf_.inputParty()) {
        int col = pid.col_id_;
        int field_size_bytes = this->field_sizes_bytes_[col];
        int fields_per_page = this->fields_per_page_[col];
        int row = pid.page_idx_ * fields_per_page;

        std::pair<int, int> range = this->getFieldPtrRange(row, col);
        memcpy(secret_shares.data(), bits, range.second - range.first);
        std::vector<int8_t> serialized = this->serializeWithRevealToXOR(secret_shares);
        DataUtilities::writeFile(this->secret_shares_path_, serialized);
    }
}

#endif