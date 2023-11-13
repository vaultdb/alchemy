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

        void secretShare(QueryTable<Bit> *dst, const int & dst_col) override {
           // no bit packing, just taking a plaintext column and converting it into a secret shared one
           // assume source compression scheme is same as dst compression scheme
           assert(dst->storageModel() == StorageModel::COMPRESSED_STORE);

           SystemConfiguration &s = SystemConfiguration::getInstance();
           int sending_party = s.emp_manager_->sendingParty();
           assert(sending_party == 0); // only want to compress data from one party, not split over 2 or more secret sharing hosts
           bool sender = (s.party_ == sending_party);
           FieldType f_type = this->parent_table_->getSchema().getField(this->column_idx_).getType();
           EmpManager *manager = s.emp_manager_;
           int row_cnt = this->parent_table_->tuple_cnt_;

           switch(f_type) {
               case FieldType::BOOL: {
                   bool *src_ptr = (bool *) this->column_data_;
                   Bit *dst_ptr = (Bit *) dst->column_data_.at(dst_col).data();
                   if (sender)
                       manager->feed(dst_ptr, sending_party, src_ptr, row_cnt);
                   else
                       manager->feed(dst_ptr, sending_party, nullptr, row_cnt);
                   break;
               }
               case FieldType::INT:
               case FieldType::LONG: {
                   // just feed it and write
                   int byte_cnt = row_cnt * col_field_size_bytes_;
                   Bit *dst_ptr = (Bit *) dst->column_data_.at(dst_col).data();
                   if (sender) {
                       auto bools = Utilities::bytesToBool(this->column_data_, byte_cnt);
                       manager->feed(dst_ptr, sending_party, bools, byte_cnt * 8);
                       delete bools;
                   } else {
                       manager->feed(dst_ptr, sending_party, nullptr, byte_cnt * 8);
                   }
                   break;
               }
               case FieldType::STRING: {
                   Bit *write_cursor = (Bit *) dst->column_data_.at(dst_col).data();
                   size_t byte_cnt = row_cnt * col_field_size_bytes_;
                   int bit_cnt = byte_cnt * 8;
                   if (sender) {
                       int8_t *read_cursor = this->column_data_;
                       for (int i = 0; i < row_cnt; ++i) {
                           string str(byte_cnt, 0);
                           memcpy((int8_t *) str.c_str(), read_cursor, byte_cnt);
                           std::reverse(str.begin(), str.end());
                           bool *bools = Utilities::bytesToBool((int8_t *) str.c_str(), byte_cnt);
                           manager->feed(write_cursor, sending_party, bools, bit_cnt);
                           delete[] bools;
                           read_cursor += byte_cnt;
                           write_cursor += bit_cnt;
                       }
                   } else { // receiver
                       for (int i = 0; i < row_cnt; ++i) {
                           manager->feed(write_cursor, sending_party, nullptr, bit_cnt);
                           write_cursor += bit_cnt;
                       }
                   }
                   break;
               }
                   // assumes float size is 32 bits
               case FieldType::FLOAT: {
                   Bit *dst_ptr = (Bit *) dst->column_data_.at(dst_col).data();
                   if (sender) {
                       float *src_flts = (float *) this->column_data_;
                       for (int i = 0; i < row_cnt; ++i) {
                           Float flt(src_flts[i], sending_party);
                           memcpy(dst_ptr, flt.value.data(), 32 * sizeof(emp::Bit));
                           dst_ptr += 32;
                       }
                   } else {
                       for (int i = 0; i < row_cnt; ++i) {
                           manager->feed(dst_ptr, sending_party, nullptr, 32);
                           dst_ptr += 32;
                       }
                   }
                   break;
               }
               default:
                   throw std::invalid_argument("Can't secret share with type " + TypeUtilities::getTypeString(f_type));
           }


        }

    private:
        int col_field_size_bytes_;

    };

}

#endif
