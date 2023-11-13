#ifndef _COMPRESSED_TABLE_
#define _COMPRESSED_TABLE_

#include "query_table/query_table.h"
#include "column_encoding.h"
#include "plain_encoding.h"

namespace vaultdb {

    //  need to support plain and MPC modes to facilitate compressing from plaintext data
    template <typename B>
    class CompressedTable : public QueryTable<B> {

        public:
        map<int, ColumnEncoding<B> *> column_encodings_; // algos for accessing compressed data in-situ

        CompressedTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def = SortDefinition()) : QueryTable<B>(tuple_cnt, schema, sort_def) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::COMPRESSED_STORE);
            setSchema(schema);

            if (tuple_cnt == 0)
                return;

            // each CompressedColumn has a pointer to column_data_ in parent table
            // column_data_ will be malloc'd according to compressed size
            for(int i = 0; i < schema.getFieldCount(); ++i) {
                column_encodings_[i] = new PlainEncoding<B>(this, i);
            }

            column_encodings_[-1] = new PlainEncoding<B>(this, -1); // dummy tag

        }

        CompressedTable(QueryTable<B> *src, const map<int, ColumnEncodingModel> & encodings) : QueryTable<B>(src->tuple_cnt_, src->getSchema(), src->order_by_) {
            assert(src->getSchema().getFieldCount() == encodings.size());
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::COMPRESSED_STORE);

            setSchema(src->getSchema());

            for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
              column_encodings_[i] = ColumnEncoding<B>::compress(src, i, this, i, encodings.at(i));
            }

        }

        explicit CompressedTable(const QueryTable<B> &src) : QueryTable<B>(src) {
            assert(SystemConfiguration::getInstance().storageModel() == StorageModel::COMPRESSED_STORE);
            setSchema(src.getSchema());
            resize(src.tuple_cnt_);
            // TODO: invoke constructors on column encodings here
        }

        ~CompressedTable() override {
            // just deletes the ColumnEncodings - leaves this->column_data_ intact in case pinned.
            for(auto &col : column_encodings_) {
                delete col.second;
            }
        }
        void resize(const size_t &tuple_cnt) override {
            for(auto pos : column_encodings_) {
                column_encodings_[pos.first]->resize(tuple_cnt);
            }
        }


        StorageModel storageModel() const override { return  StorageModel::COMPRESSED_STORE; }
        Field<B> getField(const int  & row, const int & col)  const override {
            return column_encodings_.at(col)->getField(row);
        }

        void setField(const int  & row, const int & col, const Field<B> & f) override {
            column_encodings_.at(col)->setField(row, f);
        }

        B getDummyTag(const int & row)  const override {
            return column_encodings_.at(-1)->getField(row).template getValue<B>(); }

        void setDummyTag(const int & row, const B & val) override {
            Field<B> f(TypeUtilities::getBoolType<B>(), val);
            column_encodings_.at(-1)->setField(row, f);
        }

        SecureTable *secretShare() override  {
            throw std::invalid_argument("NYI!");
        }

        void appendColumn(const QueryFieldDesc & desc) override {
            throw std::invalid_argument("NYI!");
        }


        // include dummy tag for this
        void cloneRow(const int & dst_row, const int & dst_col, const QueryTable<B> * src, const int & src_row) override {
            throw std::invalid_argument("NYI!");
        }

        void cloneRow(const B & write, const int & dst_row, const int & dst_col, const QueryTable<B> *src, const int & src_row) override {
            throw std::invalid_argument("NYI!");

        }
        void cloneTable(const int & dst_row, QueryTable<B> *src) override {
            throw std::invalid_argument("NYI!");

        }
        QueryTuple<B> getRow(const int & idx) override {

            throw std::invalid_argument("NYI!");
        }

        void setRow(const int & idx, const QueryTuple<B> & tuple) override {
            throw std::invalid_argument("NYI!");
        }
        QueryTable<B> *clone() override { return new CompressedTable<B>(*this); }
        void compareSwap(const B & swap, const int  & lhs_row, const int & rhs_row) override {}
        void cloneColumn(const int & dst_col, const int & dst_idx, const QueryTable<B> *src, const int & src_col, const int & src_offset = 0) override {}
        void setSchema(const QuerySchema &schema) override {
            // for now just using default settings for this
            this->schema_ = schema;
            int running_count = 0;
            int field_size_bytes;

            if(this->isEncrypted()) {
                // covers dummy tag as -1
                this->tuple_size_bytes_ = 0;
                QueryFieldDesc desc;

                for(auto pos : this->schema_.offsets_) {
                    desc = this->schema_.getField(pos.first);
                    field_size_bytes =  desc.size() * sizeof(emp::Bit);
                    this->tuple_size_bytes_ += field_size_bytes;
                    this->field_sizes_bytes_[pos.first] = field_size_bytes;
                    running_count += field_size_bytes;
                }

                return;
            }

            // plaintext case
            this->tuple_size_bytes_ = this->schema_.size() / 8; // bytes for plaintext
            for(auto pos : this->schema_.offsets_) {
                field_size_bytes = this->schema_.getField(pos.first).size() / 8;
                this->field_sizes_bytes_[pos.first] = field_size_bytes;
                running_count += field_size_bytes;
            }

        }


    };

//    template<typename B>
//    CompressedTable<B>::CompressedTable(const size_t &tuple_cnt, const QuerySchema &schema,
//                                        const SortDefinition &sort_def)
//            : QueryTable<B>(tuple_cnt, schema, sort_def) {
//    }
}
#endif
