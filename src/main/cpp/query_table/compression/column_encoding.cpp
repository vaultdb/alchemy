#include "column_encoding.h"
#include "uncompressed.h"
#include "plain_dictionary_encoding.h"
#include "secure_dictionary_encoding.h"
#include "query_table/compression/compressed_table.h"

using namespace vaultdb;


template<typename B>
ColumnEncoding<B> *
ColumnEncoding<B>::compress(QueryTable<B> *src, const int &src_col, QueryTable<B> *dst, const int &dst_col, const CompressionScheme &dst_encoding) {
    // only compress from plain data, don't support converting from one compression model to another
    assert(((CompressedTable<B> *) src)->column_encodings_.at(src_col)->columnEncoding() == CompressionScheme::PLAIN);
    assert(!src->isEncrypted()); // compression algos only run in plaintext -- too many data-dependent branches to do this under MPC efficiently
    switch(dst_encoding) {
        case CompressionScheme::PLAIN:
        case CompressionScheme::BIT_PACKED:
            return new Uncompressed<B>(dst, dst_col, src, src_col);
        case CompressionScheme::DICTIONARY:
            return new DictionaryEncoding<B>(dst, dst_col, src, src_col);
        default:
            throw;
    }


}

template<typename B>
void ColumnEncoding<B>::cloneColumn(const int &dst_idx, QueryTable<B> *s, const int &src_col, const int &src_idx) {
    assert(s->storageModel() == StorageModel::COMPRESSED_STORE);
    auto src = (CompressedTable<B> *) s;
    auto src_encoding = src->column_encodings_.at(src_col);
    // clone only from columns encoded with the same scheme
    assert(src_encoding->columnEncoding() == this->columnEncoding());

    int8_t *write_ptr = this->getFieldPtr(dst_idx);
    int8_t *read_ptr = src_encoding->getFieldPtr(src_idx);

    int src_tuples = src->tuple_cnt_ - src_idx;
    auto slots_remaining = this->parent_table_->tuple_cnt_ - dst_idx;
    if(src_tuples > slots_remaining) {
        src_tuples = slots_remaining; // truncate to our available slots
    }

    memcpy(write_ptr, read_ptr, this->field_size_bytes_ * src_tuples);
}



template<>
void ColumnEncoding<bool>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
    int8_t *l = getFieldPtr(lhs_row);
    int8_t *r = getFieldPtr(rhs_row);
    if(swap) {
        // swap in place
        for (int i = 0; i < this->field_size_bytes_; ++i) {
            *l = *l ^ *r;
            *r = *r ^ *l;
            *l = *l ^ *r;

            ++l;
            ++r;
        }
    }
}

template<>
void ColumnEncoding<Bit>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {
    auto lhs = (Bit *) getFieldPtr(lhs_row);
    auto rhs = (Bit *) getFieldPtr(rhs_row);

    for(int i = 0; i < this->field_size_bits_; ++i) {
        emp::swap(swap, lhs[i], rhs[i]);
    }

}

template class vaultdb::ColumnEncoding<bool>;
template class vaultdb::ColumnEncoding<emp::Bit>;
