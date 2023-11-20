#ifndef _COLUMN_ENCODING_
#define _COLUMN_ENCODING_

#include "common/defs.h"
#include "query_table/field/field.h"
#include "query_table/query_table.h"

namespace vaultdb {

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

        virtual Field<B> getField(const int & row) = 0;
        virtual void setField(const int & row, const Field<B> & f) = 0;
        virtual void deserializeField(const int & row, const int8_t *src) = 0; // deserialize from a byte array
        virtual ColumnEncoding<B> *clone(QueryTable<B> *dst, const int & dst_col) = 0;
        virtual void cloneColumn(const int & dst_idx, QueryTable<B> *src, const int & src_col, const int & src_idx) = 0;
        virtual void cloneField(const int & dst_row, const QueryTable<B> *src, const int & src_row, const int & src_col) = 0;

        virtual ColumnEncodingModel columnEncoding() = 0;
        virtual void resize(const int & tuple_cnt) = 0;
        // reverse the process of secretShare above
        // unpacks everything including dummies, needs obliviousExpand phase first to securely reveal with other cols
        virtual void revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party) = 0;
        // initialize column with contents of an uncompressed one
        // only compress where B == bool, not emp::Bit
        virtual void compress(QueryTable<B> *src, const int & src_col) = 0;
        static ColumnEncoding<B> *compress(QueryTable<B> *src, const int & src_col, QueryTable<B> *dst, const int & dst_col, const ColumnEncodingModel & dst_encoding);
        // get party info from system configuration
        virtual void secretShare(QueryTable<Bit> *dst, const int & dst_col) = 0;
        // initialize entire column to input field
        virtual void initializeColumn(const Field<B> & field) = 0;
    protected:
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

