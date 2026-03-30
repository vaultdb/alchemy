#include "bit_packed_encoding.h"
#include "compressed_table.h"

using namespace vaultdb;

void BitPackedEncoding::revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party)  {
    int row_cnt = this->parent_table_->tuple_cnt_;
    auto dst_encoding = ColumnEncoding<bool>::getColumnEncoding(dst, dst_col);
    assert(dst_encoding->columnEncoding() == CompressionScheme::PLAIN);

    Bit *src_ptr = (Bit *) this->column_data_;
    if(field_type_ == FieldType::SECURE_INT) {
        auto dst_ptr = (int32_t *) dst_encoding->column_data_;
        revealBitPackedInts(dst_ptr, src_ptr, row_cnt, party);
    }
    else if(field_type_ == FieldType::SECURE_LONG) {
        auto dst_ptr = (int64_t *) dst_encoding->column_data_;
        revealBitPackedInts(dst_ptr, src_ptr, row_cnt, party);
    }
    else
        throw;
}

void BitPackedEncoding::secretShare(QueryTable<Bit> *dst, const int &dst_col) {
    // already secret shared, just copy it
    auto column_encoding = ColumnEncoding<Bit>::getColumnEncoding(dst, dst_col);
    assert(column_encoding->columnEncoding() == CompressionScheme::BIT_PACKED);

    int row_cnt = this->parent_table_->tuple_cnt_;
    int byte_cnt = row_cnt * field_size_bits_ * sizeof(emp::Bit);
    Bit *src_ptr = (Bit *) this->column_data_;
    Bit *dst_ptr = (Bit *) dst->column_data_.at(dst_col).data();
    memcpy(dst_ptr, src_ptr, byte_cnt);
}


void BitPackedEncoding::revealBitPackedInts(int32_t *dst, emp::Bit *src, const int &row_cnt, const int &party) {

    int bool_cnt = field_size_bits_ * row_cnt;
    bool *bools = new bool[bool_cnt];
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    manager->reveal(bools, party, src, bool_cnt);

    bool *read_cursor = bools;
    int32_t *write_cursor = dst;

    // bools to bytes
    for(int i = 0; i < row_cnt; ++i) {
        std::bitset<32> bs;
        bs.reset();

        for (size_t j = 0; j < this->field_size_bits_; ++j)
            bs.set(j, read_cursor[j]);
        uint32_t to_write = bs.to_ulong();
        *write_cursor = to_write + field_min_; // unsigned with bit packing
       ++write_cursor;
       read_cursor += this->field_size_bits_;
    }


    delete[] bools;
}

void BitPackedEncoding::revealBitPackedInts(int64_t *dst, emp::Bit *src, const int &row_cnt, const int &party) {

    int bool_cnt = field_size_bits_ * row_cnt;
    bool *bools = new bool[bool_cnt];
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    manager->reveal(bools, party, src, bool_cnt);

    bool *read_cursor = bools;
    int64_t *write_cursor = dst;

    // bools to bytes
    for(int i = 0; i < row_cnt; ++i) {
        std::bitset<64> bs;
        bs.reset();
            for (size_t j = 0; j < this->field_size_bits_; ++j)
                bs.set(j, read_cursor[j]);
        auto to_write = bs.to_ulong(); // unsigned with bit packing
        *write_cursor = to_write + field_min_;
        ++write_cursor;
        read_cursor += this->field_size_bits_;
    }


    delete[] bools;
}


