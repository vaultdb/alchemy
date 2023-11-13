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
            column_data_ = parent->column_data_.at(col_idx).data();
        }

        virtual Field<B> getField(const int & row) = 0;
        virtual void setField(const int & row, const Field<B> & f) = 0;
        virtual ColumnEncodingModel columnEncoding() = 0;
        static ColumnEncoding<B> *encode(QueryTable<B> *src, const int & src_col, QueryTable<B> *dst, const int & dst_col, const ColumnEncodingModel & dst_encoding);
    };


}


#endif

