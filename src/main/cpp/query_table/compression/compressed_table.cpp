#include "compressed_table.h"

using namespace vaultdb;

template<>
void CompressedTable<bool>::cloneRow(const bool & write, const int & dst_row, const int & dst_col, const QueryTable<bool> *src, const int & src_row)  {
    if(write)
        cloneRow(dst_row, dst_col, src, src_row);
}

template<>
void CompressedTable<Bit>::cloneRow(const Bit & write, const int & dst_row, const int & dst_col, const QueryTable<Bit> *s, const int & src_row)  {

    assert(s->storageModel() == StorageModel::COMPRESSED_STORE);
    auto src = (CompressedTable<Bit> *) s;
    int src_size = src->tuple_size_bytes_ - src->field_sizes_bytes_.at(-1);

    int write_idx = dst_col; // field indexes
    int read_idx = 0;
    int bytes_read = 0;

    // re-pack row
    while(read_idx < src->schema_.getFieldCount()) {
        Field<Bit> f = src->getField(src_row, read_idx);

//        Bit *write_pos = (Bit *) getFieldPtr(dst_row, write_idx);
//        Bit *read_pos = (Bit *) src->getFieldPtr(src_row, read_idx);
        int bytes_remaining = src_size - bytes_read;
        int dst_len = field_sizes_bytes_.at(write_idx);
        int to_read = (dst_len < bytes_remaining) ? dst_len : bytes_remaining;

        int to_read_bits = to_read / sizeof(emp::Bit);
        f = SecureField::If(write, f, getField(dst_row, write_idx));
        this->setField(dst_row, write_idx, f);

        ++read_idx;
        ++write_idx;
        bytes_read += to_read;
    }

    // copy dummy tag
    Bit dummy_tag = emp::If(write, src->getDummyTag(src_row), getDummyTag(dst_row));
    setDummyTag(dst_row, dummy_tag);
}


template<>
void CompressedTable<bool>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
    if(swap) {
        for(auto pos : column_encodings_) {
            auto lhs = pos.second->getField(lhs_row);
            auto rhs = pos.second->getField(rhs_row);
            pos.second->setField(lhs_row, rhs);
            pos.second->setField(rhs_row, lhs);
        }
    }
}

template<>
void CompressedTable<Bit>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {
    for(auto pos : column_encodings_) {
        auto lhs = pos.second->getField(lhs_row);
        auto rhs = pos.second->getField(rhs_row);
        Field<Bit>::compareAndSwap(swap, lhs, rhs);

        pos.second->setField(lhs_row, lhs);
        pos.second->setField(rhs_row, rhs);
    }
}


template class vaultdb::CompressedTable<bool>;
template class vaultdb::CompressedTable<emp::Bit>;
