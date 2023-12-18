#ifndef _COLUMN_ENCODING_
#define _COLUMN_ENCODING_

#include "common/defs.h"
#include "query_table/field/field.h"
#include "query_table/query_table.h"

namespace vaultdb {

    template<typename B> class Uncompressed;
    template<typename B> class CompressedTable;

    template<typename B>
    class ColumnEncoding {
    public:
        // actually always CompressedTable, need generic to avoid compile-time dependency loop
        QueryTable<B> *parent_table_;
        int column_idx_;
        int8_t *column_data_ = nullptr;
        int field_size_bytes_ = -1;
        int field_size_bits_ = -1;

        ColumnEncoding(QueryTable<B> *parent, const int & col_idx) : parent_table_(parent), column_idx_(col_idx) {
            if(parent_table_->column_data_.find(col_idx) != parent_table_->column_data_.end())
                column_data_ = parent->column_data_.at(col_idx).data();
            this->field_size_bits_ = parent->getSchema().getField(col_idx).size();
            this->field_size_bytes_ = parent->field_sizes_bytes_[col_idx];

        }

        ColumnEncoding(const ColumnEncoding &src) : parent_table_(src.parent_table_), column_idx_(src.column_idx_), column_data_(src.column_data_), field_size_bytes_(src.field_size_bytes_), field_size_bits_(src.field_size_bits_) {}

        virtual ~ColumnEncoding() = default;

        // assumes 1:1 relationship between fields and their compressed representation
        // TODO: override this for ones where they have many-to-one like RLE
        // all subsequent methods implemented in this class have this assumption too.
        virtual Field<B> getField(const int & row) {
            int8_t *src =  this->getFieldPtr(row);
            QueryFieldDesc desc = this->parent_table_->getSchema().getField(this->column_idx_);
            return Field<B>::deserialize(desc, src);
        }

        virtual Field<B> getDecompressedField(const int & row) = 0;

        virtual void setField(const int & row, const Field<B> & f) {
            int8_t *dst = this->column_data_ + this->field_size_bytes_ * row;
            Field<B>::serialize(dst, f, this->parent_table_->getSchema().getField(this->column_idx_));
        }

        virtual void deserializeField(const int & row, const int8_t *src) {
            auto dst = getFieldPtr(row);
            memcpy(dst, src, field_size_bytes_);
        }

        virtual ColumnEncoding<B> *clone(QueryTable<B> *dst, const int & dst_col) = 0;
        virtual void cloneColumn(const int & dst_idx, QueryTable<B> *s, const int & src_col, const int & src_idx);

        virtual void cloneField(const int & dst_row, const QueryTable<B> *src, const int & src_row, const int & src_col) {
            auto src_encoding = getColumnEncoding(src, src_col);

            // clone only from columns encoded with the same scheme
            assert(src_encoding->columnEncoding() == this->columnEncoding());
            assert(this->field_size_bits_ == src_encoding->field_size_bits_);

            int8_t *write_ptr = getFieldPtr(dst_row);
            int8_t *read_ptr = src_encoding->getFieldPtr(src_row);
            memcpy(write_ptr, read_ptr, this->field_size_bytes_);

        }

        virtual void compareSwap(const B &swap, const int &lhs_row, const int &rhs_row);
        virtual CompressionScheme columnEncoding() const = 0;
        virtual void resize(const int & tuple_cnt) {
            this->parent_table_->column_data_[this->column_idx_].resize(tuple_cnt * this->field_size_bytes_);
        }
        // reverse the process of secretShare above
        // unpacks everything including dummies, needs obliviousExpand phase first to securely reveal with other cols
        virtual void revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party) = 0;
        // initialize column with contents of an uncompressed one
        // only compress where B == bool, not emp::Bit
        static ColumnEncoding<B> *compress(QueryTable<B> *src, const int & src_col, QueryTable<B> *dst, const int & dst_col, const CompressionScheme & dst_encoding);
        virtual Uncompressed<B> *decompress(QueryTable<B> *dst, const int & dst_col) = 0;

        // get party info from system configuration
        virtual void secretShare(QueryTable<Bit> *dst, const int & dst_col) = 0;
        // initialize entire column to input field
        virtual void initializeColumn(const Field<B> & field) = 0;

        // for entries that are fixed length like dictionary and bit packing
        virtual inline int8_t *getFieldPtr(const int &row)  const {
            return this->column_data_ + this->field_size_bytes_ * row;
        }

        // helper method - TODO: find somewhere better for this!
        static ColumnEncoding<B> *getColumnEncoding(const QueryTable<B> *src, const int & col_idx) {
            assert(src->storageModel() == StorageModel::COMPRESSED_STORE);
            auto compressed_table = (CompressedTable<B> *) src;
            return compressed_table->column_encodings_.at(col_idx);
        }


        template <typename T>
        static void serializeForBitPacking(bool *dst, T *src, int row_cnt, int field_bit_cnt, T field_min) {
            bool b[field_bit_cnt];
            auto write_ptr = dst;

            for (int i = 0; i < row_cnt; ++i) {
                T to_share = src[i] - field_min;
                emp::int_to_bool<T>(b, to_share, field_bit_cnt);
                memcpy(write_ptr, b, field_bit_cnt); // still plaintext
                write_ptr += field_bit_cnt;
            }
        }

        static void serializeForSecretSharing(bool *dst, int8_t *src, int row_cnt, int field_bit_cnt, int stride_bytes) {
            bool b[field_bit_cnt];
            auto write_ptr = dst;
            auto read_ptr = src;

            for (int i = 0; i < row_cnt; ++i) {
                emp::to_bool<int8_t>(b, read_ptr, field_bit_cnt);
                memcpy(write_ptr, b, field_bit_cnt); // still plaintext
                write_ptr += field_bit_cnt;
                read_ptr += stride_bytes;
            }
        }

    protected:

        inline Integer getInt(int row) const  {
            auto src =  getFieldPtr(row);
            emp::Integer dst(this->field_size_bits_ +  (this->columnEncoding() == CompressionScheme::BIT_PACKED), 0, PUBLIC);
            memcpy(dst.bits.data(), src, field_size_bytes_);
            return dst;
        }

    // batch reveal method
    static void revealToBytes(int8_t *dst, emp::Bit *src, const int & byte_cnt, const int & party = PUBLIC) {
        bool *bools = new bool[byte_cnt * 8];
        EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
        manager->reveal(bools, party, src, byte_cnt * 8);
        // bools to bytes
        bool *read_cursor = bools;
        int8_t *write_cursor = dst;
        for(int i = 0; i < byte_cnt; ++i) {
            *write_cursor = Utilities::boolsToByte(read_cursor);
            ++write_cursor;
            read_cursor += 8;
        }
        delete [] bools;
    }

    static void reverseStrings(char *src, const int & str_len, const int & row_cnt) {
        char *read_ptr = src;
        for(int i = 0; i < row_cnt; ++i) {
            Utilities::reverseString(read_ptr, str_len);
            read_ptr += str_len;
        }
    }





};




}


#endif

