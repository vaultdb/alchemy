#include "bit_packed_encoding.h"
#include "compressed_table.h"

using namespace vaultdb;

void BitPackedEncoding::revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party)  {
    int row_cnt = this->parent_table_->tuple_cnt_;

    SystemConfiguration &s = SystemConfiguration::getInstance();
    auto manager = s.emp_manager_;

    assert(dst->storageModel() == StorageModel::COMPRESSED_STORE);
    auto dst_table = (CompressedTable<bool> *) dst;
    assert(dst_table->column_encodings_.at(dst_col)->columnEncoding() == ColumnEncodingModel::PLAIN);

    Bit *src_ptr = (Bit *) this->column_data_;

    if(field_type_ == FieldType::SECURE_INT) {
        auto dst_ptr = (int32_t *) dst->column_data_.at(dst_col).data();
        this->revealBitPackedInts<int32_t>(dst_ptr, src_ptr, row_cnt, party);
    }
    else if(field_type_ == FieldType::SECURE_LONG)
        this->revealBitPackedInts<int64_t>((int64_t *) dst->column_data_.at(dst_col).data(), src_ptr, row_cnt, party);
    else
        throw;
}

void BitPackedEncoding::secretShare(QueryTable<Bit> *dst, const int &dst_col) {
    // already secret shared, just copy it
    assert(dst->storageModel() == StorageModel::COMPRESSED_STORE);
    assert(((CompressedTable<Bit> *) dst)->column_encodings_.at(dst_col)->columnEncoding() == ColumnEncodingModel::BIT_PACKED);

    int row_cnt = this->parent_table_->tuple_cnt_;
    int byte_cnt = row_cnt * field_size_bits_ * sizeof(emp::Bit);
    Bit *src_ptr = (Bit *) this->column_data_;
    Bit *dst_ptr = (Bit *) dst->column_data_.at(dst_col).data();
    memcpy(dst_ptr, src_ptr, byte_cnt);
}

template<typename T>
void BitPackedEncoding::revealBitPackedInts(T *dst, emp::Bit *src, const int &row_cnt, const int &party) {
    static_assert(std::numeric_limits<T>::is_integer, "Only support numerical types");

    int bool_cnt = field_size_bits_ * row_cnt;
    bool *bools = new bool[bool_cnt];
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    manager->reveal(bools, party, src, bool_cnt);
    // bools to bytes
    bool *read_cursor = bools;
    T *write_cursor = dst;
    T one = 1;

    for (int i = 0; i < row_cnt; ++i) {
        T to_write = 0;
        for (int j = 0; j < field_size_bits_; ++j) {
            if (*read_cursor) {
                to_write = to_write | (one << j);
            }
            ++read_cursor;
        }
        *write_cursor = to_write;
        *write_cursor += field_min_;
        ++write_cursor;
    }

    delete[] bools;
}
