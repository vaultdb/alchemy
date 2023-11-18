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
            // "true" type size, use PackedEncoding for bit packing
            auto f_type  = this->parent_table_->getSchema().getField(this->column_idx_).getType();
            int field_size_bits = TypeUtilities::getTypeSize(this->parent_table_->getSchema().getField(this->column_idx_).getType()); // size in bits
            if(f_type == FieldType::STRING || f_type == FieldType::SECURE_STRING) {
                field_size_bits *= this->parent_table_->getSchema().getField(this->column_idx_).getStringLength();
            }

            field_size_bytes_ = (std::is_same_v<B, bool>) ? (field_size_bits / 8) : (field_size_bits * sizeof(emp::Bit));

            auto dst_size = field_size_bytes_ * parent->tuple_cnt_;
            parent->column_data_[col_idx] = vector<int8_t>(dst_size);
            this->column_data_ = parent->column_data_[col_idx].data();
        }

        PlainEncoding(const PlainEncoding<B> &src) : ColumnEncoding<B>(src) {
            field_size_bytes_ = src.field_size_bytes_;
        }

        Field<B> getField(const int & row) override {
            int8_t *src =  this->column_data_ + field_size_bytes_ * row;
            QueryFieldDesc desc = this->parent_table_->getSchema().getField(this->column_idx_);
            return Field<B>::deserialize(desc, src);
        }

        void setField(const int & row, const Field<B> & f) override {
            int8_t *dst = this->column_data_ + field_size_bytes_ * row;
            Field<B>::writeField(dst, f, this->parent_table_->getSchema().getField(this->column_idx_));
        }

        void deserializeField(const int & row, const int8_t *src) override {
            int8_t *dst = this->column_data_ + field_size_bytes_ * row;
            memcpy(dst, src, field_size_bytes_);
        }

        ColumnEncodingModel columnEncoding() override { return ColumnEncodingModel::PLAIN; }
        void resize(const int & tuple_cnt) override {
            vector<int8_t> & dst = this->parent_table_->column_data_[this->column_idx_]; // reference to column data
            dst.resize(tuple_cnt * field_size_bytes_);
            this->column_data_ = dst.data();
        }
        void compress(QueryTable<B> *src, const int & src_col) override {
            auto dst_size = field_size_bytes_ * this->parent_table_->tuple_cnt_;
            auto src_ptr = src->column_data_.at(src_col).data();
            memcpy(this->column_data_, src_ptr, dst_size);
        }

        void secretShare(QueryTable<Bit> *dst, const int & dst_col) override;

        void revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party = PUBLIC) override;

        ColumnEncoding<B> *clone(QueryTable<B> *dst, const int & dst_col) override {
            assert(dst->tuple_cnt_ == this->parent_table_->tuple_cnt_);
            PlainEncoding<B> *dst_encoding = new PlainEncoding<B>(dst, dst_col);
            memcpy(dst_encoding->column_data_, this->column_data_, this->parent_table_->tuple_cnt_ * field_size_bytes_);
            return dst_encoding;
        }

        void cloneColumn(const int & dst_idx, QueryTable<B> *s, const int & src_col, const int & src_idx) override;

    private:
        int field_size_bytes_;

        void secretShareForBitPacking(QueryTable<Bit> *dst_table, const int & dst_col);
        // serialize a column of type T into a bit array where each T will be projected down to field_bit_cnt bits.
        template <typename T>
         void serializeForBitPacking(bool *dst, T *src, int row_cnt, int field_bit_cnt, T field_min) {
            bool b[field_bit_cnt];
            auto read_ptr = src;
            auto write_ptr = dst;

            for (int i = 0; i < row_cnt; ++i) {
                T to_share = *read_ptr - field_min;
                emp::int_to_bool<int32_t>(b, to_share, field_bit_cnt);
                memcpy(write_ptr, b, field_bit_cnt); // still plaintext
                ++read_ptr;
                write_ptr += field_bit_cnt;
            }
        }
    };



}


#endif
