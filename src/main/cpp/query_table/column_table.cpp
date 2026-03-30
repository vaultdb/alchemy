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

    while(read_idx < src->schema_.getFieldCount()) {
        Bit *read_pos = reinterpret_cast<Bit *>(src->getFieldPtr(src_row, read_idx));
        PageId r = PageId(src->table_id_, read_idx, src_row / src->fields_per_page_.at(read_idx));
        bpm_.pinPage(r);

        Bit *write_pos = reinterpret_cast<Bit *>(getFieldPtr(dst_row, write_idx));
        int to_read_bits = this->field_sizes_bytes_[write_idx] / sizeof(emp::Bit);


        for(int i = 0; i <  to_read_bits; ++i) {
            *write_pos = emp::If(write, *read_pos, *write_pos);
            ++write_pos;
            ++read_pos;
        }
        PageId dst_pid(this->table_id_, write_idx, dst_row / this->fields_per_page_.at(write_idx));
        bpm_.markDirty(dst_pid);
        bpm_.unpinPage(r);

        ++read_idx;
        ++write_idx;
    }

    // copy dummy tag
    Bit *write_pos = reinterpret_cast<Bit *>(getFieldPtr(dst_row, -1));
    Bit *read_pos = reinterpret_cast<Bit *>(src->getFieldPtr(src_row, -1));
    *write_pos = emp::If(write, *read_pos, *write_pos);
    PageId dst_pid(this->table_id_, -1, dst_row / this->fields_per_page_.at(-1));
    bpm_.markDirty(dst_pid);
}

template <>
void ColumnTable<bool>::setRow(const int &idx, const QueryTuple<bool> &tuple) {

    for (int i = -1; i < schema_.getFieldCount(); ++i) {
        auto f = tuple.getField(i);
        setField(idx, i, f); // needed to maintain dirty bit in BPM
    }
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
        for (int i =  -1; i < schema_.getFieldCount(); ++i) {

            PageId lhs (this->table_id_, i, lhs_row / fields_per_page_[i]);
            auto l = getFieldPtr(lhs_row, i);
            bpm_.pinPage(lhs);

            PageId rhs (this->table_id_, i, rhs_row / fields_per_page_[i]);
            auto r =  getFieldPtr(rhs_row, i);
            bpm_.pinPage(rhs);


            // swap in place
            for(int j = 0; j < field_sizes_bytes_[i]; ++j) {
                *l = *l ^ *r;
                *r = *r ^ *l;
                *l = *l ^ *r;

                ++l;
                ++r;
            }

            bpm_.markDirty(lhs);
            bpm_.unpinPage(lhs);

            bpm_.markDirty(rhs);
            bpm_.unpinPage(rhs);

        }
    }
}

template<>
void ColumnTable<Bit>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {
    int col_cnt = schema_.getFieldCount();

    for(int col_id = 0; col_id < col_cnt; ++col_id) {

        int field_len = schema_.fields_.at(col_id).size();
        PageId lhs (this->table_id_, col_id, lhs_row / fields_per_page_[col_id]);
        PageId rhs (this->table_id_, col_id, rhs_row / fields_per_page_[col_id]);
        Bit *l = reinterpret_cast<Bit *>(getFieldPtr(lhs_row, col_id));
        bpm_.pinPage(lhs);

        Bit *r = reinterpret_cast<Bit *>(getFieldPtr(rhs_row, col_id));
        bpm_.pinPage(rhs);

        for(int i = 0; i < field_len; ++i) {
            emp::swap(swap, l[i], r[i]);
        }

        markDirty(lhs_row, col_id);
        markDirty(rhs_row, col_id);
        bpm_.unpinPage(lhs);
        bpm_.unpinPage(rhs);

    }
    // dummy tag
    PageId lhs (this->table_id_, -1, lhs_row / fields_per_page_[-1]);
    PageId rhs (this->table_id_, -1, rhs_row / fields_per_page_[-1]);

    Bit *l = reinterpret_cast<Bit *>(getFieldPtr(lhs_row, -1));
    bpm_.pinPage(lhs);
    Bit *r = reinterpret_cast<Bit *>(getFieldPtr(rhs_row, -1));
    bpm_.pinPage(rhs);

    Bit o = emp::If(swap, *l, *r);
    o ^= *r;
    *l ^= o;
    *r ^= o;

    markDirty(lhs_row, -1);
    markDirty(rhs_row, -1);
    bpm_.unpinPage(lhs);
    bpm_.unpinPage(rhs);

}




template class vaultdb::ColumnTable<bool>;
template class vaultdb::ColumnTable<emp::Bit>;
