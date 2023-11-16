#include "plain_encoding.h"
#include "compressed_table.h"

using namespace vaultdb;

template<typename B>
void PlainEncoding<B>::secretShare(QueryTable<Bit> *dst, const int &dst_col) {
    // no bit packing, just taking a plaintext column and converting it into a secret shared one
    // assume source compression scheme is same as dst compression scheme
    assert(dst->storageModel() == StorageModel::COMPRESSED_STORE);

    SystemConfiguration &s = SystemConfiguration::getInstance();
    auto  manager = s.emp_manager_;
    int sending_party = manager->sendingParty();
    assert(sending_party != 0); // only want to compress data from one party, not split over 2 or more secret sharing hosts
    bool sender = (s.party_ == sending_party);

    FieldType f_type = this->parent_table_->getSchema().getField(this->column_idx_).getType();
    int row_cnt = dst->tuple_cnt_;
    // need true column because may be uninitialized on computing parties
    int dst_col_size_bits = TypeUtilities::getTypeSize(this->parent_table_->getSchema().getField(this->column_idx_).getType());

    if(dst->getSchema().getField(dst_col).bitPacked()) {
        secretShareForBitPacking(dst, dst_col);
        return;
    }

    switch(f_type) {
        case FieldType::BOOL: {
            bool *src_ptr = (bool *) this->column_data_;
            Bit *dst_ptr = (Bit *) dst->column_data_.at(dst_col).data();
//            cout << "Secret sharing " << row_cnt << " bits for column " << this->column_idx_ << endl;
            if (sender)
                manager->feed(dst_ptr, sending_party, src_ptr, row_cnt);
            else
                manager->feed(dst_ptr, sending_party, nullptr, row_cnt);
            break;
        }
        case FieldType::STRING: {
            // temporarily reverse all strings and then follow process of INT/LONG
            int str_len = dst->getSchema().getField(dst_col).getStringLength();
            dst_col_size_bits *= str_len;
            if(sender) {
                ColumnEncoding<B>::reverseStrings((char *) this->column_data_, str_len, row_cnt);
            }
            // fall through to INT/LONG
        }
        case FieldType::INT:
        case FieldType::LONG:
        {
            // just feed it and write
            int col_byte_cnt = dst_col_size_bits / 8;
            int byte_cnt = row_cnt * col_byte_cnt;
            Bit *dst_ptr = (Bit *) dst->column_data_.at(dst_col).data();

            if (sender) {
                auto bools = Utilities::bytesToBool(this->column_data_, byte_cnt);
                manager->feed(dst_ptr, sending_party, bools, dst_col_size_bits * row_cnt);
                delete[] bools;
            } else {
                manager->feed(dst_ptr, sending_party, nullptr,  dst_col_size_bits * row_cnt);
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

    if(f_type == FieldType::STRING && sender) {
        // undo string reversal
        int str_len = this->parent_table_->getSchema().getField(dst_col).getStringLength();
        ColumnEncoding<B>::reverseStrings((char *) this->column_data_, str_len, row_cnt);
    }
}

template<typename B>
void PlainEncoding<B>::secretShareForBitPacking(QueryTable<Bit> *dst, const int &dst_col) {
    assert(dst->storageModel() == StorageModel::COMPRESSED_STORE);

    SystemConfiguration &s = SystemConfiguration::getInstance();
    auto  manager = s.emp_manager_;
    int sending_party = manager->sendingParty();
    assert(sending_party != 0); // only want to compress data from one party, not split over 2 or more secret sharing hosts
    bool sender = (s.party_ == sending_party);

    auto dst_table = (CompressedTable<Bit> *) dst;
    auto dst_encoding = (BitPackedEncoding *)  dst_table->column_encodings_.at(dst_col);
    int field_size_bits = dst_encoding->field_size_bits_;
    int row_cnt = dst->tuple_cnt_;
    FieldType f_type = dst_encoding->field_type_;

    if(sender) {
        bool *bools = new bool[field_size_bits * row_cnt];

        if (f_type == FieldType::SECURE_INT) {
            serializeForBitPacking<int32_t>(bools, (int32_t *) this->column_data_, row_cnt, field_size_bits, dst_encoding->field_min_);
        }
        else if(f_type == FieldType::SECURE_LONG){
            serializeForBitPacking<int64_t>(bools, (int64_t *) this->column_data_, row_cnt, field_size_bits, dst_encoding->field_min_);
        }

        manager->feed((Bit *) dst_encoding->column_data_, sending_party, bools, field_size_bits * row_cnt);
        delete [] bools;
    }
    else {
        manager->feed((Bit *) dst_encoding->column_data_, sending_party, nullptr,  field_size_bits * row_cnt);
    }

}

template<typename B>
void PlainEncoding<B>::revealInsecure(QueryTable<bool> *dst, const int & dst_col, const int & party) {
    int row_cnt = this->parent_table_->tuple_cnt_;
    FieldType f_type = this->parent_table_->getSchema().getField(this->column_idx_).getType();
    SystemConfiguration &s = SystemConfiguration::getInstance();
    auto manager = s.emp_manager_;

    assert(dst->storageModel() == StorageModel::COMPRESSED_STORE);
    auto dst_table = (CompressedTable<bool> *) dst;
    assert(dst_table->column_encodings_.at(dst_col)->columnEncoding() == ColumnEncodingModel::PLAIN);

    switch(f_type) {
        case FieldType::SECURE_BOOL: {
            Bit *src_ptr = (Bit *) this->column_data_;
            bool *dst_ptr = (bool *) dst->column_data_.at(dst_col).data();
            manager->reveal(dst_ptr, party, src_ptr, row_cnt);
            break; }
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            int byte_cnt = row_cnt * dst->field_sizes_bytes_[dst_col];
            Bit *src_ptr = (Bit *) this->column_data_;
            int8_t *dst_ptr = dst->column_data_.at(dst_col).data();
            ColumnEncoding<B>::revealToBytes(dst_ptr, src_ptr, byte_cnt, party);
            break;
        }
        case FieldType::SECURE_STRING: {
            // first batch reveal all bytes
            Bit *src_ptr = (Bit *) this->column_data_;
            int8_t *dst_ptr = dst->column_data_.at(dst_col).data();
            int str_len =  dst->getSchema().getField(dst_col).getStringLength();
            ColumnEncoding<B>::revealToBytes(dst_ptr, src_ptr, row_cnt * str_len, party);
            // reverse the string at the end
            for(int i = 0; i < row_cnt; ++i) {
                string str(str_len, 0);
                memcpy((int8_t *) str.c_str(), dst_ptr + i * str_len, str_len);
                std::reverse(str.begin(), str.end());
                memcpy(dst_ptr + i * str_len, (int8_t *) str.c_str(), str_len);
            }
            break;
        }
        case FieldType::SECURE_FLOAT: {
            Float *src_flts = (Float *) this->column_data_;
            float *dst_ptr = (float *) dst->column_data_.at(dst_col).data();
            for(int i = 0; i < row_cnt; ++i) {
                Float flt = src_flts[i];
                dst_ptr[i] = flt.reveal<double>();
            }
            break;
        }

        default:
            throw;

    }

}




template class vaultdb::PlainEncoding<bool>;
template class vaultdb::PlainEncoding<emp::Bit>;
