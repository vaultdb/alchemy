#ifndef _PLAIN_ENCODING_
#define _PLAIN_ENCODING_
#include "column_encoding.h"
#include "query_table/query_table.h"
#include "util/field_utilities.h"

namespace vaultdb {
    // Column encoding with no compression
    template<typename B>
    class PlainEncoding : public ColumnEncoding<B> {
        public:
        // this is mostly a placeholder to support heterogeneous (one compression scheme / table) projections of a table
        PlainEncoding<B>(QueryTable<B> *parent, const int & col_idx) : ColumnEncoding<B>(parent, col_idx) {
            // "true" type size, use PackedEncoding for bit packing
            QueryFieldDesc desc = this->parent_table_->getSchema().getField(this->column_idx_);
            auto f_type  = desc.getType();
            this->field_size_bits_ = TypeUtilities::getTypeSize(f_type); // size in bits, no bit packing overrides parent constructor

            if(f_type == FieldType::STRING || f_type == FieldType::SECURE_STRING) {
                this->field_size_bits_ *= desc.getStringLength();
            }

            if(desc.bitPacked() && std::is_same_v<B, Bit>) this->field_size_bytes_ = this->field_size_bits_ * sizeof(emp::Bit);

            auto dst_size_bytes = this->field_size_bytes_ * parent->tuple_cnt_;
            parent->column_data_[col_idx] = vector<int8_t>(dst_size_bytes);
            this->column_data_ = parent->column_data_[col_idx].data();
            memset(this->column_data_, 0, dst_size_bytes);
        }

        PlainEncoding(const PlainEncoding<B> &src) : ColumnEncoding<B>(src) {
            int dst_size = this->parent_table_->tuple_cnt_ * this->field_size_bytes_;
            memcpy(this->column_data_, src.column_data_, dst_size);
        }

        Field<B> getField(const int & row) override {
            int8_t *src =  this->column_data_ + this->field_size_bytes_ * row;
            QueryFieldDesc desc = this->parent_table_->getSchema().getField(this->column_idx_);
            return Field<B>::deserialize(desc, src);
        }

        void setField(const int & row, const Field<B> & f) override {
            int8_t *dst = this->column_data_ + this->field_size_bytes_ * row;
            Field<B>::writeField(dst, f, this->parent_table_->getSchema().getField(this->column_idx_));
        }

        void deserializeField(const int & row, const int8_t *src) override {
            int8_t *dst = this->column_data_ + this->field_size_bytes_ * row;
            memcpy(dst, src, this->field_size_bytes_);
        }

        ColumnEncodingModel columnEncoding() override { return ColumnEncodingModel::PLAIN; }

        void resize(const int & tuple_cnt) override {
            vector<int8_t> & dst = this->parent_table_->column_data_[this->column_idx_]; // reference to column data
            dst.resize(tuple_cnt * this->field_size_bytes_);
            this->column_data_ = dst.data();
        }
        void compress(QueryTable<B> *src, const int & src_col) override {
            auto dst_size = this->field_size_bytes_ * this->parent_table_->tuple_cnt_;
            auto src_ptr = src->column_data_.at(src_col).data();
            memcpy(this->column_data_, src_ptr, dst_size);
        }

        void secretShare(QueryTable<Bit> *dst, const int & dst_col) override;

        void revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party = PUBLIC) override;

        ColumnEncoding<B> *clone(QueryTable<B> *dst, const int & dst_col) override {
            assert(dst->tuple_cnt_ == this->parent_table_->tuple_cnt_);
            auto dst_encoding = new PlainEncoding<B>(dst, dst_col);
            memcpy(dst_encoding->column_data_, this->column_data_, this->parent_table_->tuple_cnt_ * this->field_size_bytes_);
            return dst_encoding;
        }

        void cloneColumn(const int & dst_idx, QueryTable<B> *s, const int & src_col, const int & src_idx) override;
        void cloneField(const int & dst_row, const QueryTable<B> *src, const int & src_row, const int & src_col) override;
        void initializeColumn(const Field<B> & field) override {
            auto desc = this->parent_table_->getSchema().getField(this->column_idx_);
            auto field_type = desc.getType();
            assert(field_type == field.getType());
            int8_t *dst = this->column_data_;
            int cnt = this->parent_table_->tuple_cnt_;


            switch(field_type) {
                case FieldType::BOOL: {
                    bool b = field.template getValue<bool>();
                    memset(dst, b, cnt);
                    break;
                }
                case FieldType::INT: {
                    initializeHelper<int32_t>(field, dst);
                    break;
                }
                case FieldType::LONG: {
                    initializeHelper<int64_t>(field, dst);
                    break;
                }
                case FieldType::FLOAT: {
                    initializeHelper<float>(field, dst);
                    break;
                }
                case FieldType::STRING: {
                    string str = field.template getValue<string>();
                    auto cursor = dst;
                    for(int i = 0; i < this->parent_table_->tuple_cnt_; ++i) {
                        memcpy(cursor, str.c_str(), this->field_size_bytes_);
                        dst += this->field_size_bytes_;
                    }
                }
                case FieldType::SECURE_BOOL: {
                    initializeHelper<emp::Bit>(field, dst);
                    break;
                }
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING: {
                    initializeHelper<Integer>(field, dst);
                    break;
                }
                case FieldType::SECURE_FLOAT: {
                    initializeHelper<Float>(field, dst);
                    break;
                }
                default:
                    throw;
            }

        }

    private:

        void secretShareForBitPacking(QueryTable<Bit> *dst_table, const int & dst_col);
        // serialize a column of type T into a bit array where each T will be projected down to field_bit_cnt bits.
        template <typename T>
         void serializeForBitPacking(bool *dst, T *src, int row_cnt, int field_bit_cnt, T field_min) {
            bool b[field_bit_cnt];
            auto write_ptr = dst;

            for (int i = 0; i < row_cnt; ++i) {
                T to_share = src[i] - field_min;
                emp::int_to_bool<T>(b, to_share, field_bit_cnt);
                memcpy(write_ptr, b, field_bit_cnt); // still plaintext
                write_ptr += field_bit_cnt;
            }
        }

        template<typename T>
        void initializeHelper(const Field<B> & field, int8_t *dst) {
            T t = field.template getValue<T>();
            auto cursor = dst;
            auto cnt = this->parent_table_->tuple_cnt_;

            for(int i = 0; i < cnt; ++i) {
                memcpy(cursor, &t, this->field_size_bytes_);
                cursor += this->field_size_bytes_;
            }
        }
    };




}


#endif
