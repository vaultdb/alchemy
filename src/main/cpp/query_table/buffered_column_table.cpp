#include "buffered_column_table.h"

using namespace vaultdb;

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/emp-zk.h")

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid) {
    return std::vector<emp::Bit>();
}

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const string &src_data_path) {
    return std::vector<emp::Bit>();
}

std::vector<int8_t> BufferedColumnTable::serializeWithRevealToXOR(std::vector<emp::Bit> &bits) {
    return std::vector<int8_t>();
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {}

#else

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid) {
    int col = pid.col_id_;
    int reading_tuple_cnt = (this->tuple_cnt_ < this->fields_per_page_[col]) ? this->tuple_cnt_ : this->fields_per_page_[col];
    cout << "col: " << col << endl;
    cout << "reading_tuple_cnt: " << reading_tuple_cnt << endl;

    auto plain_field = this->plain_schema_.getField(col);
    bool *dst_bools;
    size_t dst_bit_cnt = reading_tuple_cnt * plain_field.size();

    cout << "dst_bit_cnt: " << dst_bit_cnt << endl;

    if(!this->conf_.inputParty()) {
        size_t src_bytes_cnt = std::filesystem::file_size(this->secret_shares_path_);
        size_t src_bits_cnt = src_bytes_cnt * 8;

        cout << "src_bits_cnt: " << src_bits_cnt << endl;

        dst_bools = new bool[dst_bit_cnt];
        bool *dst_cursor = dst_bools;

        assert(src_bits_cnt % this->plain_schema_.size() == 0);
        FILE *fp = fopen(this->secret_shares_path_.c_str(), "rb");

        int64_t fread_offset = (this->serialized_col_bits_offsets_[col] + (int64_t) (pid.page_idx_ * dst_bit_cnt)) / 8;
        fseek(fp, fread_offset, SEEK_SET);

        std::vector<int8_t> tmp_read(dst_bit_cnt / 8);
        fread(tmp_read.data(), 1, dst_bit_cnt / 8, fp);
        emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), dst_bit_cnt, false);

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

std::vector<emp::Bit> BufferedColumnTable::readSecretSharedPageFromDisk(const PageId pid, const int tuple_cnt, const QuerySchema &schema, const string &src_data_path) {
    int col = pid.col_id_;
    int reading_tuple_cnt = (this->tuple_cnt_ < this->fields_per_page_[col]) ? this->tuple_cnt_ : this->fields_per_page_[col];
    cout << "col: " << col << endl;
    cout << "reading_tuple_cnt: " << reading_tuple_cnt << endl;

    QuerySchema plain_schema = QuerySchema::toPlain(schema);
    auto plain_field = plain_schema.getField(col);
    bool *dst_bools;
    size_t dst_bit_cnt = reading_tuple_cnt * plain_field.size();
    cout << "dst_bit_cnt: " << dst_bit_cnt << endl;

    if(!this->conf_.inputParty()) {
        size_t src_bytes_cnt = std::filesystem::file_size(src_data_path);
        size_t src_bits_cnt = src_bytes_cnt * 8;

        cout << "src_bits_cnt: " << src_bits_cnt << endl;

        int64_t col_bytes_cnt = 0L;
        if(col != 0) {
            for (int i = 1; i < plain_schema.getFieldCount(); ++i) {
                col_bytes_cnt += (plain_schema.getField(i - 1).size() * tuple_cnt) / 8;

                if(col == i) {
                    break;
                }
            }

            if(col == -1) {
                col_bytes_cnt += (plain_schema.getField(plain_schema.getFieldCount() - 1).size() * tuple_cnt) / 8;
            }
        }

        cout << "col_bytes_cnt: " << col_bytes_cnt << endl;

        dst_bools = new bool[dst_bit_cnt];
        bool *dst_cursor = dst_bools;

        assert(src_bits_cnt % plain_schema.size() == 0);
        FILE *fp = fopen(src_data_path.c_str(), "rb");

        int64_t fread_offset = col_bytes_cnt + (int64_t) (pid.page_idx_ * dst_bit_cnt / 8);
        cout << "fread_offset: " << fread_offset << endl;
        fseek(fp, fread_offset, SEEK_SET);

        std::vector<int8_t> tmp_read(dst_bit_cnt / 8);
        fread(tmp_read.data(), 1, dst_bit_cnt / 8, fp);
        emp::to_bool<int8_t>(dst_cursor, tmp_read.data(), dst_bit_cnt, false);

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
    cout << "bits size input to reveal:" << bits.size() << endl;

//    for(int i = 0; i < bits.size(); ++i) {
//        bools[i] = bits[i].reveal(emp::XOR);
//    }

    Integer tmp(bits);
    tmp.revealBools(bools, emp::XOR);

    cout << "revealed to XOR\n";

    // convert bools to vector of int8_t
    std::vector<int8_t> serialized = Utilities::boolsToBytes(bools, bits.size());

    cout << "serialized size: " << serialized.size() << endl;

    delete [] bools;
    return serialized;
}

void BufferedColumnTable::writePageToDisk(const PageId &pid, const emp::Bit *bits) {
    assert(this->isEncrypted());

    if(!this->conf_.inputParty()) {
        // TODO: write the page to disk
    }
}

#endif