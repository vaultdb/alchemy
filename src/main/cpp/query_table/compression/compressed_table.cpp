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
    int col_cnt = src->schema_.getFieldCount();
    int write_idx = dst_col; // field indexes

    for(int i = 0; i < col_cnt; ++i) {
        auto dst_encoding = column_encodings_.at(dst_col + i);
        Field<Bit> f = src->getField(src_row, i);
        f = Field<Bit>::If(write, f, getField(dst_row, write_idx));
        dst_encoding->setField(dst_row, f);
        ++write_idx;
    }

    auto dst_encoding = column_encodings_.at(-1);
    Field<Bit> f = src->getField(src_row, -1);
    f = Field<Bit>::If(write, f, getField(dst_row, -1));
    dst_encoding->setField(dst_row, f);

}



template class vaultdb::CompressedTable<bool>;
template class vaultdb::CompressedTable<emp::Bit>;
