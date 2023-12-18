#ifndef _PLAIN_DICTIONARY_ENCODING_H
#define _PLAIN_DICTIONARY_ENCODING_H

#include "column_encoding.h"
#include "dictionary_encoding.h"

namespace vaultdb {
    template<>
    class DictionaryEncoding<bool> : public ColumnEncoding<bool> {
    public:
        // when operating under MPC, we need a way to know the order in which we added elements to the dictionary
        map<int32_t, Field<bool> > dictionary_;
        map<Field<bool>, int32_t > reverse_dictionary_;
        int32_t dictionary_entry_cnt_ = 0; // count of entries in dictionary
        QueryFieldDesc desc_;

        DictionaryEncoding<bool>(QueryTable<bool> *dst, const int &dst_col, const QueryTable<bool> *src, const int &src_col) : ColumnEncoding<bool>(dst, dst_col) {
            int dst_size_bytes = this->parent_table_->tuple_cnt_ * this->field_size_bytes_;
            dst->column_data_[dst_col] = vector<int8_t>(dst_size_bytes);
            this->column_data_ = dst->column_data_[dst_col].data();
            memset(this->column_data_, 0, dst_size_bytes);
            assert(src->tuple_cnt_ == dst->tuple_cnt_);
            if(src->tuple_cnt_ == 0) return; // not sending party

            map<Field<bool>, int32_t> setup_dictionary;

            // count # of entries to assign codes to them
            for(int i = 0; i < src->tuple_cnt_; ++i) {
                auto field = src->getField(i, src_col);
                if(setup_dictionary.find(field) == setup_dictionary.end()) {
                    setup_dictionary[field] = setup_dictionary.size();
                }
            }

            dictionary_entry_cnt_ = setup_dictionary.size();
            field_size_bits_ = ceil(log2(dictionary_entry_cnt_));
            field_size_bytes_ = (field_size_bits_ / 8) + ((field_size_bits_ % 8) != 0); // round up
            desc_ = this->parent_table_->getSchema().getField(dst_col);

            int entry_cnt = src->tuple_cnt_;
            parent_table_->column_data_[column_idx_].resize(entry_cnt * field_size_bytes_);

            // initialize dictionary
            for(auto &pair : setup_dictionary) {
                dictionary_[pair.second] = pair.first;
                reverse_dictionary_[pair.first] = pair.second;
            }

            // compress the entries
            auto dst_ptr = this->column_data_;
            for(int i = 0; i < src->tuple_cnt_; ++i) {
                auto field = src->getField(i, src_col);
                auto key = reverse_dictionary_[field];
                memcpy(dst_ptr, &key, field_size_bytes_);
                dst_ptr += field_size_bytes_;
            }


        }

        // copy constructor, remapping it onto a new table
        DictionaryEncoding(QueryTable<bool> *dst, const int & dst_col, DictionaryEncoding<bool> & src)  : ColumnEncoding<bool>(dst, dst_col) {
            dictionary_ = src.dictionary_;
            reverse_dictionary_ = src.reverse_dictionary_;
            dictionary_entry_cnt_ = src.dictionary_entry_cnt_;
            desc_ = src.desc_;
            ((CompressedTable<bool> *) dst)->column_encodings_[dst_col] = this;

            int payload_bytes = src.parent_table_->tuple_cnt_ * src.field_size_bytes_;
            dst->column_data_[dst_col] = vector<int8_t>(payload_bytes);
            memcpy(dst->column_data_[dst_col].data(), src.parent_table_->column_data_[src.column_idx_].data(), payload_bytes);
        }

        // this is just for setup, for use in secretShare method
        // initialize compression there
        DictionaryEncoding(QueryTable<bool> *dst, const int &dst_col)  : ColumnEncoding<bool>(dst, dst_col) {
            ((CompressedTable<bool> *) dst)->column_encodings_[dst_col] = this;
        }

        CompressionScheme columnEncoding() const override {
            return CompressionScheme::DICTIONARY;
        }

        void initializeColumn(const Field<bool> &field) override {
            // not supported here because if all fields have the same value then we probably need to initialize and build a dictionary first!
            exit(0);
        }

        Field<bool> getField(const int &row) override {
            auto pos = getFieldPtr(row);
            int32_t dst;
            memcpy(&dst, pos, field_size_bytes_);
            return Field<bool>(FieldType::INT, dst);
        }

        Field<bool> getDecompressedField(const int &row) override {
            auto pos = getFieldPtr(row);
            int32_t dst;
            memcpy(&dst, pos, field_size_bytes_);
            return dictionary_[dst];
        }

        void setField(const int &row, const Field<bool> &f) override {
            int32_t src = f.getValue<int32_t>();
            auto pos = getFieldPtr(row);
            memcpy(pos, &src, field_size_bytes_);
        }


        // clone a pre-existing compressed column
        ColumnEncoding<bool> *clone(QueryTable<bool> *dst, const int &dst_col) override {
            return new DictionaryEncoding<bool>(dst, dst_col, *this);
        }





        void revealInsecure(QueryTable<bool> *dst, const int &dst_col, const int &party) override {
            auto dst_encoding = new DictionaryEncoding<bool>(dst, dst_col, *this);
        }

        void secretShare(QueryTable<Bit> *d, const int &dst_col) override;

        PlainEncoding<bool> *decompress(QueryTable<bool> *dst, const int &dst_col) override {
            assert(dst->tuple_cnt_ == this->parent_table_->tuple_cnt_);

            auto dst_encoding = (PlainEncoding<bool> *) ColumnEncoding<bool>::getColumnEncoding(dst, dst_col);

            for(int i = 0; i < dst->tuple_cnt_; ++i) {
                dst_encoding->setField(i, this->getDecompressedField(i));
            }
            return dst_encoding;
        }

    };


}
#endif