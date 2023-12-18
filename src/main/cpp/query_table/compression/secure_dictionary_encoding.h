#ifndef __SECURE_DICTIONARY_ENCODING_H__
#define __SECURE_DICTIONARY_ENCODING_H__

#include "dictionary_encoding.h"
#include "plain_dictionary_encoding.h"

namespace vaultdb {
    template<>
    class DictionaryEncoding<Bit> : public ColumnEncoding<Bit>  {

    public:
        // when operating under MPC, we need a way to know the order in which we added elements to the dictionary
        int32_t dictionary_entry_cnt_ = 0; // count of entries in dictionary
        QueryFieldDesc desc_;
        vector<pair<Integer, Field<Bit> > > dictionary_;

        DictionaryEncoding<Bit>(QueryTable<Bit> *dst, const int &dst_col, const QueryTable<Bit> *src, const int &src_col) : ColumnEncoding<Bit>(dst, dst_col) {
            throw; // can't compress under MPC, too many data-dependent branches!
        }

        // copy constructor, remapping it onto a new table
        DictionaryEncoding(QueryTable<Bit> *dst, const int & dst_col, DictionaryEncoding<Bit> & src) :  ColumnEncoding<Bit>(dst, dst_col) {
            dictionary_ = src.dictionary_;
            dictionary_entry_cnt_ = src.dictionary_entry_cnt_;
            desc_ = dst->getSchema().getField(dst_col);
            ((CompressedTable<Bit> *) dst)->column_encodings_[dst_col] = this;

            int payload_bytes = src.parent_table_->tuple_cnt_ * src.field_size_bytes_;

            dst->column_data_[dst_col] = vector<int8_t>(payload_bytes);
            memcpy(dst->column_data_[dst_col].data(), src.parent_table_->column_data_[src.column_idx_].data(), payload_bytes);
        }

        // this is just for setup, for use in secretShare method
        // initialize compression there
        DictionaryEncoding(QueryTable<Bit> *dst, const int &dst_col)  : ColumnEncoding<Bit>(dst, dst_col) {
            ((CompressedTable<Bit> *) dst)->column_encodings_[dst_col] = this;
        }

        CompressionScheme columnEncoding() const override {
            return CompressionScheme::DICTIONARY;
        }

        void initializeColumn(const Field<Bit> &field) override {
            // not supported here because if all fields have the same value then we probably need to initialize and build a dictionary first!
            exit(0);
        }


        Field<Bit> getField(const int &row) override {
            Integer i = getInt(row);
            return Field<Bit>(FieldType::SECURE_INT, i);
        }

        Field<Bit> getDecompressedField(const int &row) override {
            Integer encoded = getInt(row);
            Field<Bit> dst(desc_.getType());

            for(auto entry : dictionary_) {
                Bit b = (entry.first == encoded);
                dst = Field<Bit>::If(b, entry.second, dst);
            }

            return dst;
        }

        // input is compressed field
        void setField(const int &row, const Field<Bit> &f) override {
            Integer src = f.getInt();
            auto pos = (Bit *) this->getFieldPtr(row);
            memcpy(pos, src.bits.data(), src.size() * sizeof(Bit));
        }


        // clone a pre-existing compressed column
        ColumnEncoding<Bit> *clone(QueryTable<Bit> *dst, const int &dst_col) override {
            return new DictionaryEncoding<Bit>(dst, dst_col, *this);
        }

        void revealInsecure(QueryTable<bool> *dst, const int &dst_col, const int &party) override {
            auto dst_encoding = new DictionaryEncoding<bool>(dst, dst_col);
            dst_encoding->dictionary_entry_cnt_ = this->dictionary_entry_cnt_;
            dst_encoding->field_size_bits_ = this->field_size_bits_;
            dst_encoding->field_size_bytes_ = (this->field_size_bits_ / 8) +  ((this->field_size_bits_ % 8) != 0);
            int field_bit_cnt = dst_encoding->field_size_bits_; // shortcuts for readability

            auto manager = SystemConfiguration::getInstance().emp_manager_;

            // initialize the dictionary
            for(auto pos : dictionary_) {
                int32_t key = pos.first.reveal<int32_t>(party);
                Field<bool> value = pos.second.reveal(desc_, party);
                dst_encoding->dictionary_[key] = value;
                dst_encoding->reverse_dictionary_[value] = key;
            }

            // reveal the column
            int row_cnt = this->parent_table_->tuple_cnt_;
            int reveal_bit_cnt = row_cnt * field_bit_cnt;
            bool *bools = new bool[reveal_bit_cnt];
            manager->reveal(bools, party, (Bit *) this->column_data_, reveal_bit_cnt);
            //vector<int8_t> key(field_byte_cnt);
            int64_t key;

            for(int i = 0; i < row_cnt; ++i) {
                emp::from_bool(bools + i * dst_encoding->field_size_bits_, &key, dst_encoding->field_size_bits_, false);
                Field<bool> f = dst_encoding->dictionary_[key];
                dst_encoding->setField(i, f);
            }

            delete [] bools;

        }

        void secretShare(QueryTable<Bit> *d, const int &dst_col) override {
            // just clone what we already have
            auto dst_encoding = new DictionaryEncoding<Bit>(d, dst_col, *this);
            ((CompressedTable<Bit> *) d)->column_encodings_[dst_col] = dst_encoding;

        }

        Uncompressed<Bit> *decompress(QueryTable<Bit> *dst, const int &dst_col) override {
            assert(dst->tuple_cnt_ == this->parent_table_->tuple_cnt_);

            auto dst_encoding = (Uncompressed<Bit> *) ColumnEncoding<Bit>::getColumnEncoding(dst, dst_col);

            for(int i = 0; i < dst->tuple_cnt_; ++i) {
                dst_encoding->setField(i, this->getDecompressedField(i));
            }
            return dst_encoding;
        }


    };
}
#endif
