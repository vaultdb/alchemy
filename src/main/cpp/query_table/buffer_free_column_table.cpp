#include "query_table/buffer_free_column_table.h"
#include "query_table/plain_tuple.h"
#include "query_table/secure_tuple.h"
#include "util/field_utilities.h"

using namespace vaultdb;

template<>
void BufferFreeColumnTable<bool>::cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<bool> *src, const int & src_row)  {
    if(write)
        cloneRow(dst_row, dst_col, src, src_row);
}

template<>
void BufferFreeColumnTable<Bit>::cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *s, const int & src_row)  {

    assert(s->storageModel() == StorageModel::COLUMN_STORE);
    auto src = (BufferFreeColumnTable<Bit> *) s;

    int write_idx = dst_col; // field indexes
    int read_idx = 0;

    while(read_idx < src->schema_.getFieldCount()) {
        Bit *read_pos = reinterpret_cast<Bit *>(src->getFieldPtr(src_row, read_idx));
        PageId r = PageId(src->table_id_, read_idx, src_row / src->fields_per_page_.at(read_idx));

        Bit *write_pos = reinterpret_cast<Bit *>(getFieldPtr(dst_row, write_idx));
        int to_read_bits = this->field_sizes_bytes_[write_idx] / sizeof(emp::Bit);


        for(int i = 0; i <  to_read_bits; ++i) {
            *write_pos = emp::If(write, *read_pos, *write_pos);
            ++write_pos;
            ++read_pos;
        }
        PageId dst_pid(this->table_id_, write_idx, dst_row / this->fields_per_page_.at(write_idx));
        this->markDirty(dst_row, write_idx);
        ++read_idx;
        ++write_idx;
    }

    // copy dummy tag
    Bit *write_pos = reinterpret_cast<Bit *>(getFieldPtr(dst_row, -1));
    Bit *read_pos = reinterpret_cast<Bit *>(src->getFieldPtr(src_row, -1));
    *write_pos = emp::If(write, *read_pos, *write_pos);
    this->markDirty(dst_row, -1);
}

template <>
void BufferFreeColumnTable<bool>::setRow(const int &idx, const QueryTuple<bool> &tuple) {

    for (int i = -1; i < schema_.getFieldCount(); ++i) {
        auto f = tuple.getField(i);
        setField(idx, i, f); // needed to maintain dirty bit in BPM
    }
}

template<>
void BufferFreeColumnTable<Bit>::setRow(const int &idx, const QueryTuple<Bit> &tuple) {
    auto read_pos = tuple.getData();
    int row_bits = schema_.size();
    emp::Integer i_tuple(row_bits, 0L);
    memcpy(i_tuple.bits.data(), read_pos, row_bits * sizeof(emp::Bit));
    packRow(idx, i_tuple);
}

template<>
void BufferFreeColumnTable<bool>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
    if(swap) {
        throw std::runtime_error("BufferFreeColumnTable<bool>::compareSwap() swaps not implemented");
    }
}

template<>
void BufferFreeColumnTable<Bit>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {
    throw std::runtime_error("BufferFreeColumnTable<bool>::compareSwap() swaps not implemented");
}




template class vaultdb::BufferFreeColumnTable<bool>;
template class vaultdb::BufferFreeColumnTable<emp::Bit>;
