#include "query_table/column_table.h"
#include "query_table/plain_tuple.h"
#include "query_table/secure_tuple.h"
#include "util/field_utilities.h"

using namespace vaultdb;

template<>
void ColumnTable<bool>::cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<bool> *src, const int & src_row)  {
    if(write)
        cloneRow(dst_row, dst_col, src, src_row);
}

template<>
void ColumnTable<Bit>::cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *s, const int & src_row)  {

    assert(s->storageModel() == StorageModel::COLUMN_STORE);
    auto src = (ColumnTable<Bit> *) s;

    int write_idx = dst_col; // field indexes
    int read_idx = 0;
    int bytes_read = 0;

    while(read_idx < src->schema_.getFieldCount()) {
        Bit *write_pos = reinterpret_cast<Bit *>(getFieldPtr(dst_row, write_idx));
        Bit *read_pos = reinterpret_cast<Bit *>(src->getFieldPtr(src_row, read_idx));
        int to_read_bits = this->field_sizes_bytes_[write_idx] / sizeof(emp::Bit);
        for(int i = 0; i <  to_read_bits; ++i) {
            *write_pos = emp::If(write, *read_pos, *write_pos);
            ++write_pos;
            ++read_pos;
        }
        ++read_idx;
        ++write_idx;
    }

    // copy dummy tag
    Bit *write_pos = reinterpret_cast<Bit *>(getFieldPtr(dst_row, -1));
    Bit *read_pos = reinterpret_cast<Bit *>(src->getFieldPtr(src_row, -1));
    *write_pos = emp::If(write, *read_pos, *write_pos);
}

template <>
void ColumnTable<bool>::setRow(const int &idx, const QueryTuple<bool> &tuple) {
    auto read_pos = tuple.getData();

    for(int i = 0; i < schema_.getFieldCount(); ++i) {
        auto write_pos = getFieldPtr(idx, i);
        memcpy(write_pos, read_pos, field_sizes_bytes_[i]);
        read_pos += field_sizes_bytes_[i];
    }

    // dummy tag
    auto write_pos = getFieldPtr(idx, -1);
    memcpy(write_pos, read_pos, field_sizes_bytes_[-1]);
}

template<>
void ColumnTable<Bit>::setRow(const int &idx, const QueryTuple<Bit> &tuple) {
    auto read_pos = tuple.getData();
    int row_bits = schema_.size();
    emp::Integer i_tuple(row_bits, 0L);
    memcpy(i_tuple.bits.data(), read_pos, row_bits * sizeof(emp::Bit));
    packRow(idx, i_tuple);
}

template<>
void ColumnTable<bool>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
    if(swap) {
        // iterating on column_data to cover dummy tag at -1
        for(auto pos : column_data_) {
            int col_id = pos.first;
            int8_t *l = getFieldPtr(lhs_row, col_id);
            int8_t *r = getFieldPtr(rhs_row, col_id);

            // swap in place
            for(int i = 0; i < field_sizes_bytes_[col_id]; ++i) {
                *l = *l ^ *r;
                *r = *r ^ *l;
                *l = *l ^ *r;

                ++l;
                ++r;
            }

        }
    }
}

template<>
void ColumnTable<Bit>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {

    int col_cnt = schema_.getFieldCount();

    for(int col_id = 0; col_id < col_cnt; ++col_id) {

        int field_len = schema_.fields_.at(col_id).size();
        Bit *l = reinterpret_cast<Bit *>(getFieldPtr(lhs_row, col_id));
        Bit *r = reinterpret_cast<Bit *>(getFieldPtr(rhs_row, col_id));

        for(int i = 0; i < field_len; ++i) {
            emp::swap(swap, l[i], r[i]);
        }

    }
    // dummy tag
    Bit *l = reinterpret_cast<Bit *>(getFieldPtr(lhs_row, -1));
    Bit *r = reinterpret_cast<Bit *>(getFieldPtr(rhs_row, -1));
    Bit o = emp::If(swap, *l, *r);
    o ^= *r;
    *l ^= o;
    *r ^= o;

}



template class vaultdb::ColumnTable<bool>;
template class vaultdb::ColumnTable<emp::Bit>;
