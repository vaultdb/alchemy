#ifndef _BIT_PACKED_ENCODING_
#define _BIT_PACKED_ENCODING_

#include "column_encoding.h"
#include "query_table/query_table.h"


namespace vaultdb {

    // compress secret shared columns by suppressing leading null bits
    class BitPackedEncoding : public ColumnEncoding<Bit> {
    public:
        FieldType field_type_;
        int64_t field_min_;


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
            Bit *src =  (Bit *) (this->column_data_ + row * this->field_size_bytes_);
            emp::Integer dst(this->field_size_bits_ + 1, 0, PUBLIC);

            memcpy(dst.bits.data(), src, field_size_bytes_);
            return Field<Bit>(field_type_, dst);
        }

        void setField(const int & row, const Field<Bit> & f) override {
            int8_t *dst = this->column_data_ + row * this->field_size_bytes_;
            Integer src = f.getInt();
            memcpy(dst, src.bits.data(), field_size_bytes_);
        }

        void deserializeField(const int & row, const int8_t *src) override {
            int8_t *dst = this->column_data_ + field_size_bytes_ * row;
            memcpy(dst, src, field_size_bytes_);
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

        void cloneColumn(const int & dst_idx, QueryTable<Bit> *src, const int & src_col, const int & src_idx) override;
        void cloneField(const int & dst_row, const QueryTable<Bit> *src, const int & src_row, const int & src_col) override;
        ColumnEncodingModel columnEncoding() override { return ColumnEncodingModel::BIT_PACKED; }

        void resize(const int & tuple_cnt) override {
            vector<int8_t> & dst = this->parent_table_->column_data_[this->column_idx_]; // reference to column data
            dst.resize(tuple_cnt * field_size_bytes_);
            this->column_data_ = dst.data();
        }

        void compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) override {
            Bit *lhs = (Bit *) (this->column_data_ + lhs_row * this->field_size_bytes_);
            Bit *rhs = (Bit *) (this->column_data_ + rhs_row * this->field_size_bytes_);

            for(int i = 0; i < this->field_size_bits_; ++i) {
                emp::swap(swap, *lhs, *rhs);
                ++lhs;
                ++rhs;
            }
        }
        void revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party) override;

        // not implemented because there is no plaintext compression phase to this technique
        void compress(QueryTable<Bit> *src, const int & src_col) override {
        }

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

    private:

        // based on emp::reveal
        void revealBitPackedInts(int32_t *dst, emp::Bit *src, const int & row_cnt, const int & party = PUBLIC);
        void revealBitPackedInts(int64_t *dst, emp::Bit *src, const int & row_cnt, const int & party = PUBLIC);


    };
}

#endif //_BIT_PACKED_ENCODING_