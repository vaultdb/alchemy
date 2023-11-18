#ifndef _BIT_PACKED_ENCODING_
#define _BIT_PACKED_ENCODING_

#include "column_encoding.h"
#include "query_table/query_table.h"


namespace vaultdb {

    // compress secret shared columns by suppressing leading null bits
    class BitPackedEncoding : public ColumnEncoding<Bit> {
    public:
        FieldType field_type_;
        int field_size_bits_ = 0;
        int64_t field_min_;


        BitPackedEncoding(QueryTable<Bit> *parent, const int & col_idx) : ColumnEncoding<Bit>(parent, col_idx) {
            field_type_  = this->parent_table_->getSchema().getField(this->column_idx_).getType();
            assert(field_type_ == FieldType::SECURE_INT || field_type_ == FieldType::SECURE_LONG);
            field_size_bits_ = parent->getSchema().getField(col_idx).size();
            int col_field_size_bytes = field_size_bits_ * sizeof(emp::Bit);
            auto dst_size = col_field_size_bytes * parent->tuple_cnt_;
            if(parent->column_data_.find(col_idx) == parent->column_data_.end()) {
                parent_table_->column_data_.insert({col_idx, vector<int8_t>(dst_size)});
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
            field_size_bits_ = src.field_size_bits_;
            field_min_ = src.field_min_;
        }

        // just unpacking it in its entirety for now
        // TODO: support getting a subset of the bits
        Field<Bit> getField(const int & row) override {
            Bit *src =  (Bit *) (this->column_data_ + field_size_bits_ * row * sizeof(emp::Bit));
            emp::Integer dst(this->field_size_bits_ + 1, 0, PUBLIC);

            memcpy(dst.bits.data(), src, field_size_bits_ * sizeof(emp::Bit));
            return Field<Bit>(field_type_, dst);
        }

        void setField(const int & row, const Field<Bit> & f) override {
            int8_t *dst = this->column_data_ + field_size_bits_ * row * sizeof(emp::Bit);
            emp::Integer src = f.template getValue<emp::Integer>();
            memcpy(dst, src.bits.data(), field_size_bits_ * sizeof(emp::Bit));
        }

        void deserializeField(const int & row, const int8_t *src) override {
            int8_t *dst = this->column_data_ + field_size_bits_ * row * sizeof(emp::Bit);
            memcpy(dst, src, field_size_bits_ * sizeof(emp::Bit));
        }

        ColumnEncoding<Bit> *clone(QueryTable<Bit> *dst, const int & dst_col) override {
            auto dst_encoding = new BitPackedEncoding(*this);
            dst_encoding->parent_table_ = dst;
            dst_encoding->column_data_ = dst->column_data_.at(dst_col).data();
            dst_encoding->column_idx_ = dst_col;
            auto dst_ptr = dst_encoding->column_data_;
            memcpy(dst_ptr, this->column_data_, this->field_size_bits_ * this->parent_table_->tuple_cnt_ * sizeof(emp::Bit));
            return dst_encoding;
        }

        void cloneColumn(const int & dst_idx, QueryTable<Bit> *src, const int & src_col, const int & src_idx) override;

        ColumnEncodingModel columnEncoding() override { return ColumnEncodingModel::BIT_PACKED; }

        void resize(const int & tuple_cnt) override {
            vector<int8_t> & dst = this->parent_table_->column_data_[this->column_idx_]; // reference to column data
            dst.resize(tuple_cnt * field_size_bits_ * sizeof(emp::Bit));
            this->column_data_ = dst.data();
        }

        void revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party) override;

        // not implemented because there is no plaintext compression phase to this technique
        void compress(QueryTable<Bit> *src, const int & src_col) override {
           throw;
        }

        void secretShare(QueryTable<Bit> *dst, const int & dst_col) override;

    private:
        // for int32_t or int64_t
        // based on emp::reveal
        template<typename T>
        void revealBitPackedInts(T *dst, emp::Bit *src, const int & row_cnt, const int & party = PUBLIC);



    };
}

#endif //_BIT_PACKED_ENCODING_