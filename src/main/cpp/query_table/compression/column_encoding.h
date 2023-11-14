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
        int8_t *column_data_;

        ColumnEncoding(QueryTable<B> *parent, const int & col_idx) : parent_table_(parent), column_idx_(col_idx) {
            if(parent_table_->column_data_.find(col_idx) != parent_table_->column_data_.end())
                column_data_ = parent->column_data_.at(col_idx).data();
        }

        virtual ~ColumnEncoding() = default;

        virtual Field<B> getField(const int & row) = 0;
        virtual void setField(const int & row, const Field<B> & f) = 0;
        virtual ColumnEncoding<B> *clone(QueryTable<B> *dst, const int & dst_col) = 0;
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
            for(int i = 0; i < row_cnt; ++i) {
                string str(str_len, 0);
                memcpy((int8_t *) str.c_str(), src + i * str_len, str_len);
                std::reverse(str.begin(), str.end());
                memcpy(src + i * str_len, (int8_t *) str.c_str(), str_len);
            }
        }

    };


}


#endif

