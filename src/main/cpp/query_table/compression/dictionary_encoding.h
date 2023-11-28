#ifndef _DICTIONARY_ENCODING_
#define _DICTIONARY_ENCODING_

#include "column_encoding.h"
#include "plain_encoding.h"
#include "compressed_table.h"

namespace vaultdb {
    template<typename B>
    class DictionaryEncoding : public ColumnEncoding<B> {
    public:
        // when operating under MPC, we need a way to know the order in which we added elements to the dictionary
        map<int, vector<int8_t> > dictionary_idxs_;
        map<vector<int8_t>, Field<B> > dictionary_;
        map<Field<B>, vector<int8_t> > reverse_dictionary_;
        int32_t dictionary_entry_cnt_ = 0; // count of entries in dictionary
        QueryFieldDesc desc_;

        DictionaryEncoding<B>(QueryTable<B> *dst, const int &dst_col, const QueryTable<B> *src, const int &src_col);

        // copy constructor, remapping it onto a new table
        DictionaryEncoding(QueryTable<B> *dst, const int & dst_col, DictionaryEncoding<B> & src);

        // this is just for setup, for use in secretShare method
        // initialize compression there
        DictionaryEncoding(QueryTable<B> *dst, const int &dst_col)  : ColumnEncoding<B>(dst, dst_col) {
            ((CompressedTable<B> *) dst)->column_encodings_[dst_col] = this;
        }

        Field<B> getField(const int &row) override;

        Field<B> getDecompressedField(const int &row) override;

        void setField(const int &row, const Field<B> &f) override;


        // clone a pre-existing compressed column
        ColumnEncoding<B> *clone(QueryTable<B> *dst, const int &dst_col) override {
            return new DictionaryEncoding<B>(dst, dst_col, *this);
        }


        CompressionScheme columnEncoding() override {
            return CompressionScheme::DICTIONARY;
        }


        void revealInsecure(QueryTable<bool> *dst, const int &dst_col, const int &party) override;

        void secretShare(QueryTable<Bit> *dst, const int &dst_col) override;

        void initializeColumn(const Field<B> &field) override {
            // not supported here because if all fields have the same value then we probably need to initialize and build a dictionary first!
            throw;
        }

        PlainEncoding<B> *decompress(QueryTable<B> *dst, const int &dst_col) override {
            // unlikely to do this when B == Bit - just including it for completeness
            // can do this obliviously but it's an n^2 operation
            assert(dst->tuple_cnt_ == this->parent_table_->tuple_cnt_);

            auto dst_encoding = (PlainEncoding<B> *) ColumnEncoding<B>::getColumnEncoding(dst, dst_col);
            assert(dst_encoding->columnEncoding() == CompressionScheme::PLAIN);
            for(int i = 0; i < dst->tuple_cnt_; ++i) {
                dst_encoding->setField(i, this->getDecompressedField(i));
            }
            return dst_encoding;
        }


    };


}


#endif
