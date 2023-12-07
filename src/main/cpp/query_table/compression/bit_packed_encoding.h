#ifndef _BIT_PACKED_ENCODING_
#define _BIT_PACKED_ENCODING_

#include "column_encoding.h"
#include "query_table/query_table.h"
#include "util/field_utilities.h"
#include "plain_encoding.h"

namespace vaultdb {

    // compress secret shared columns by suppressing leading null bits
    class BitPackedEncoding : public ColumnEncoding<Bit> {
    public:
        FieldType field_type_;
        int64_t field_min_;

        // just a copy constructor - compression all happens during secret sharing
        BitPackedEncoding(QueryTable<Bit> *dst, const int & dst_col, QueryTable<Bit> *src, const int & src_col)  : ColumnEncoding<Bit>(dst, dst_col) {
                int dst_size = this->parent_table_->tuple_cnt_ * this->field_size_bytes_;
                memcpy(this->column_data_, src->column_data_[src_col].data(), dst_size);
        }

        BitPackedEncoding(QueryTable<Bit> *parent, const int & col_idx) : ColumnEncoding<Bit>(parent, col_idx) {
            field_type_  = this->parent_table_->getSchema().getField(this->column_idx_).getType();
            assert(field_type_ == FieldType::SECURE_INT || field_type_ == FieldType::SECURE_LONG);
            auto dst_size = this->field_size_bytes_ * parent->tuple_cnt_;

            if(parent->column_data_.find(col_idx) == parent->column_data_.end()) {
                parent_table_->column_data_.insert({col_idx, vector<int8_t>(dst_size)});
                memset(parent->column_data_[col_idx].data(), 0, dst_size);
            }
            else if(parent->column_data_[col_idx].size() != dst_size){
                parent->column_data_[col_idx].resize(dst_size);
            }

            this->column_data_ = parent->column_data_[col_idx].data();
            field_min_ = parent->getSchema().getField(col_idx).getFieldMin();
        }

        // copy constructor
        BitPackedEncoding(const BitPackedEncoding &src)  : ColumnEncoding<Bit>(src) {
            field_type_ = src.field_type_;
            field_min_ = src.field_min_;
        }

        Field<Bit> getField(const int & row) override {
            return Field<Bit>(field_type_, getInt(row));
        }

        Field<Bit> getDecompressedField(const int & row) override {
            Integer i = getInt(row);
            int dst_size = (field_type_ == FieldType::SECURE_INT) ? 32 : 64;
            i.resize(dst_size);
            i = i + Integer(dst_size, field_min_, PUBLIC);
            return Field<Bit>(field_type_, i);
        }

        void setField(const int & row, const Field<Bit> & f) override {
            auto dst = getFieldPtr(row);
            Integer src = f.getInt();
            memcpy(dst, src.bits.data(), field_size_bytes_);
        }


        ColumnEncoding<Bit> *clone(QueryTable<Bit> *dst, const int & dst_col) override {
            assert(dst->tuple_cnt_ <= this->parent_table_->tuple_cnt_);

            auto dst_encoding = new BitPackedEncoding(*this);
            dst_encoding->parent_table_ = dst;
            dst_encoding->column_data_ = dst->column_data_.at(dst_col).data();
            dst_encoding->column_idx_ = dst_col;
            auto dst_ptr = dst_encoding->column_data_;
            int cnt = this->parent_table_->tuple_cnt_;
            memcpy(dst_ptr, this->column_data_, field_size_bytes_ * cnt);
            return dst_encoding;
        }


        CompressionScheme columnEncoding() const override { return CompressionScheme::BIT_PACKED; }

        void resize(const int & tuple_cnt) override {
            vector<int8_t> & dst = this->parent_table_->column_data_[this->column_idx_]; // reference to column data
            dst.resize(tuple_cnt * field_size_bytes_);
            this->column_data_ = dst.data();
        }




        void revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party) override;


        void secretShare(QueryTable<Bit> *dst, const int & dst_col) override;

        void initializeColumn(const SecureField & field) override {
            auto field_type = parent_table_->getSchema().getField(this->column_idx_).getType();
            assert(field_type == field.getType());
            Integer init = field.getInt();
            int cnt = this->parent_table_->tuple_cnt_;
            Bit *dst = (Bit *) this->column_data_;

            for(int i = 0; i < cnt; ++i) {
                memcpy(dst, init.bits.data(), this->field_size_bytes_);
                dst += field_size_bits_;
            }
        }

        PlainEncoding<Bit> *decompress(QueryTable<Bit> *dst, const int &dst_col) override {
            auto dst_encoding = new PlainEncoding<Bit>(dst, dst_col);
            auto dst_ptr = dst_encoding->column_data_; // in bytes
            int cnt = this->parent_table_->tuple_cnt_;
            auto src =  this->column_data_;
            int32_t dst_bit_cnt = (field_type_ == FieldType::SECURE_INT) ? 32 : 64;
            Integer dst_field_min(dst_bit_cnt, field_min_, PUBLIC);

            for(int i = 0; i < cnt; ++i) {
                Integer packed(this->field_size_bits_, 0, PUBLIC);
                memcpy(packed.bits.data(), src, field_size_bytes_);
                packed.resize(dst_bit_cnt);
                packed = packed + dst_field_min;
                memcpy(dst_ptr, packed.bits.data(), dst_encoding->field_size_bytes_);
                dst_ptr += dst_encoding->field_size_bytes_;
                src += field_size_bytes_;
            }

            return dst_encoding;
        }

    private:


        // based on emp::reveal
        void revealBitPackedInts(int32_t *dst, emp::Bit *src, const int & row_cnt, const int & party = PUBLIC);
        void revealBitPackedInts(int64_t *dst, emp::Bit *src, const int & row_cnt, const int & party = PUBLIC);


    };
}

#endif //_BIT_PACKED_ENCODING_