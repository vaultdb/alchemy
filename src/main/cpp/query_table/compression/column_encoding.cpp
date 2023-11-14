#include "column_encoding.h"
#include "plain_encoding.h"
#include "query_table/compression/compressed_table.h"
#include "query_table/compression/compressed_table.h"
using namespace vaultdb;


template<typename B>
ColumnEncoding<B> *
ColumnEncoding<B>::compress(QueryTable<B> *src, const int &src_col, QueryTable<B> *dst, const int &dst_col, const ColumnEncodingModel &dst_encoding) {
    // only compress from plain data, don't support converting from one compression model to another
    assert(((CompressedTable<B> *) src)->column_encodings_.at(src_col)->columnEncoding() == ColumnEncodingModel::PLAIN);

    switch(dst_encoding) {
        case ColumnEncodingModel::PLAIN: {
            auto plain =  new PlainEncoding<B>(dst, dst_col);
            plain->compress(src, src_col);
            return plain;
        }
        default:
            throw;
    }


}





template class vaultdb::ColumnEncoding<bool>;
template class vaultdb::ColumnEncoding<emp::Bit>;
