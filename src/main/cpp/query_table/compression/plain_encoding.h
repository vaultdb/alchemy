#ifndef _PLAIN_ENCODING_
#define _PLAIN_ENCODING_
#include "column_encoding.h"
#include "query_table/query_table.h"

namespace vaultdb {
    // Column encoding with no compression
    template<typename B>
    class PlainEncoding : public ColumnEncoding<B> {
        public:
        // this is mostly a placeholder to support heterogeneous (one compression scheme / table) projections of a table
        PlainEncoding<B>(QueryTable<B> *parent, const int & col_idx) : ColumnEncoding<B>(parent, col_idx) {
            col_field_size_bytes_ = parent->field_sizes_bytes_.at(col_idx);
            auto dst_size = col_field_size_bytes_ * parent->tuple_cnt_;
            parent->column_data_[col_idx].resize(dst_size);
        }


        Field<B> getField(const int & row) override {
            int8_t *src =  this->column_data_ + col_field_size_bytes_ * row;
            QueryFieldDesc desc = this->parent_table_->getSchema().getField(this->column_idx_);
            return Field<B>::deserialize(desc, src);

        }

        void setField(const int & row, const Field<B> & f) override {
            int8_t *dst = this->column_data_ + col_field_size_bytes_ * row;
            Field<B>::writeField(dst, f, this->parent_table_->getSchema().getField(this->column_idx_));
        }

        ColumnEncodingModel columnEncoding() override { return ColumnEncodingModel::PLAIN; }
        void resize(const int & tuple_cnt) override {
            this->parent_table_->column_data_[this->column_idx_].resize(tuple_cnt * col_field_size_bytes_);
        }
        void compress(QueryTable<B> *src, const int & src_col) override {
            auto dst_size = col_field_size_bytes_ * this->parent_table_->tuple_cnt_;
            auto src_ptr = src->column_data_.at(src_col).data();
            memcpy(this->column_data_, src_ptr, dst_size);
        }

        void secretShare(const int & party, QueryTable<Bit> *dst, QueryTable<Bit> *dst_col) override {
            throw std::invalid_argument("NYI!");
        }

    private:
        int col_field_size_bytes_;

    };

}

#endif
